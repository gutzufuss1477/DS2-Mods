@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1

set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\public"
if not exist "%OUT%" mkdir "%OUT%"

cl /nologo /c /O2 /GS- /GR- /EHsc- /Zl /Oi /W4 /WX ^
  /Fo"%OUT%\sam_overhaul.obj" "%ROOT%\src\sam_overhaul.cpp"
if errorlevel 1 exit /b 2

rc /nologo /fo "%OUT%\version.res" "%ROOT%\src\version.rc"
if errorlevel 1 exit /b 3

link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib ^
  /dynamicbase /nxcompat /highentropyva /cetcompat ^
  /implib:"%OUT%\DS2_Sam_Overhaul_v1.0.0.lib" ^
  /out:"%OUT%\DS2_Sam_Overhaul_v1.0.0.asi" ^
  "%OUT%\sam_overhaul.obj" "%OUT%\version.res" kernel32.lib
if errorlevel 1 exit /b 4

exit /b 0
