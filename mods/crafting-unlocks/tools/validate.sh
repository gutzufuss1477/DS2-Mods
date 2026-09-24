#!/usr/bin/env bash
# Runs local host tests and builds a Windows-x64 ASI. Does not launch or modify DS2.
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build docs validation release
CXX="${CXX:-clang++}"
CLANG_CL="${CLANG_CL:-clang-cl}"
LLD_LINK="${LLD_LINK:-lld-link}"
OBJDUMP="${OBJDUMP:-objdump}"
{
  "$CXX" --version
  "$CLANG_CL" --version
  "$LLD_LINK" --version
  printf '%s\n' 'Runtime compile flags: --target=x86_64-pc-windows-msvc /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl'
  printf '%s\n' 'Link flags: /dll /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /opt:ref /opt:icf'
} > validation/TOOLCHAIN.txt
python3 tools/generate_english_config.py > docs/CONFIG_GENERATION.json
"$CXX" -std=c++17 -O2 tests/core_tests.cpp -o build/core_tests
"$CXX" -std=c++17 -O2 tests/config_tests.cpp -o build/config_r1_tests
"$CXX" -std=c++17 -O2 tests/backpack_tests.cpp -o build/backpack_tests
./build/core_tests | tee docs/CORE_TESTS.txt
./build/config_r1_tests tests/fixtures/config_r1.ini | tee docs/CONFIG_R1_TESTS.txt
./build/backpack_tests release/ds2_crafting_unlocks.ini | tee docs/BACKPACK_TESTS.txt
"$CXX" -std=c++17 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined tests/core_tests.cpp -o build/core_tests_asan
"$CXX" -std=c++17 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined tests/config_tests.cpp -o build/config_tests_asan
"$CXX" -std=c++17 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined tests/backpack_tests.cpp -o build/backpack_tests_asan
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=1"
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1"
./build/core_tests_asan | tee docs/CORE_SANITIZERS.txt
./build/config_tests_asan tests/fixtures/config_r1.ini | tee docs/CONFIG_SANITIZERS.txt
./build/backpack_tests_asan release/ds2_crafting_unlocks.ini | tee docs/BACKPACK_SANITIZERS.txt
"$LLD_LINK" /lib /machine:x64 /def:src/kernel32.def /out:build/kernel32.lib
"$CLANG_CL" --target=x86_64-pc-windows-msvc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /c src/mod.cpp /Fobuild/mod.obj
"$LLD_LINK" /dll /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /opt:ref /opt:icf /out:release/ds2_crafting_unlocks.asi /implib:build/ds2_crafting_unlocks.lib build/mod.obj build/kernel32.lib
"$OBJDUMP" -p release/ds2_crafting_unlocks.asi > docs/PE_HEADER_IMPORTS_UNWIND.txt
"$OBJDUMP" -d release/ds2_crafting_unlocks.asi > docs/COMPILED_DISASSEMBLY.txt
python3 tests/verify_build.py | tee docs/STRUCTURAL_CHECK.txt
