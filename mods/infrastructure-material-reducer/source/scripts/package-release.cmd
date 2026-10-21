@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0package-release.ps1" %*
pause
