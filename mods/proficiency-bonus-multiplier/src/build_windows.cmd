@echo off
setlocal
set "SRC=%~dp0"
set "ROOT=%SRC%.."
set "OUT=%ROOT%\build"

set "LLVM_BIN=C:\Program Files\LLVM\bin"
if exist "%LLVM_BIN%\clang-cl.exe" goto :llvm_found
where clang-cl.exe >nul 2>nul || (
  echo ERROR: clang-cl.exe not found. Install LLVM for Windows or add it to PATH.
  exit /b 1
)
for %%I in (clang-cl.exe) do set "CLANG=%%~$PATH:I"
for %%I in (lld-link.exe) do set "LLD=%%~$PATH:I"
goto :build

:llvm_found
set "CLANG=%LLVM_BIN%\clang-cl.exe"
set "LLD=%LLVM_BIN%\lld-link.exe"

:build
if not exist "%OUT%" mkdir "%OUT%"
"%LLD%" /lib /def:"%SRC%kernel32.def" /machine:x64 /out:"%OUT%\kernel32.lib" || exit /b 1
"%CLANG%" --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /Gs9999999 /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin /Fo:"%OUT%\mod.obj" "%SRC%ds2_proficiency_bonus_multiplier.cpp" || exit /b 1
"%LLD%" /dll /nodefaultlib /entry:DllMain /machine:x64 /out:"%OUT%\DS2_ProficiencyBonusMultiplier.asi" "%OUT%\mod.obj" "%OUT%\kernel32.lib" || exit /b 1
echo Built: %OUT%\DS2_ProficiencyBonusMultiplier.asi
