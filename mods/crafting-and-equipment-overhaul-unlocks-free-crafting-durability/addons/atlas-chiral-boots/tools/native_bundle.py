"""Lossless private resource extension. Original game files are read-only inputs.

This builds data, not an installer. Outputs must be new files below the addon.
Native object numbers stay stable; private clones receive appended numbers.
"""
from dataclasses import dataclass
import hashlib
from pathlib import Path
import struct
import sys
import uuid

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT.parents[1] / 'scripts'))
from atlas_graph import Graph
from dsar_reader import DSAR


def murmur128(data, seed=42):
    """MurmurHash3 x64/128, Decima's seed. Byte order is little endian."""
    mask = (1 << 64)-1
    c1, c2 = 0x87c37b91114253d5, 0x4cf5ad432745937f
    def rot(x, n):
        return ((x << n) | (x >> (64-n))) & mask
    def mix1(x):
        return rot(x*c1 & mask, 31)*c2 & mask
    def mix2(x):
        return rot(x*c2 & mask, 33)*c1 & mask
    def fmix(x):
        x ^= x >> 33
        x = x*0xff51afd7ed558ccd & mask
        x ^= x >> 33
        x = x*0xc4ceb9fe1a85ec53 & mask
        return x ^ (x >> 33)
    h1 = h2 = seed & 0xffffffff
    end = len(data)//16*16
    for k1, k2 in struct.iter_unpack('<QQ', data[:end]):
        h1 = ((rot(h1 ^ mix1(k1), 27)+h2)*5+0x52dce729) & mask
        h2 = ((rot(h2 ^ mix2(k2), 31)+h1)*5+0x38495ab5) & mask
    h1 ^= mix1(int.from_bytes(data[end:end+8], 'little'))
    h2 ^= mix2(int.from_bytes(data[end+8:], 'little'))
    h1 ^= len(data)
    h2 ^= len(data)
    h1 = (h1+h2) & mask
    h2 = (h2+h1) & mask
    h1, h2 = fmix(h1), fmix(h2)
    h1 = (h1+h2) & mask
    h2 = (h2+h1) & mask
    return struct.pack('<QQ', h1, h2)


def output_path(path):
    path = Path(path).resolve()
    if not path.is_relative_to(ROOT) or path.exists():
        raise ValueError('Output must be a NEW file inside the addon workspace')
    return path


def decode_link(data, at):
    first = data[at]
    external = bool(first & 64)
    value = first & 63
    at += 1
    while first & 128:
        first = data[at]
        value = (value << 7) | (first & 127)
        at += 1
    if not external:
        return (None, value), at
    group = value
    first = data[at]
    value = first & 127
    at += 1
    while first & 128:
        first = data[at]
        value = (value << 7) | (first & 127)
        at += 1
    return (group, value), at


def number(value, reserve_external=True):
    if not isinstance(value, int) or value < 0 or value >= 2**32:
        raise ValueError('Invalid resource number')
    digits = [value & 127]
    while value > 127:
        value >>= 7
        digits.insert(0, value & 127)
    if reserve_external and digits[0] & 64:
        digits.insert(0, 0)
    return bytes(v | (128 if i < len(digits)-1 else 0) for i, v in enumerate(digits))


def encode_link(link):
    group, index = link
    if group is None:
        return number(index)
    prefix = bytearray(number(group))
    prefix[0] |= 64
    return bytes(prefix) + number(index)


@dataclass
class Resource:
    id: int
    source: int
    type: str
    raw: bytearray
    links: list
    locators: list
    label: str

    def replace(self, old, new, count=1):
        if self.raw.count(old) != count:
            raise ValueError(f'{self.label}: replacement is not unique ({self.raw.count(old)})')
        self.raw = self.raw.replace(old, new)

    def remap(self, replacements):
        self.links = [replacements.get(link, link) for link in self.links]


