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
cl /nologo /c /TC /O2 /GS- /Zl /Oi- /W4 /WX ^
  /Fo"%OUT%\minimal_crt.obj" "%ROOT%\src\footprints\minimal_crt.c"
if errorlevel 1 exit /b 2
rem Upstream MinHook is compiled unchanged under its normal vendor warning policy.
cl /nologo /c /TC /O2 /GS- /Zl /Oi /DNDEBUG /W3 /WX- ^
  /I"%MH%\include" /I"%MH%\src" /Fo"%OUT%\minhook\\" ^
  "%MH%\src\buffer.c" "%MH%\src\hook.c" "%MH%\src\trampoline.c" "%MH%\src\hde\hde64.c"
if errorlevel 1 exit /b 2
rc /nologo /fo "%OUT%\version.res" "%ROOT%\src\version.rc"
if errorlevel 1 exit /b 3
link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib ^
  /dynamicbase /nxcompat /highentropyva /cetcompat /incremental:no ^
  /implib:"%OUT%\DS2_Sam_Overhaul_v%VERSION%.lib" ^
  /out:"%OUT%\DS2_Sam_Overhaul_v%VERSION%.asi" ^
  "%OUT%\sam_overhaul.obj" "%OUT%\footprints.obj" "%OUT%\minimal_crt.obj" ^
  "%OUT%\minhook\buffer.obj" "%OUT%\minhook\hook.obj" ^
  "%OUT%\minhook\trampoline.obj" "%OUT%\minhook\hde64.obj" ^
  "%OUT%\version.res" kernel32.lib bcrypt.lib
if errorlevel 1 exit /b 4
exit /b 0