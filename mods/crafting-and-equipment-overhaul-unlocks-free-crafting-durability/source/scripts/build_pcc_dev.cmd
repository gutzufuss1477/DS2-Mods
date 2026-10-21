@echo off
setlocal
cd /d "%~dp0.."
if not exist build mkdir build
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" > build\pcc-vcvars.log 2>&1
if errorlevel 1 exit /b 1
powershell -NoProfile -ExecutionPolicy Bypass -File build.ps1 -OutputDirectory build/pcc-dev1
if errorlevel 1 exit /b 1
call scripts\test_pcc.cmd
if errorlevel 1 exit /b 1
call scripts\build_tests.cmd
if errorlevel 1 exit /b 1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/test_atlas.ps1
if errorlevel 1 exit /b 1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/test_weapons.ps1
exit /b %errorlevel%
