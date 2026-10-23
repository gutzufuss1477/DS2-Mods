"""Generate and round-trip a byte-exact graph copy/splice program.

Graph inputs stay local. The generated header contains structural changes only.
"""
from pathlib import Path
import argparse
import hashlib
import json
import sys

def generate(source, target, header, namespace, parser_dir):
    sys.path.insert(0, str(parser_dir))
    from atlas_graph import Graph
    src, dst = Graph(source), Graph(target)
    assert src.order == dst.order and src.raw[-4:] == dst.raw[-4:] == b'\0'*4
    operations, literals, result = [], bytearray(), bytearray()
    def add(kind, offset, data):
        if not data:
            return
        if operations and operations[-1][0] == kind and sum(operations[-1][1:]) == offset:
            operations[-1][2] += len(data)
        else:
            operations.append([kind, offset, len(data)])
        result.extend(data)
    def cp(offset, size):
        add(0, offset, src.raw[offset:offset+size])
    def lit(data):
        add(1, len(literals), data)
        literals.extend(data)
    lit(dst.raw[:12])
    source_offset = 12
    for section in src.order:
        old, new = src.parts[section], dst.parts[section]
        assert len(new) >= len(old), section
        cursor = index = 0
        while index < len(old):
            if new[index] == old[index]:
                index += 1
                continue
            begin = index
            while index < len(old) and new[index] != old[index]:
                index += 1
            cp(source_offset+cursor, begin-cursor)
            lit(new[begin:index])
            cursor = index
        cp(source_offset+cursor, len(old)-cursor)
        lit(new[len(old):])
        source_offset += len(old)
    assert source_offset+4 == len(src.raw)
    cp(source_offset, 4)
    assert bytes(result) == dst.raw
    def array(data):
        return '\n'.join(' '+','.join(f'0x{x:02x}' for x in data[i:i+16])+',' for i in range(0,len(data),16))
    hashes = [hashlib.sha256(g.raw).digest() for g in (src, dst)]
    entries = '\n'.join(' {'+','.join(f'{x}u' for x in row)+'},' for row in operations)
    header.write_text(f'''#pragma once
// Generated structural registration of the native Story model.
// Input and output are SHA-pinned; no original game file is modified.
namespace {namespace} {{
using u8=unsigned char;using u32=unsigned int;
struct Op{{u32 kind,offset,length;}};
constexpr u32 OriginalSize={len(src.raw)}u,TargetSize={len(dst.raw)}u;
constexpr u32 OperationCount={len(operations)}u,LiteralCount={len(literals)}u;
constexpr u8 OriginalSHA[32]={{
{array(hashes[0])}
}};
constexpr u8 TargetSHA[32]={{
{array(hashes[1])}
}};
constexpr Op Program[OperationCount]={{
{entries}
}};
constexpr u8 Literals[LiteralCount]={{
{array(literals)}
}};
}}
''')
    return dict(inputSHA256=hashes[0].hex(), targetSHA256=hashes[1].hex(),
                inputBytes=len(src.raw), targetBytes=len(dst.raw),
                operationCount=len(operations), literalBytes=len(literals), roundTripExact=True)

if __name__ == '__main__':
    p = argparse.ArgumentParser()
    for name in ('source','target','header','parser-dir','audit'):
        p.add_argument('--'+name, type=Path, required=True)
    p.add_argument('--namespace', required=True)
    a = p.parse_args()
    report = generate(a.source,a.target,a.header,a.namespace,a.parser_dir)
    a.audit.write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report))
