"""Allocate the actual texture descriptors in an isolated D3D12 process.

Reads the captured Test06 descriptor and final private archive. No game process,
UI, hooks, global debug settings, original-file writes, or save access.
COM declarations follow Windows SDK 10.0.26100.0/um/d3d12.h.
"""
import argparse
import ctypes as c
import hashlib
import json
import uuid
from pathlib import Path
from inspect_minidump import Dump
from native_bundle import ROOT, DSAR
from serialize_native_geometry import PACKAGE
from fix_resident_textures import private_texture_rows, validate_resident_headers


class GUID(c.Structure):
    _fields_ = [('bytes', c.c_ubyte * 16)]

    @classmethod
    def parse(cls, text):
        return cls.from_buffer_copy(uuid.UUID(text).bytes_le)


class HeapProperties(c.Structure):
    _fields_ = [(n, c.c_uint32) for n in
                ('Type', 'CPUPageProperty', 'MemoryPoolPreference',
                 'CreationNodeMask', 'VisibleNodeMask')]


class ResourceDesc(c.Structure):
    _fields_ = [('Dimension', c.c_uint32), ('Alignment', c.c_uint64),
                ('Width', c.c_uint64), ('Height', c.c_uint32),
                ('DepthOrArraySize', c.c_uint16), ('MipLevels', c.c_uint16),
                ('Format', c.c_uint32), ('SampleCount', c.c_uint32),
                ('SampleQuality', c.c_uint32), ('Layout', c.c_uint32),
                ('Flags', c.c_uint32)]


def method(obj, index, result, *args):
    table = c.cast(obj, c.POINTER(c.POINTER(c.c_void_p))).contents
    return c.WINFUNCTYPE(result, c.c_void_p, *args)(table[index])


def release(obj):
    if obj:
        method(obj, 2, c.c_uint32)(obj)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--stage', required=True)
    parser.add_argument('--output', required=True)
    args = parser.parse_args()
    assert c.sizeof(ResourceDesc) == 56
    assert ResourceDesc.Flags.offset == 48
    assert c.sizeof(HeapProperties) == 20
    stage = (ROOT / args.stage).resolve()
    assert stage.is_relative_to(ROOT / 'staging')
    report = json.loads((stage / 'report.json').read_text())
    original = DSAR(PACKAGE / 'package.42.02.core.stream')
    archive = DSAR(Path(report['cache']) / 'package.42.02.core.stream')
    payload = archive.read(original.total, archive.total-original.total)
    checked_headers = validate_resident_headers(payload)
    dump_path = ROOT / 'evidence/test06-startup-crash.dmp'
    dump = Dump(dump_path)
    # Exception R8 points to the D3D12_RESOURCE_DESC in this captured dump.
    captured = dump.memory(0xbfa03ff618, 56)
    assert captured is not None
    bad = ResourceDesc.from_buffer_copy(captured)
    assert (bad.Dimension, bad.Width, bad.Height, bad.MipLevels,
            bad.Format, bad.Flags) == (3, 1024, 1024, 11, 98, 4)
    iid_device = GUID.parse('189819f1-1db6-4b57-be54-1821339b85f7')
    iid_resource = GUID.parse('696442be-a72e-4059-bc79-5b5c98040fad')
    d3d = c.WinDLL('d3d12.dll')
    create = d3d.D3D12CreateDevice
    create.argtypes = [c.c_void_p, c.c_uint32, c.POINTER(GUID), c.POINTER(c.c_void_p)]
    create.restype = c.c_int32
    device = c.c_void_p()
    hr = create(None, 0xb000, c.byref(iid_device), c.byref(device))
    assert hr >= 0 and device, f'D3D12CreateDevice failed: {hr & 0xffffffff:08x}'
    heap = HeapProperties(1, 0, 0, 1, 1)
    results = []
    try:
        allocate = method(device, 27, c.c_int32, c.POINTER(HeapProperties),
                          c.c_uint32, c.POINTER(ResourceDesc), c.c_uint32,
                          c.c_void_p, c.POINTER(GUID), c.POINTER(c.c_void_p))

        def probe(name, desc, should_succeed):
            resource = c.c_void_p()
            hr = allocate(device, c.byref(heap), 0, c.byref(desc), 0, None,
                          c.byref(iid_resource), c.byref(resource))
            result = dict(name=name, descriptor={k: int(getattr(desc,k))
                          for k,_ in ResourceDesc._fields_},
                          hresult=f'0x{hr & 0xffffffff:08x}',
                          resourceCreated=bool(resource))
            results.append(result)
            try:
                assert ((hr >= 0 and bool(resource)) if should_succeed
                        else (hr < 0 and not resource)), result
            finally:
                release(resource)

        probe('Test06 captured BC7 descriptor: reject UAV', bad, False)
        corrected = ResourceDesc.from_buffer_copy(captured)
        corrected.Flags = 0
        probe('Same captured descriptor, only Flags=0', corrected, True)
        for row, at, _ in private_texture_rows(payload):
            flags = payload[at+75]
            assert flags & 8 and payload[at+76] == 0
            dxgi = 98 if payload[at+71] == 75 else (72 if payload[at+73] else 71)
            # Native RVA 0x2112c70: bit 8 suppresses UAV. These private
            # textures have no render-target bit; all mips are resident.
            desc = ResourceDesc(3, 0, row['width'], row['height'], 1,
                                row['mips'], dxgi, 1, 0, 0, 0)
            probe(row['name'], desc, True)
    finally:
        release(device)
    evidence = dict(api='System D3D12CreateDevice / ID3D12Device::CreateCommittedResource',
                    limitations='Resource allocation only; no game reader, shader, upload or rendered frame tested.',
                    test06DumpSha256=hashlib.sha256(dump_path.read_bytes()).hexdigest(),
                    capturedDescriptorAddress='0xbfa03ff618', cache=report['cache'],
                    tests=results, residentHeaders=checked_headers,
                    gameLaunched=False, gameFilesWritten=False, saveFilesAccessed=False)
    with (ROOT / args.output).open('x', encoding='utf-8') as out:
        json.dump(evidence, out, indent=2)
    print(json.dumps(results, indent=2))


if __name__ == '__main__':
    main()
