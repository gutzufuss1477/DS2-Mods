@echo off
setlocal EnableExtensions
chcp 65001 >nul

title DS2 High-Density Backpack Modules v1.0.0 - Uninstall
set "ROOT=%~dp0..\"

echo DS2 High-Density Backpack Modules v1.0.0 - Uninstall
echo.
echo IMPORTANT:
echo Before uninstalling, reduce the backpack to a layout that fits the original
echo component footprints, apply the layout and create a new save.
echo.

tasklist /FI "IMAGENAME eq DS2.exe" 2>nul | find /I "DS2.exe" >nul
if not errorlevel 1 (
    echo [ERROR] DS2.exe is still running. Close the game completely.
    echo.
    pause
    exit /b 1
)

set "GAME_DIR="
for /f "usebackq delims=" %%I in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%tools\Find-DS2GameDir.ps1" 2^>nul`) do if not defined GAME_DIR set "GAME_DIR=%%I"
if not defined GAME_DIR set /p "GAME_DIR=Enter the complete folder containing DS2.exe: "

if not exist "%GAME_DIR%\DS2.exe" (
    echo [ERROR] DS2.exe was not found.
    pause
    exit /b 1
)

del /q "%GAME_DIR%\DS2_HighDensityBackpackModules_v*.asi" 2>nul
del /q "%GAME_DIR%\DS2_HighDensityBackpackModules.log" 2>nul

if exist "%GAME_DIR%\DS2_HighDensityBackpackModules_v1.0.0.asi" (
    echo [ERROR] The ASI could not be removed.
    pause
    exit /b 1
)

echo [OK] High-Density Backpack Modules was removed.
pause
exit /b 0
