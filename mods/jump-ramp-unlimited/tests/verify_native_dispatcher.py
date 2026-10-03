"""Execute the production cave and installed DS2 dispatcher in an isolated process.

Does not open, launch, or modify the game process or executable. Engine helper
functions are replaced by explicit test doubles. This verifies machine code and
dispatch behavior, NOT animation acceptance or actual gameplay success.
"""
import ctypes as C
import hashlib
import itertools
import json
import os
from pathlib import Path
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[1]
GAME_EXE = Path(os.environ.get("DS2_EXE", r"C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe"))
LLVM = Path(os.environ.get("LLVM_BIN", r"C:\Program Files\LLVM\bin"))
OUT = ROOT / "build" / "tests"
OUT.mkdir(parents=True, exist_ok=True)


def build():
    lib = OUT / "kernel32.lib"
    obj = OUT / "native_harness.obj"
    dll = OUT / "native_harness.dll"
    subprocess.run([str(LLVM / "lld-link.exe"), "/lib", "/machine:x64",
                    f"/def:{ROOT / 'src/kernel32.def'}", f"/out:{lib}"], check=True)
    subprocess.run([str(LLVM / "clang-cl.exe"), "--target=x86_64-pc-windows-msvc",
                    "/nologo", "/c", "/O2", "/Ob0", "/GS-", "/GR-", "/EHs-c-",
                    "/Zl", "/Oi", "/W4", "/WX", "/clang:-fno-builtin",
                    f"/I{ROOT / 'src'}", "/TP", f"/Fo{obj}",
                    str(ROOT / "tests/native_harness.cpp")], check=True)
    subprocess.run([str(LLVM / "lld-link.exe"), "/dll", "/entry:DllMain",
                    "/nodefaultlib", "/machine:x64", "/subsystem:windows", "/Brepro",
                    f"/out:{dll}", str(obj), str(lib)], check=True)
    return dll


data = GAME_EXE.read_bytes()
pe = struct.unpack_from("<I", data, 0x3C)[0]
assert data[:2] == b"MZ" and data[pe:pe + 4] == b"PE\0\0"
assert struct.unpack_from("<H", data, pe + 4)[0] == 0x8664
assert struct.unpack_from("<I", data, pe + 8)[0] == 0x6A3DAE46
assert struct.unpack_from("<I", data, pe + 24 + 56)[0] == 0xB292000
section_start = pe + 24 + struct.unpack_from("<H", data, pe + 20)[0]
sections = [struct.unpack_from("<IIII", data, section_start + 40 * i + 8)
            for i in range(struct.unpack_from("<H", data, pe + 6)[0])]


def read_rva(rva, size):
    for virtual_size, virtual_address, raw_size, raw_pointer in sections:
        if virtual_address <= rva and rva + size <= virtual_address + raw_size:
            offset = raw_pointer + rva - virtual_address
            return data[offset:offset + size]
    raise ValueError(hex(rva))


assert read_rva(0x106153C, 5) == bytes.fromhex("83 f9 01 75 2c")
assert read_rva(0x1061541, 10) == bytes.fromhex("b2 03 48 8b cb e8 f5 04 00 00")
assert read_rva(0x1061561, 4) == bytes.fromhex("84 c0 74 08")
assert read_rva(0x106156D, 12) == bytes.fromhex("c5 f8 28 74 24 30 48 83 c4 40 5b c3")

k32 = C.WinDLL("kernel32", use_last_error=True)
k32.VirtualAlloc.argtypes = (C.c_void_p, C.c_size_t, C.c_ulong, C.c_ulong)
k32.VirtualAlloc.restype = C.c_void_p
k32.FlushInstructionCache.argtypes = (C.c_void_p, C.c_void_p, C.c_size_t)
k32.FlushInstructionCache.restype = C.c_int
k32.GetCurrentProcess.restype = C.c_void_p
image = k32.VirtualAlloc(None, 0x1063000, 0x3000, 0x40)
if not image:
    raise C.WinError(C.get_last_error())
