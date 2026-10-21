@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title DEATH STRANDING 2 Vehicle Mods - Install

fltmc >nul 2>&1
if errorlevel 1 (
    powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass ^
      -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass ^
  -File "%~dp0Setup.ps1" -Action Install -Selection Menu

echo.
pause
