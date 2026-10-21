@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 2
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\tests\native_repair_gate_integration"
if not exist "%OUT%" mkdir "%OUT%"
ml64 /nologo /c /Fo"%OUT%\construction_repair_gate_asm.obj" "%ROOT%\src\construction_repair_gate.asm"
if errorlevel 1 exit /b 3
ml64 /nologo /c /Fo"%OUT%\native_repair_gate_integration_wrapper.obj" "%ROOT%\tests\native_repair_gate_integration_wrapper.asm"
if errorlevel 1 exit /b 4
cl /nologo /c /std:c++17 /O2 /W4 /WX /EHsc /Fo"%OUT%\construction_repair_gate.obj" "%ROOT%\src\construction_repair_gate.cpp"
if errorlevel 1 exit /b 5
cl /nologo /c /std:c++17 /O2 /W4 /WX /EHsc /Fo"%OUT%\native_repair_gate_integration_tests.obj" "%ROOT%\tests\native_repair_gate_integration_tests.cpp"
if errorlevel 1 exit /b 6
link /nologo /machine:x64 /out:"%OUT%\native_repair_gate_integration_tests.exe" "%OUT%\construction_repair_gate.obj" "%OUT%\construction_repair_gate_asm.obj" "%OUT%\native_repair_gate_integration_wrapper.obj" "%OUT%\native_repair_gate_integration_tests.obj" kernel32.lib
if errorlevel 1 exit /b 7
"%OUT%\native_repair_gate_integration_tests.exe"
if errorlevel 1 exit /b 8
exit /b 0