C.memmove(image + 0x1061470, read_rva(0x1061470, 265), 265)
C.memmove(image, data[:0x1000], 0x1000)
C.memmove(image + 0x1061A81, read_rva(0x1061A81, 13), 13)

self_buffer = C.create_string_buffer(0x2000)
owner_buffer = C.create_string_buffer(0x7400)
self_addr = C.addressof(self_buffer)
state = {}
calls = []
callback_errors = []


def byte_at(offset):
    return C.c_ubyte.from_address(self_addr + offset)


GATE = C.CFUNCTYPE(C.c_ubyte, C.c_void_p)
HELPER = C.CFUNCTYPE(C.c_ubyte, C.c_void_p, C.c_ubyte)
SUCCESS = C.CFUNCTYPE(None, C.c_void_p)


@GATE
def active_gate(ptr):
    if ptr != self_addr:
        callback_errors.append(("gate", ptr))
        return 0
    return state["active"]


def run_helper(ptr, requested):
    if ptr != self_addr:
        callback_errors.append(("helper", ptr))
        return 0
    calls.append(("helper", requested, byte_at(0x1DFB).value))
    cooldown = C.c_float.from_address(self_addr + 0x1DF4).value
    if cooldown > 0 or requested > state["cap"] or not state["input"]:
        return 0
    byte_at(0x1DFB).value = requested
    return 1


@GATE
def first_helper(ptr):
    return run_helper(ptr, 1)


@HELPER
def next_helper(ptr, requested):
    return run_helper(ptr, requested)


@SUCCESS
def success(ptr):
    if ptr != self_addr:
        callback_errors.append(("success", ptr))
        return
    calls.append(("success", byte_at(0x1DFB).value))
    C.c_float.from_address(self_addr + 0x1DF4).value = 0.2


for rva, callback in [(0xFEF430, active_gate), (0x10617A0, first_helper),
                      (0x1061A40, next_helper), (0x1062080, success)]:
    jump = bytes.fromhex("ff 25 00 00 00 00") + struct.pack("<Q", C.cast(callback, C.c_void_p).value)
    C.memmove(image + rva, jump, len(jump))
assert k32.FlushInstructionCache(k32.GetCurrentProcess(), image, 0x1063000)

harness = C.WinDLL(str(build()))
harness.HarnessPrepare.argtypes = (C.c_void_p,)
harness.HarnessPrepare.restype = C.c_int
harness.HarnessToggle.argtypes = (C.c_int,)
harness.HarnessToggle.restype = C.c_int
harness.HarnessCave.restype = C.c_void_p
harness.HarnessCaveSize.restype = C.c_uint
harness.HarnessCaveProtection.restype = C.c_uint
harness.HarnessParseEnabled.argtypes = (C.c_wchar_p,)
harness.HarnessParseEnabled.restype = C.c_int
assert harness.HarnessPrepare(image) == 1
assert harness.HarnessCaveProtection() == 0x20  # executable, not writable
# Fail closed for changes to every checked byte, not just the five-byte hook.
signature_bytes = 0
for rva, size in [(0x106152C, 16), (0x106153C, 5), (0x1061541, 10),
                  (0x1061561, 12), (0x106156D, 12), (0x1061A81, 13)]:
    for offset in range(size):
        cell = C.c_ubyte.from_address(image + rva + offset)
        original = cell.value
        cell.value ^= 0xFF
        assert harness.HarnessValidateSites() == 0
        assert harness.HarnessToggle(1) == 0
        assert cell.value == original ^ 0xFF
        cell.value = original
        signature_bytes += 1
assert harness.HarnessValidateSites() == 1
build_fields = [0, pe, pe + 4, pe + 24, pe + 8, pe + 24 + 56]
for offset in build_fields:
    cell = C.c_ubyte.from_address(image + offset)
    original = cell.value
    cell.value ^= 0xFF
    assert harness.HarnessValidateBuild() == 0
    cell.value = original
