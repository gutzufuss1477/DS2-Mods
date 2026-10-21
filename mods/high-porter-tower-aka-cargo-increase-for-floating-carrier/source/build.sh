#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
NAME="DS2_FloatingCarrier_CargoCapacity_v1.0.0.asi"
mkdir -p "$ROOT/buildA" "$ROOT/buildB" "$ROOT/release"
build_one(){
  local OUT="$1"
  rm -f "$OUT"/*
  clang-cl --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX "$ROOT/src/kernel32_stub.cpp" /Fo"$OUT/kernel32_stub.obj"
  lld-link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib /out:"$OUT/kernel32.dll" /implib:"$OUT/kernel32.lib" "$OUT/kernel32_stub.obj"
  clang-cl --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin "$ROOT/src/floating_carrier_cargo_capacity.cpp" /Fo"$OUT/floating_carrier_cargo_capacity.obj"
  lld-link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib /timestamp:0 /dynamicbase /nxcompat /highentropyva /noimplib /out:"$OUT/$NAME" "$OUT/floating_carrier_cargo_capacity.obj" "$OUT/kernel32.lib"
}
build_one "$ROOT/buildA"
build_one "$ROOT/buildB"
cmp "$ROOT/buildA/$NAME" "$ROOT/buildB/$NAME"
cp "$ROOT/buildA/$NAME" "$ROOT/release/"
sha256sum "$ROOT/release/$NAME"
