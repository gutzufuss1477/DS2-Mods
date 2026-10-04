@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\tests"
if not exist "%OUT%" mkdir "%OUT%"
ml64 /nologo /c /Fo"%OUT%\autodrive_timer_cpu_asm.obj" "%ROOT%\tests\autodrive_timer_cpu.asm"
if errorlevel 1 exit /b 2
cl /nologo /O2 /EHsc /std:c++17 /W4 /WX /fp:strict /Fo"%OUT%\autodrive_timer_cpu_cpp.obj" /Fe"%OUT%\autodrive_timer_cpu.exe" "%ROOT%\tests\autodrive_timer_cpu.cpp" "%OUT%\autodrive_timer_cpu_asm.obj"
if errorlevel 1 exit /b 3
"%OUT%\autodrive_timer_cpu.exe"
exit /b %errorlevel%
