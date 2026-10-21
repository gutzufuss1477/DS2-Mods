@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title DEATH STRANDING 2 Vehicle Mods - Uninstall

fltmc >nul 2>&1
if errorlevel 1 (
    powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass ^
      -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass ^
  -File "%~dp0Setup.ps1" -Action Uninstall -Selection Menu

echo.
pause
