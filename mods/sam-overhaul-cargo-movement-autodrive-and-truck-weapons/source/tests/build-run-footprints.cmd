@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
set "ROOT=%~dp0.."
set "OUT=%ROOT%\build\tests"
set "PUB=%ROOT%\build\public"
set "MH=%ROOT%\vendor\minhook"
set /p VERSION=<"%ROOT%\VERSION.txt"
if not exist "%OUT%" mkdir "%OUT%"
set MHLIBS="%PUB%\minhook\buffer.obj" "%PUB%\minhook\hook.obj" "%PUB%\minhook\trampoline.obj" "%PUB%\minhook\hde64.obj"
rem Execute the hook tests with the same no-CRT flags/primitives as the shipped ASI.
cl /nologo /c /O2 /GS- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0 /Zl /Oi /std:c++17 /W4 /WX /I"%MH%\include" ^
 /Fo"%OUT%\footprints_runtime_tests.obj" "%ROOT%\tests\footprints_runtime_tests.cpp"
if errorlevel 1 exit /b 2
link /nologo /machine:x64 /subsystem:console /entry:TestEntry /nodefaultlib /dynamicbase /nxcompat ^
 /out:"%OUT%\footprints_runtime_tests.exe" "%OUT%\footprints_runtime_tests.obj" ^
 "%PUB%\minimal_crt.obj" %MHLIBS% kernel32.lib
if errorlevel 1 exit /b 2
"%OUT%\footprints_runtime_tests.exe"
if errorlevel 1 exit /b 3
cl /nologo /O2 /MT /EHsc /std:c++17 /W4 /WX /I"%MH%\include" ^
 /Fo"%OUT%\footprints_config_tests.obj" /Fe"%OUT%\footprints_config_tests.exe" ^
 "%ROOT%\tests\footprints_config_tests.cpp" "%PUB%\footprints.obj" %MHLIBS% bcrypt.lib
if errorlevel 1 exit /b 4
"%OUT%\footprints_config_tests.exe"
if errorlevel 1 exit /b 5
cl /nologo /O2 /MT /EHsc /std:c++17 /W4 /WX ^
 /Fo"%OUT%\combined_asi_loader.obj" /Fe"%OUT%\combined_asi_loader.exe" ^
 "%ROOT%\tests\combined_asi_loader.cpp"
if errorlevel 1 exit /b 6
"%OUT%\combined_asi_loader.exe" "%PUB%\DS2_Sam_Overhaul_v%VERSION%.asi"
if errorlevel 1 exit /b 7
exit /b 0
