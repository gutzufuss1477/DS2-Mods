@echo off
setlocal EnableExtensions
cd /d "%~dp0\.."

where clang-cl.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: clang-cl.exe was not found.
  echo Install "C++ Clang tools for Windows" in Visual Studio Installer,
  echo then run this script from "x64 Native Tools Command Prompt for VS 2022".
  exit /b 1
)
where lld-link.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: lld-link.exe was not found.
  exit /b 1
)

if not exist build mkdir build

echo [1/2] Compiling with clang-cl...
clang-cl.exe /nologo /c /O2 /GS- /Gs999999 /GR- /EHs-c- /Zl /Oi /std:c++17 ^
  /DWIN32 /D_WINDOWS /D_USRDLL ^
  /Fo:"build\pickup_cargo_capacity_patch.obj" ^
  "src\pickup_cargo_capacity_patch.cpp"
if errorlevel 1 exit /b 1

echo [2/2] Linking with lld-link...
lld-link.exe /dll /entry:DllMain /nodefaultlib /machine:x64 ^
  /out:"build\PickupCargoCapacity.asi" ^
  "build\pickup_cargo_capacity_patch.obj" kernel32.lib
if errorlevel 1 exit /b 1

echo.
echo Built: %CD%\build\PickupCargoCapacity.asi
exit /b 0
