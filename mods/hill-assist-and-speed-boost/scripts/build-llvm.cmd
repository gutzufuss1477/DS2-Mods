@echo off
setlocal EnableExtensions
cd /d "%~dp0\.."

where clang-cl.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: clang-cl.exe was not found.
    echo Install the LLVM tools for Visual Studio and use a developer prompt.
    exit /b 1
)

where lld-link.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: lld-link.exe was not found.
    exit /b 1
)

if not exist build mkdir build

clang-cl.exe /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /std:c++17 ^
    /Fobuild\DS2VehicleCore.obj ^
    src\DS2VehicleCore.cpp
if errorlevel 1 exit /b 1

lld-link.exe /dll /entry:DllMain /nodefaultlib /machine:x64 ^
    /subsystem:windows ^
    /out:build\DS2VehicleCore.asi ^
    build\DS2VehicleCore.obj kernel32.lib
if errorlevel 1 exit /b 1

echo Build completed: %CD%\build\DS2VehicleCore.asi
