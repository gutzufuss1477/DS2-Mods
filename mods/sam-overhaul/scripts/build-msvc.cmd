@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\public"
set "MH=%ROOT%\vendor\minhook"
set /p VERSION=<"%ROOT%\VERSION.txt"
if not defined VERSION exit /b 1
if not exist "%OUT%\minhook" mkdir "%OUT%\minhook"

rem Preserve the original, lean no-CRT Sam Overhaul build.
cl /nologo /c /O2 /GS- /GR- /EHsc- /Zl /Oi /W4 /WX ^
  /Fo"%OUT%\sam_overhaul.obj" "%ROOT%\src\sam_overhaul.cpp"
if errorlevel 1 exit /b 2
cl /nologo /c /std:c++17 /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /DNDEBUG /W4 /WX ^
  /I"%MH%\include" /Fo"%OUT%\footprints.obj" "%ROOT%\src\footprints\footprints.cpp"
if errorlevel 1 exit /b 2
cl /nologo /c /std:c++17 /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /DNDEBUG /W4 /WX ^
  /I"%MH%\include" /Fo"%OUT%\construction_ranges.obj" "%ROOT%\src\construction_ranges.cpp"
if errorlevel 1 exit /b 2
cl /nologo /c /std:c++17 /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /DNDEBUG /W4 /WX ^
  /Fo"%OUT%\construction_refresh.obj" "%ROOT%\src\construction_refresh.cpp"
if errorlevel 1 exit /b 2
cl /nologo /c /std:c++17 /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /DNDEBUG /W4 /WX ^
  /I"%MH%\include" /Fo"%OUT%\construction_charger_update.obj" "%ROOT%\src\construction_charger_update.cpp"
if errorlevel 1 exit /b 2
cl /nologo /c /TC /O2 /GS- /Zl /Oi- /W4 /WX ^
  /Fo"%OUT%\minimal_crt.obj" "%ROOT%\src\footprints\minimal_crt.c"
if errorlevel 1 exit /b 2
cl /nologo /c /std:c++17 /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /DNDEBUG /W4 /WX ^
  /I"%MH%\include" /Fo"%OUT%\construction_shelter.obj" "%ROOT%\src\construction_shelter.cpp"
if errorlevel 1 exit /b 2
rem Scoped RepairSpray native source gate (MASM preserves original CPU context).
ml64 /nologo /c /Fo"%OUT%\construction_repair_gate_asm.obj" "%ROOT%\src\construction_repair_gate.asm"
if errorlevel 1 exit /b 2
cl /nologo /c /std:c++17 /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /DNDEBUG /W4 /WX ^
  /Fo"%OUT%\construction_repair_gate.obj" "%ROOT%\src\construction_repair_gate.cpp"
if errorlevel 1 exit /b 2
rem Targeted built-in German Timefall Shelter action label, no extra dependencies.
cl /nologo /c /std:c++17 /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /DNDEBUG /W4 /WX ^
  /Fo"%OUT%\shelter_rest_label.obj" "%ROOT%\src\shelter_rest_label.cpp"
if errorlevel 1 exit /b 2
rem Upstream MinHook is compiled unchanged under its normal vendor warning policy.
cl /nologo /c /TC /O2 /GS- /Zl /Oi /DNDEBUG /W3 /WX- ^
  /I"%MH%\include" /I"%MH%\src" /Fo"%OUT%\minhook\\" ^
  "%MH%\src\buffer.c" "%MH%\src\hook.c" "%MH%\src\trampoline.c" "%MH%\src\hde\hde64.c"
if errorlevel 1 exit /b 2
rc /nologo /fo "%OUT%\version.res" "%ROOT%\src\version.rc"
if errorlevel 1 exit /b 3
link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib ^
  /dynamicbase /nxcompat /highentropyva /cetcompat /incremental:no /MAP:"%OUT%\DS2_Sam_Overhaul_v%VERSION%.map" ^
  /implib:"%OUT%\DS2_Sam_Overhaul_v%VERSION%.lib" ^
  /out:"%OUT%\DS2_Sam_Overhaul_v%VERSION%.asi" ^
  "%OUT%\sam_overhaul.obj" "%OUT%\footprints.obj" "%OUT%\construction_ranges.obj" "%OUT%\construction_refresh.obj" "%OUT%\construction_charger_update.obj" "%OUT%\construction_shelter.obj" "%OUT%\construction_repair_gate.obj" "%OUT%\construction_repair_gate_asm.obj" "%OUT%\shelter_rest_label.obj" "%OUT%\minimal_crt.obj" ^
  "%OUT%\minhook\buffer.obj" "%OUT%\minhook\hook.obj" ^
  "%OUT%\minhook\trampoline.obj" "%OUT%\minhook\hde64.obj" ^
  "%OUT%\version.res" kernel32.lib bcrypt.lib
if errorlevel 1 exit /b 4
exit /b 0