assert harness.HarnessValidateBuild() == 1
ini_cases = [("0", 0), ("1", 1), (" 1\t", 1), ("00", 0), ("01", 1),
             ("", -1), (" ", -1), ("2", -1), ("-1", -1), ("true", -1),
             ("1junk", -1), ("1 0", -1), ("9" * 100, -1), (None, -1)]
for text, expected in ini_cases:
    assert harness.HarnessParseEnabled(text) == expected, text
dispatch = C.CFUNCTYPE(None, C.c_void_p, C.c_float)(image + 0x1061470)
cases = 0

for enabled, active, pressed, cap, cooldown, stage in itertools.product(
        (0, 1), (False, True), (False, True), (0, 1, 2, 3, 4, 255), (0.0, 0.5), range(256)):
    assert harness.HarnessToggle(enabled) == 1
    C.memset(self_addr, 0, len(self_buffer))
    C.c_void_p.from_address(self_addr + 0x28).value = C.addressof(owner_buffer)
    byte_at(0x333).value = 1
    byte_at(0x1DFB).value = stage
    byte_at(0x1DFC).value = cap
    C.c_float.from_address(self_addr + 0x1DF4).value = cooldown
    state.update(active=active, input=pressed, cap=cap)
    calls.clear()
    dispatch(self_addr, 0.0)
    assert not callback_errors, callback_errors
    repeating = bool(enabled and stage == cap and stage in (2, 3))
    requested = cap if repeating else stage + 1 if stage < 3 else None
    attempts = active and requested is not None
    accepted = attempts and pressed and requested <= cap and cooldown == 0
    expected_stage = requested if accepted else stage
    expected_calls = []
    if attempts:
        helper_stage = stage - 1 if repeating else stage
        expected_calls.append(("helper", requested, helper_stage))
    if accepted:
        expected_calls.append(("success", requested))
    context = (enabled, active, pressed, cap, cooldown, stage)
    assert byte_at(0x1DFB).value == expected_stage, context
    assert calls == expected_calls, (context, calls, expected_calls)
    cases += 1

# Repeated requests at either real ramp cap must not increment/wrap the byte.
assert harness.HarnessToggle(1) == 1
for cap in (2, 3):
    state.update(active=True, input=True, cap=cap)
    byte_at(0x1DFB).value = cap
    byte_at(0x1DFC).value = cap
    for i in range(1000):
        C.c_float.from_address(self_addr + 0x1DF4).value = 0
        calls.clear()
        dispatch(self_addr, 0.0)
        assert not callback_errors, callback_errors
        assert byte_at(0x1DFB).value == cap
        assert calls == [("helper", cap, cap-1), ("success", cap)]
assert harness.HarnessToggle(0) == 1
assert C.string_at(image + 0x106153C, 5) == read_rva(0x106153C, 5)

# A conflicting patch must be refused, never overwritten.
changed = bytes.fromhex("90 90 90 90 90")
C.memmove(image + 0x106153C, changed, 5)
assert harness.HarnessToggle(1) == 0
assert C.string_at(image + 0x106153C, 5) == changed

report = {
    "result": "PASS", "dispatcher_cases": cases, "repeat_cycles": 2000,
    "game_exe_sha256": hashlib.sha256(data).hexdigest(),
    "version": (ROOT / "VERSION.txt").read_text().strip(),
    "built_asi_sha256": hashlib.sha256((ROOT / "build/ds2_jump_ramp_unlimited.asi").read_bytes()).hexdigest(),
    "source_sha256": hashlib.sha256((ROOT / "src/jump_ramp_unlimited.cpp").read_bytes()).hexdigest(),
    "signature_mutations_refused": signature_bytes,
    "build_identity_mutations_refused": len(build_fields),
    "ini_parser_cases": len(ini_cases),
    "cave_size_bytes": harness.HarnessCaveSize(),
    "cave_page_protection": "PAGE_EXECUTE_READ",
    "limits": "Engine helpers are test doubles. No gameplay/animation success is claimed.",
}
(OUT / "verification.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps(report, indent=2))
