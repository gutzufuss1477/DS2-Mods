#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
SRC="$ROOT/src"
OUT="$ROOT/build"
ASI="$ROOT/dist/DS2_HighDensityBackpackModules_v1.1.0.asi"
mkdir -p "$OUT" "$ROOT/dist"
rm -f "$ASI"
CLANG_CL="${CLANG_CL:-clang-cl}"
LLD_LINK="${LLD_LINK:-lld-link}"
"$LLD_LINK" /dll /noentry /machine:x64 /def:"$SRC/kernel32.def" /out:"$OUT/kernel32_stub.dll" /implib:"$OUT/kernel32.lib"
"$CLANG_CL" --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /Gs9999999 /GR- /EHs-c- /Zl /Oi /W4 /WX \
  /clang:-ffreestanding /clang:-fno-builtin /clang:-funwind-tables /Fo"$OUT/high_density_backpack_modules.obj" "$SRC/high_density_backpack_modules.cpp"
"$CLANG_CL" --target=x86_64-pc-windows-msvc /nologo /c /Fo"$OUT/charm_adapters.obj" "$SRC/charm_adapters.s"
"$LLD_LINK" /dll /machine:x64 /entry:DllMain /nodefaultlib /timestamp:0 \
  /out:"$ASI" /implib:"$OUT/high_density_backpack_modules.lib" \
  "$OUT/high_density_backpack_modules.obj" "$OUT/charm_adapters.obj" "$OUT/kernel32.lib"
printf 'Built: %s\n' "$ASI"