class Group:
    def __init__(self, graph, links, group_id, audit):
        self.graph, self.all_links = graph, links
        self.info = next(g for g in graph.groups if g['id'] == group_id)
        self.data = (Path(audit) / f'{group_id}.bin').read_bytes()
        self.rows = []
        for line in (Path(audit) / f'{group_id}.tsv').read_text().splitlines():
            index, typ, *values = line.split('\t')
            self.rows.append((int(index), typ, *map(int, values)))
        g = self.info
        assert len(self.rows) == g['typeCount'] == g['numObjects']
        assert len(self.data) == g['groupSize']
        cursor = 0
        for i, row in enumerate(self.rows):
            assert row[0] == i and row[2] == cursor
            cursor += row[3]
        assert cursor == len(self.data)
        self.links = []
        at = g['linkStart']
        while at < g['linkStart'] + g['linkSize']:
            link, at = decode_link(links, at)
            self.links.append(link)
        assert at == g['linkStart'] + g['linkSize']
        assert len(self.links) == self.rows[-1][5]
        assert b''.join(map(encode_link, self.links)) == links[g['linkStart']:at]
        p = graph.parts
        self.locators = list(struct.unpack_from('<'+'Q'*g['locatorCount'],
                                               p['locators'], 4+8*g['locatorStart']))
        assert len(self.locators) == self.rows[-1][7]
        self.spans = [struct.unpack_from('<Iii', p['spans'], 4+i*12)
                      for i in range(g['spanStart'], g['spanStart']+g['spanCount'])]
        assert len(self.spans) == 1, 'Only verified contiguous native groups supported'
        assert self.spans[0][1] == len(self.data)
        self.added, self.edits = [], {}

    def original(self, index):
        _, typ, at, size, ls, le, ds, de = self.rows[index]
        return Resource(index, index, typ, bytearray(self.data[at:at+size]),
                        list(self.links[ls:le]), list(self.locators[ds:de]), f'native/{index}')

    def clone(self, index, label):
        r = self.original(index)
        r.id = len(self.rows) + len(self.added)
        r.label = label
        # New identities are deterministic and never replace an original UUID.
        r.raw[:16] = uuid.uuid5(uuid.NAMESPACE_URL,
                               f'ds2-chiral-cuff-v3/{self.info["id"]}/{label}').bytes_le
        assert bytes(r.raw[:16]) not in self.data
        self.added.append(r)
        return r

    def edit_private_copy(self, index):
        if index in self.edits:
            raise ValueError('Resource already edited')
        r = self.original(index)
        self.edits[index] = r
        return r

    def emit(self, payload_offset, links_offset, payload_file_index=None):
        """Return one group's appended bytes and tables, leaving source intact."""
        # This is an intermediate archive, not a runtime-ready cache: appended
        # objects and resized Arrays also require rebuilding graph ArrayTable
        # reservations from an object census. generate_cache_config enforces
        # that audit before an ASI can point the game at the resulting cache.
        file_index, _, source_offset = self.spans[0]
        destination_file = file_index if payload_file_index is None else payload_file_index
        spans, payload, cursor = [], bytearray(), 0
        for index, r in sorted(self.edits.items()):
            at, size = self.rows[index][2:4]
            if at > cursor:
                spans.append((file_index, at-cursor, source_offset+cursor))
            spans.append((destination_file, len(r.raw), payload_offset+len(payload)))
            payload.extend(r.raw)
            cursor = at+size
        if cursor < len(self.data):
            spans.append((file_index, len(self.data)-cursor, source_offset+cursor))
        extra = b''.join(r.raw for r in self.added)
        if extra:
            spans.append((destination_file, len(extra), payload_offset+len(payload)))
            payload.extend(extra)
        resources = [self.edits.get(i) or self.original(i) for i in range(len(self.rows))] + self.added
        links = b''.join(encode_link(link) for r in resources for link in r.links)
        locators = [locator for r in resources for locator in r.locators]
        p, g = self.graph.parts, self.info
        old_type = p['typeTable'][4:]
        type_count, capacity = struct.unpack_from('<II', old_type, 8)
        assert type_count == capacity
        old_group_type = old_type[20+2*g['typeStart']:20+2*(g['typeStart']+g['typeCount'])]
        types = old_group_type + b''.join(old_group_type[2*r.source:2*r.source+2] for r in self.added)
        extended = bytearray(old_type+types)
        struct.pack_into('<II', extended, 8, type_count+len(types)//2, type_count+len(types)//2)
        p['typeTable'] = struct.pack('<I', len(extended))+extended
        loc_count = struct.unpack_from('<I', p['locators'])[0]
        p['locators'] = (struct.pack('<I', loc_count+len(locators))+p['locators'][4:]+
                         struct.pack('<'+'Q'*len(locators), *locators))
        span_count = struct.unpack_from('<I', p['spans'])[0]
        p['spans'] = (struct.pack('<I', span_count+len(spans))+p['spans'][4:]+
                      b''.join(struct.pack('<Iii', *s) for s in spans))
        groups = bytearray(p['groups'])
        struct.pack_into('<iq', groups, g['relative']+4, len(resources), sum(s[1] for s in spans))
        struct.pack_into('<8I', groups, g['relative']+32, span_count, len(spans), type_count,
                         len(resources), links_offset, len(links), loc_count, len(locators))
        p['groups'] = groups
        return bytes(payload), links


def archive_segments(source, payloads, folder, label):
    """Small patch description for the existing validated private cache builder.

    Every original compressed payload is copied verbatim to the private result.
    Only the archive header/table and new compressed chunks are distributed.
    """
    import lz4.block
    source, folder = Path(source), Path(folder)
    a = DSAR(source)
    payloads = list(map(bytes, payloads))
    if not payloads or any(not 0 < len(b) <= 256*1024*1024 for b in payloads):
        raise ValueError('Invalid appended chunk size')
    # Keep native-size blocks and 16-byte physical alignment. Independent
    # archive readers accept huge blocks, but the game's streaming allocator
    # should not have to handle non-native multi-megabyte compressed blocks.
    block_size = 262144
    delta = 32*sum((len(b)+block_size-1)//block_size for b in payloads)
    with source.open('rb') as f:
        header = bytearray(f.read(32))
    rows = [(v, p+delta, n, c, k) for v, p, n, c, k in a.chunks]
    virtual, physical = a.total, source.stat().st_size+delta
    starts, compressed = [], []
    for raw in payloads:
        starts.append(virtual)
        for offset in range(0, len(raw), block_size):
            piece = raw[offset:offset+block_size]
            packed = lz4.block.compress(piece, mode='high_compression', compression=9, store_size=False)
            assert lz4.block.decompress(packed, uncompressed_size=len(piece)) == piece
            padding = (-physical) % 16
            compressed.append(bytes(padding))
            physical += padding
            rows.append((virtual, physical, len(piece), len(packed), 3))
            compressed.append(packed)
            virtual += len(piece)
            physical += len(packed)
    if virtual >= 2**31:
        raise ValueError('Native signed archive offset limit exceeded')
    struct.pack_into('<IIQ', header, 8, len(rows), a.first+delta, virtual)
    table = bytes(header)+b''.join(struct.pack('<QQIIB7x', *r) for r in rows)
    assert len(table) == a.first+delta, 'Unexpected native header padding'
    tail = b''.join(compressed)
    head_name, tail_name = label+'-table.bin', label+'-append.bin'
    output_path(folder/head_name).write_bytes(table)
    output_path(folder/tail_name).write_bytes(tail)
    def part(name, data):
        return dict(payload=name, sha256=hashlib.sha256(data).hexdigest(), offset=0, length=len(data))
    return starts, [part(head_name, table),
                    dict(original=a.first, length=source.stat().st_size-a.first),
                    part(tail_name, tail)], physical
