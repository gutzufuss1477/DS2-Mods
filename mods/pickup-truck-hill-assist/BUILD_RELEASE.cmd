@echo off
setlocal
cd /d "%~dp0"

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: cl.exe was not found.
    echo Run this file from an x64 Native Tools Command Prompt for Visual Studio.
    pause
    exit /b 1
)

where link.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: link.exe was not found.
    pause
    exit /b 1
)

if not exist build mkdir build

cl.exe /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /std:c++17 ^
    /Fobuild\PickupTruckHillAssist.obj ^
    src\PickupTruckHillAssist.cpp

if errorlevel 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

link.exe /nologo /dll /entry:DllMain /nodefaultlib /machine:x64 ^
    /subsystem:windows /out:build\PickupTruckHillAssist.asi ^
    build\PickupTruckHillAssist.obj kernel32.lib

if errorlevel 1 (
    echo Linking failed.
    pause
    exit /b 1
)

echo.
echo Build completed:
echo %CD%\build\PickupTruckHillAssist.asi
echo.
pause
