@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-llvm.ps1"
pause
