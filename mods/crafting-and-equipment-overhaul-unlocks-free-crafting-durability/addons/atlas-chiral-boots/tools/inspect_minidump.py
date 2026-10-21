"""Read-only Windows x64 minidump triage. Stack pointers are candidates, not unwound frames."""
import argparse
import hashlib
import json
import struct
from pathlib import Path


class Dump:
    def __init__(self, path):
        self.path = Path(path)
        self.data = self.path.read_bytes()
        assert self.data[:4] == b"MDMP"
        count, directory = self.unpack("<II", 8)
        self.streams = {}
        for i in range(count):
            kind, size, rva = self.unpack("<III", directory + i * 12)
            self.streams[kind] = (size, rva)
        self.modules = []
        _, off = self.streams[4]
        for i in range(self.unpack("<I", off)[0]):
            base, size, checksum, stamp, name_rva = self.unpack("<QIIII", off + 4 + 108 * i)
            length = self.unpack("<I", name_rva)[0]
            name = self.data[name_rva+4:name_rva+4+length].decode("utf-16-le")
            self.modules.append(dict(base=base, size=size, timestamp=stamp, path=name))
        self.ranges = []
        if 5 in self.streams:
            _, off = self.streams[5]
            for i in range(self.unpack("<I", off)[0]):
                base, size, rva = self.unpack("<QII", off + 4 + 16*i)
                self.ranges.append((base, size, rva))
        if 9 in self.streams:
            _, off = self.streams[9]
            count, rva = self.unpack("<QQ", off)
            for i in range(count):
                base, size = self.unpack("<QQ", off + 16 + 16*i)
                self.ranges.append((base, size, rva))
                rva += size
        self.threads = []
        _, off = self.streams[3]
        for i in range(self.unpack("<I", off)[0]):
            p = off + 4 + 48*i
            tid = self.unpack("<I", p)[0]
            stack_base, stack_size, stack_rva, ctx_size, ctx_rva = self.unpack("<QIIII", p+24)
            self.ranges.append((stack_base, stack_size, stack_rva))
            self.threads.append(dict(id=tid, stack_base=stack_base, stack_size=stack_size,
                                     context_size=ctx_size, context_rva=ctx_rva))

    def unpack(self, fmt, off):
        return struct.unpack_from(fmt, self.data, off)

    def memory(self, address, size):
        for base, length, rva in self.ranges:
            if base <= address and address + size <= base + length:
                return self.data[rva+address-base:rva+address-base+size]
        return None

    def resolve(self, address):
        for m in self.modules:
            if m["base"] <= address < m["base"]+m["size"]:
                return f'{Path(m["path"]).name}+0x{address-m["base"]:x}'
        return None

    def context(self, off):
        names = "rax rcx rdx rbx rsp rbp rsi rdi r8 r9 r10 r11 r12 r13 r14 r15 rip".split()
        return dict(zip(names, self.unpack("<17Q", off+0x78)))

    def report(self):
        _, off = self.streams[6]
        tid = self.unpack("<I", off)[0]
        code, flags, record, address, count = self.unpack("<IIQQI", off+8)
        params = self.unpack("<15Q", off+40)[:count]
        ctx_size, ctx_rva = self.unpack("<II", off+160)
        regs = self.context(ctx_rva)
        stack_candidates = []
        for delta in range(0, 8192, 8):
            data = self.memory(regs["rsp"]+delta, 8)
            if data is None:
                break
            value = struct.unpack("<Q", data)[0]
            match = self.resolve(value)
            if match:
                stack_candidates.append(dict(stack_offset=hex(delta), value=hex(value), module_rva=match))
        return dict(source=str(self.path), sha256=hashlib.sha256(self.data).hexdigest(),
                    exception=dict(thread_id=tid, code=hex(code), flags=hex(flags),
                                   address=hex(address), module_rva=self.resolve(address),
                                   parameters=[hex(p) for p in params]),
                    registers={k:hex(v) for k,v in regs.items()},
                    instruction_bytes=(self.memory(regs["rip"], 32) or b"").hex(),
                    stack_candidates_not_unwound_frames=stack_candidates,
                    modules=self.modules, threads=self.threads)


if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("dump")
    p.add_argument("--output", required=True)
    a = p.parse_args()
    report = Dump(a.dump).report()
    Path(a.output).write_text(json.dumps(report, indent=2), encoding="utf-8")
    print(json.dumps({k:v for k,v in report.items() if k not in ("modules", "threads")}, indent=2))
