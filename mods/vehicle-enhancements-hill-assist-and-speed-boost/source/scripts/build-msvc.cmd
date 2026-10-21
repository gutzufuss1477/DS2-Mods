@echo off
setlocal
powershell.exe -NoProfile -File "%~dp0..\..\..\tools\native\build-hardened.ps1" -Mod vehicle -Toolchain MSVC
exit /b %errorlevel%
