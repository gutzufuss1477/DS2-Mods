@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 2
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\tests\repair_gate_asm"
if not exist "%OUT%" mkdir "%OUT%"
ml64 /nologo /c /Fo"%OUT%\construction_repair_gate_asm.obj" "%ROOT%\src\construction_repair_gate.asm"
if errorlevel 1 exit /b 3
cl /nologo /std:c++17 /EHsc /O2 /W4 /WX /Fo"%OUT%\repair_gate_asm_tests.obj" /c "%ROOT%\tests\repair_gate_asm_tests.cpp"
if errorlevel 1 exit /b 4
link /nologo /machine:x64 /out:"%OUT%\repair_gate_asm_tests.exe" "%OUT%\construction_repair_gate_asm.obj" "%OUT%\repair_gate_asm_tests.obj"
if errorlevel 1 exit /b 5
"%OUT%\repair_gate_asm_tests.exe"
if errorlevel 1 exit /b 6
exit /b 0
