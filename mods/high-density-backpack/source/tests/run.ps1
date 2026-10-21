param([string]$Llvm = 'C:\Program Files\LLVM\bin')
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $root 'build'
New-Item -ItemType Directory -Force -Path $out | Out-Null
& "$Llvm\lld-link.exe" /dll /noentry /machine:x64 "/def:$root\src\kernel32.def" "/out:$out\kernel32_stub.dll" "/implib:$out\kernel32.lib"
if ($LASTEXITCODE) { throw 'Import library failed' }
& "$Llvm\clang-cl.exe" /nologo /c /O2 /GS- /Gs9999999 /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-ffreestanding /clang:-fno-builtin /clang:-funwind-tables "/Fo$out\runtime_tests.obj" "$PSScriptRoot\runtime_tests.cpp"
if ($LASTEXITCODE) { throw 'Test compilation failed' }
foreach ($item in @(@('charm_adapters', "$root\src\charm_adapters.s"), @('adapter_test', "$PSScriptRoot\adapter_test.s"))) {
    & "$Llvm\clang-cl.exe" /nologo /c "/Fo$out\$($item[0]).obj" $item[1]
    if ($LASTEXITCODE) { throw 'Test assembly failed' }
}
& "$Llvm\lld-link.exe" /dll /noentry /nodefaultlib /machine:x64 "/out:$out\runtime_tests.dll" "$out\runtime_tests.obj" "$out\charm_adapters.obj" "$out\adapter_test.obj" "$out\kernel32.lib"
if ($LASTEXITCODE) { throw 'Test linking failed' }
python -c "import ctypes,sys; d=ctypes.CDLL(sys.argv[1]); n=d.run_tests(); print('PASS: placement, visibility, ownership, eight equipment slots, visual packing and bone references, equipment-preview lifecycle, battery deduplication, adapter registers, 771 native Cryptobiosis loop cases' if n==0 else 'FAIL: runtime_tests.cpp line '+str(n)); sys.exit(bool(n))" "$out\runtime_tests.dll"
if ($LASTEXITCODE) { throw 'Runtime tests failed' }

python -c "import ctypes,sys; d=ctypes.CDLL(sys.argv[1]); d.test_state_file.argtypes=[ctypes.c_wchar_p]; n=d.test_state_file(sys.argv[2]); print('PASS: accessory state file roundtrip and invalid data' if n==0 else 'FAIL: runtime_tests.cpp line '+str(n)); sys.exit(bool(n))" "$out\runtime_tests.dll" "$out\test-charms.ini"
if ($LASTEXITCODE) { throw 'State file tests failed' }
