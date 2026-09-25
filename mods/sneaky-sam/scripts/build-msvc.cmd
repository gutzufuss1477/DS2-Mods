@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1

set ROOT=%~dp0..
if not exist "%ROOT%\build\public" mkdir "%ROOT%\build\public"
del /q "%ROOT%\build\public\*" 2>nul

cl /nologo /c /O2 /GS- /GR- /EHsc- /Zl /Oi /W4 /WX /Fo"%ROOT%\build\public\sneaky_sam.obj" "%ROOT%\src\sneaky_sam.cpp"
if errorlevel 1 exit /b 2

rc /nologo /fo "%ROOT%\build\public\version.res" "%ROOT%\src\version.rc"
if errorlevel 1 exit /b 3

link /nologo /dll /machine:x64 /entry:DllMain /nodefaultlib /dynamicbase /nxcompat /highentropyva /cetcompat /out:"%ROOT%\build\public\DS2_SneakySam_v1.0.1.asi" "%ROOT%\build\public\sneaky_sam.obj" "%ROOT%\build\public\version.res" kernel32.lib
if errorlevel 1 exit /b 4

powershell -NoProfile -Command "$h=(Get-FileHash '%ROOT%\build\public\DS2_SneakySam_v1.0.1.asi' -Algorithm SHA256).Hash.ToUpper(); Set-Content '%ROOT%\build\public\SHA256.txt' ($h + '  DS2_SneakySam_v1.0.1.asi') -Encoding ascii; Write-Host ('SHA256=' + $h)"
exit /b 0
