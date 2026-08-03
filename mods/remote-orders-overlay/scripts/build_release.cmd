@echo off
setlocal
cd /d "%~dp0\.."
where py >nul 2>nul
if errorlevel 1 (
  echo Python launcher "py" was not found.
  exit /b 1
)
if not exist dist mkdir dist
py -3 tools\patch_no_log.py validated_baseline\DS2_Remote_Orders_Intermediate_Overlay_compact_width.asi dist\DS2_Remote_Orders_Overlay.asi
if errorlevel 1 exit /b 1
echo Release created in dist\DS2_Remote_Orders_Overlay.asi
