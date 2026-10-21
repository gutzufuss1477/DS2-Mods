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
where llvm-rc.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: llvm-rc.exe was not found.
  exit /b 1
)
where llvm-dlltool.exe >nul 2>nul
if errorlevel 1 (
  echo ERROR: llvm-dlltool.exe was not found.
  exit /b 1
)

if not exist build mkdir build

echo [1/4] Creating the minimal import library...
llvm-dlltool.exe -m i386:x86-64 -d "src\kernel32.def" -l "build\kernel32.lib"
if errorlevel 1 exit /b 1

echo [2/4] Compiling with clang-cl...
clang-cl.exe /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /Oi /clang:-fno-builtin /std:c++17 ^
  /DWIN32 /D_WINDOWS /D_USRDLL ^
  /Fo:"build\pickup_cargo_capacity_patch.obj" ^
  "src\pickup_cargo_capacity_patch.cpp"
if errorlevel 1 exit /b 1

echo [3/4] Compiling version information...
llvm-rc.exe /FO "build\PickupCargoCapacity.res" "src\PickupCargoCapacity.rc"
if errorlevel 1 exit /b 1

echo [4/4] Linking hardened ASI...
lld-link.exe /dll /entry:DllMain /nodefaultlib /machine:x64 /dynamicbase /nxcompat /highentropyva /brepro ^
  /out:"build\PickupCargoCapacity.asi" ^
  "build\pickup_cargo_capacity_patch.obj" "build\PickupCargoCapacity.res" "build\kernel32.lib"
if errorlevel 1 exit /b 1

echo.
echo Built: %CD%\build\PickupCargoCapacity.asi
exit /b 0
