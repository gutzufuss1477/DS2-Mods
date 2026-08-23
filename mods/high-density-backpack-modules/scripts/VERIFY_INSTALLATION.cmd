@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul

title DS2 High-Density Backpack Modules v1.0.0 - Verify
set "ROOT=%~dp0..\"
set "EXPECTED=C4C5CC2B525BEB88AEF264521B56C00E6616ABF6DFB0C2F6970C92379D719C1C"
set "GAME_DIR="
for /f "usebackq delims=" %%I in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%tools\Find-DS2GameDir.ps1" 2^>nul`) do if not defined GAME_DIR set "GAME_DIR=%%I"
if not defined GAME_DIR set /p "GAME_DIR=Enter the complete folder containing DS2.exe: "

if not exist "%GAME_DIR%\DS2.exe" (
    echo [ERROR] DS2.exe was not found.
    pause
    exit /b 1
)

echo Game directory: "%GAME_DIR%"
echo.

if exist "%GAME_DIR%\DS2_HighDensityBackpackModules_v1.0.0.asi" (
    set "INSTALLED_HASH="
    for /f "tokens=*" %%H in ('powershell.exe -NoProfile -Command "(Get-FileHash -Algorithm SHA256 -LiteralPath '%GAME_DIR%\DS2_HighDensityBackpackModules_v1.0.0.asi').Hash"') do set "INSTALLED_HASH=%%H"
    echo Installed ASI SHA-256: !INSTALLED_HASH!
    echo Expected SHA-256:      %EXPECTED%
) else (
    echo [ERROR] DS2_HighDensityBackpackModules_v1.0.0.asi is not installed.
)

echo.
echo Common ASI-loader files:
for %%F in (version.dll dinput8.dll winmm.dll) do if exist "%GAME_DIR%\%%F" echo   %%F

echo.
echo Runtime log:
if exist "%GAME_DIR%\DS2_HighDensityBackpackModules.log" (
    powershell.exe -NoProfile -Command "Get-Content -LiteralPath '%GAME_DIR%\DS2_HighDensityBackpackModules.log' -Tail 120"
) else (
    echo No log exists yet. The ASI has not been loaded or the loader is missing.
)

echo.
echo Running DS2 process:
tasklist /FI "IMAGENAME eq DS2.exe"
echo.
pause
