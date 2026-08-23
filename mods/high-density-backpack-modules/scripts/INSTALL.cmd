@echo off
setlocal EnableExtensions
chcp 65001 >nul

title DS2 High-Density Backpack Modules v1.0.0 - Install
set "ROOT=%~dp0..\"
set "SOURCE=%ROOT%reference\DS2_HighDensityBackpackModules_v1.0.0.asi"
set "EXPECTED=C4C5CC2B525BEB88AEF264521B56C00E6616ABF6DFB0C2F6970C92379D719C1C"

echo DS2 High-Density Backpack Modules v1.0.0
echo.

if not exist "%SOURCE%" (
    echo [ERROR] The ASI file is missing from the package.
    echo Extract the complete ZIP before running this installer.
    echo.
    pause
    exit /b 1
)

tasklist /FI "IMAGENAME eq DS2.exe" 2>nul | find /I "DS2.exe" >nul
if not errorlevel 1 (
    echo [ERROR] DS2.exe is still running. Close the game completely and try again.
    echo.
    pause
    exit /b 1
)

set "GAME_DIR="
for /f "usebackq delims=" %%I in (`powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%tools\Find-DS2GameDir.ps1" 2^>nul`) do if not defined GAME_DIR set "GAME_DIR=%%I"

if not defined GAME_DIR (
    echo The DS2 game directory was not found automatically.
    set /p "GAME_DIR=Enter the complete folder containing DS2.exe: "
)

if not exist "%GAME_DIR%\DS2.exe" (
    echo [ERROR] DS2.exe was not found in:
    echo %GAME_DIR%
    echo.
    pause
    exit /b 1
)

echo Target: "%GAME_DIR%"
echo Removing only older High-Density / Unlimited Backpack test files ...
del /q "%GAME_DIR%\DS2_UnlimitedBackpack_*Probe*.asi" 2>nul
del /q "%GAME_DIR%\DS2_UnlimitedBackpack_*Prototype*.asi" 2>nul
del /q "%GAME_DIR%\DS2_UnlimitedBackpack_*Probe.log" 2>nul
del /q "%GAME_DIR%\DS2_UnlimitedBackpack_*Prototype.log" 2>nul
del /q "%GAME_DIR%\DS2_HighDensityBackpackModules_v*.asi" 2>nul
del /q "%GAME_DIR%\DS2_HighDensityBackpackModules.log" 2>nul

copy /y "%SOURCE%" "%GAME_DIR%\DS2_HighDensityBackpackModules_v1.0.0.asi" >nul
if errorlevel 1 (
    echo [ERROR] The ASI could not be copied. Try running the installer as administrator.
    echo.
    pause
    exit /b 1
)

set "INSTALLED_HASH="
for /f "tokens=*" %%H in ('powershell.exe -NoProfile -Command "(Get-FileHash -Algorithm SHA256 -LiteralPath '%GAME_DIR%\DS2_HighDensityBackpackModules_v1.0.0.asi').Hash"') do set "INSTALLED_HASH=%%H"

if /I not "%INSTALLED_HASH%"=="%EXPECTED%" (
    echo [ERROR] The installed ASI SHA-256 does not match.
    echo Expected: %EXPECTED%
    echo Found:    %INSTALLED_HASH%
    del /q "%GAME_DIR%\DS2_HighDensityBackpackModules_v1.0.0.asi" 2>nul
    echo.
    pause
    exit /b 1
)

if not exist "%GAME_DIR%\version.dll" if not exist "%GAME_DIR%\dinput8.dll" if not exist "%GAME_DIR%\winmm.dll" (
    echo.
    echo [WARNING] No common ASI-loader DLL was detected beside DS2.exe.
    echo Install a compatible external 64-bit ASI loader if your other ASI mods are not already loading.
)

echo.
echo [OK] Installation and SHA-256 verification completed.
echo Start DS2, then check:
echo "%GAME_DIR%\DS2_HighDensityBackpackModules.log"
echo.
echo Required runtime status: status=PATCH_APPLIED
pause
exit /b 0
