@echo off
setlocal EnableExtensions
cd /d "%~dp0\.."

where cl.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: cl.exe was not found.
  echo Run this script from "x64 Native Tools Command Prompt for VS 2022".
  exit /b 1
)

if not exist build mkdir build

echo [1/2] Compiling with MSVC...
cl.exe /nologo /c /O2 /GS- /Gs999999 /GR- /EHsc- /Zl /Oi /std:c++17 ^
  /DWIN32 /D_WINDOWS /D_USRDLL ^
  /Fo"build\pickup_cargo_capacity_patch.obj" ^
  "src\pickup_cargo_capacity_patch.cpp"
if errorlevel 1 exit /b 1

echo [2/2] Linking ASI...
link.exe /nologo /dll /entry:DllMain /nodefaultlib /machine:x64 ^
  /out:"build\PickupCargoCapacity.asi" ^
  "build\pickup_cargo_capacity_patch.obj" kernel32.lib
if errorlevel 1 exit /b 1

echo.
echo Built: %CD%\build\PickupCargoCapacity.asi
exit /b 0
