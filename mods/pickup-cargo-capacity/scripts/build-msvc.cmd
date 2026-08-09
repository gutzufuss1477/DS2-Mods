@echo off
setlocal EnableExtensions
cd /d "%~dp0\.."

where cl.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: cl.exe was not found.
  echo Run this script from "x64 Native Tools Command Prompt for VS 2022".
  exit /b 1
)
where rc.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: rc.exe was not found.
  echo Run this script from "x64 Native Tools Command Prompt for VS 2022".
  exit /b 1
)

if not exist build mkdir build

echo [1/3] Compiling with MSVC and static runtime...
cl.exe /nologo /c /O2 /GS /guard:cf /GR- /EHsc- /Oi /MT /std:c++17 ^
  /DWIN32 /D_WINDOWS /D_USRDLL ^
  /Fo"build\pickup_cargo_capacity_patch.obj" ^
  "src\pickup_cargo_capacity_patch.cpp"
if errorlevel 1 exit /b 1

echo [2/3] Compiling version information...
rc.exe /nologo /fo "build\PickupCargoCapacity.res" "src\PickupCargoCapacity.rc"
if errorlevel 1 exit /b 1

echo [3/3] Linking hardened ASI...
link.exe /nologo /dll /machine:x64 /guard:cf /dynamicbase /nxcompat /highentropyva ^
  /out:"build\PickupCargoCapacity.asi" ^
  "build\pickup_cargo_capacity_patch.obj" "build\PickupCargoCapacity.res" kernel32.lib
if errorlevel 1 exit /b 1

echo.
echo Built: %CD%\build\PickupCargoCapacity.asi
exit /b 0
