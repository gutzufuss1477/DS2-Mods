@echo off
setlocal
cd /d "%~dp0.."
if not exist build mkdir build
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" > build\pcc-vcvars.log 2>&1
if errorlevel 1 exit /b 1
set "CC=C:\Program Files\LLVM\bin\clang-cl.exe"
"%CC%" /nologo /c src\pcc_thunk.s /Fobuild\pcc_thunk.obj
if errorlevel 1 exit /b 1
"%CC%" /nologo /std:c++17 /EHsc /MT /O2 tests\pcc_native_tests.cpp /Febuild\pcc_native_tests.exe /Fobuild\pcc_native_tests.obj /link build\pcc_thunk.obj
if errorlevel 1 exit /b 1
build\pcc_native_tests.exe
exit /b %errorlevel%
