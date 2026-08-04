@echo off
setlocal EnableExtensions
cd /d "%~dp0\.."

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: cl.exe was not found.
    echo Run this file from an x64 Native Tools Command Prompt for VS 2022.
    exit /b 1
)

where link.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: link.exe was not found.
    exit /b 1
)

if not exist build mkdir build

cl.exe /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /std:c++17 ^
    /Fobuild\DS2VehicleCore.obj ^
    src\DS2VehicleCore.cpp
if errorlevel 1 exit /b 1

link.exe /nologo /dll /entry:DllMain /nodefaultlib /machine:x64 ^
    /subsystem:windows ^
    /out:build\DS2VehicleCore.asi ^
    build\DS2VehicleCore.obj kernel32.lib
if errorlevel 1 exit /b 1

echo Build completed: %CD%\build\DS2VehicleCore.asi
