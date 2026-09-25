@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" > docs\DEVENV_SETUP.log 2>&1
if errorlevel 1 exit /b 1
set "CC=C:\Program Files\LLVM\bin\clang-cl.exe"
python scripts\generate_cost_fixture.py
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\core_tests.cpp /Febuild\core_tests.exe /Fobuild\core_tests.obj
if errorlevel 1 exit /b 1
build\core_tests.exe
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\config_tests.cpp /Febuild\config_tests.exe /Fobuild\config_tests.obj
if errorlevel 1 exit /b 1
build\config_tests.exe tests\fixtures\config_r1.ini
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\backpack_tests.cpp /Febuild\backpack_tests.exe /Fobuild\backpack_tests.obj
if errorlevel 1 exit /b 1
build\backpack_tests.exe release\ds2_crafting_unlocks.ini
if errorlevel 1 exit /b 1
"%CC%" /nologo /c tests\cost_fixture_generated.s /Fobuild\cost_fixture.obj
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\freecrafting_native_tests.cpp /Febuild\freecrafting_native_tests.exe /Fobuild\freecrafting_native_tests.obj /link build\cost_fixture.obj build\freecrafting_thunks.obj
if errorlevel 1 exit /b 1
build\freecrafting_native_tests.exe release\ds2_crafting_unlocks.ini
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\freecrafting_transaction_tests.cpp /Febuild\freecrafting_transaction_tests.exe /Fobuild\freecrafting_transaction_tests.obj
if errorlevel 1 exit /b 1
build\freecrafting_transaction_tests.exe
if errorlevel 1 exit /b 1
"%CC%" /nologo /c src\special_boot_thunk.s /Fobuild\special_boot_thunk_test.obj
if errorlevel 1 exit /b 1
"%CC%" /nologo /c tests\special_boot_filter_fixture.s /Fobuild\special_boot_filter_fixture.obj
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\special_boot_filter_tests.cpp /Febuild\special_boot_filter_tests.exe /Fobuild\special_boot_filter_tests.obj /link build\special_boot_thunk_test.obj build\special_boot_filter_fixture.obj
if errorlevel 1 exit /b 1
build\special_boot_filter_tests.exe
if errorlevel 1 exit /b 1
"%CC%" /nologo /c src\durability_thunks.s /Fobuild\durability_thunks_test.obj
if errorlevel 1 exit /b 1
"%CC%" /nologo /c tests\durability_fixture.s /Fobuild\durability_fixture.obj
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\durability_tests.cpp /Febuild\durability_tests.exe /Fobuild\durability_tests.obj /link build\durability_thunks_test.obj build\durability_fixture.obj
if errorlevel 1 exit /b 1
build\durability_tests.exe
if errorlevel 1 exit /b 1
echo ALL_WINDOWS_TESTS_PASSED
