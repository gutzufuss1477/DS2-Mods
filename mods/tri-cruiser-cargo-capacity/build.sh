#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
mkdir -p "$ROOT/build" "$ROOT/release"
rm -f "$ROOT/build"/* "$ROOT/release"/*
clang-cl --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX "$ROOT/src/kernel32_stub.cpp" /Fo"$ROOT/build/kernel32_stub.obj"
lld-link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib /out:"$ROOT/build/kernel32.dll" /implib:"$ROOT/build/kernel32.lib" "$ROOT/build/kernel32_stub.obj"
clang --target=x86_64-pc-windows-msvc -c "$ROOT/src/capacity_stubs.s" -o "$ROOT/build/capacity_stubs.obj"
clang-cl --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin "$ROOT/src/tri_cruiser_cargo_capacity.cpp" /Fo"$ROOT/build/tri_cruiser_cargo_capacity.obj"
lld-link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib /timestamp:0 /dynamicbase /nxcompat /highentropyva /noimplib /out:"$ROOT/release/DS2_TriCruiser_CargoCapacity_v1.0.0.asi" "$ROOT/build/tri_cruiser_cargo_capacity.obj" "$ROOT/build/capacity_stubs.obj" "$ROOT/build/kernel32.lib"
