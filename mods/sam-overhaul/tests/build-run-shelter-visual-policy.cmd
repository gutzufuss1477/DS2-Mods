@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 2
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\tests\shelter_visual_policy"
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++17 /EHsc /O2 /W4 /WX /Fe"%OUT%\shelter_visual_policy_tests.exe" "%ROOT%\tests\shelter_visual_policy_tests.cpp" kernel32.lib
if errorlevel 1 exit /b 3
"%OUT%\shelter_visual_policy_tests.exe"
if errorlevel 1 exit /b 4
exit /b 0
