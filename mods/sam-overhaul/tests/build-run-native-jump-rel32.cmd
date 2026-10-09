@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\tests\native_jump_rel32"
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++17 /EHsc /O2 /W4 /WX /Fe"%OUT%\native_jump_rel32_tests.exe" "%ROOT%\tests\native_jump_rel32_tests.cpp"
if errorlevel 1 exit /b 2
"%OUT%\native_jump_rel32_tests.exe"
if errorlevel 1 exit /b 3
exit /b 0
