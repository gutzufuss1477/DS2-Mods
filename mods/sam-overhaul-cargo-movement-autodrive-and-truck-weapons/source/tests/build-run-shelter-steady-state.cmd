@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 2
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\tests\shelter_steady_state"
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++17 /EHsc /O2 /W4 /WX /Fe"%OUT%\shelter_steady_state_tests.exe" "%ROOT%\tests\shelter_steady_state_tests.cpp" kernel32.lib
if errorlevel 1 exit /b 3
"%OUT%\shelter_steady_state_tests.exe"
if errorlevel 1 exit /b 4
exit /b 0
