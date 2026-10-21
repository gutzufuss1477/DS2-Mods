"""Exercise vehicle rules and real ASI loading without launching the game."""
import ctypes as C
from pathlib import Path
import tempfile

root = Path(__file__).resolve().parents[1]
fixture = C.WinDLL(str(root / "build/vehicle_tests.dll"))
fixture.TestVehiclePhysics.argtypes = []
fixture.TestVehiclePhysics.restype = C.c_int
result = fixture.TestVehiclePhysics()
assert result == 0, f"Vehicle physics fixture failed: {result}"

# Use normal Windows loader lifetime so static CRT initialization/teardown runs.
k32 = C.WinDLL("kernel32", use_last_error=True)
k32.LoadLibraryW.argtypes = [C.c_wchar_p]
k32.LoadLibraryW.restype = C.c_void_p
k32.FreeLibrary.argtypes = [C.c_void_p]
k32.FreeLibrary.restype = C.c_int
with tempfile.TemporaryDirectory(prefix="ds2-vehicle-loader-") as folder:
    binary = Path(folder) / "DS2VehicleCore.asi"
    binary.write_bytes((root / "build/DS2VehicleCore.asi").read_bytes())
    for _ in range(20):
        handle = k32.LoadLibraryW(str(binary))
        assert handle, C.WinError(C.get_last_error())
        assert k32.FreeLibrary(handle)
    assert not list(Path(folder).glob("*.log")), "Unsupported host must not start the mod"
print("PASS: speed/hill rules, CFG callback, 20 real DLL load/unload cycles, unsupported-host refusal")
