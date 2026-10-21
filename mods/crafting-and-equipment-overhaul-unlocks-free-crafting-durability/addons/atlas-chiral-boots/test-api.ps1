$ErrorActionPreference='Stop'
Push-Location $PSScriptRoot
try {
 $cc='C:\Program Files\LLVM\bin\clang-cl.exe';$ld='C:\Program Files\LLVM\bin\lld-link.exe'
 $py='D:\DS2-Asset-Lab\tools\geometry-venv\Scripts\python.exe'
 foreach ($name in @('api_runtime_tests','api_host_tests','api_harness')) {
  & $cc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /Isrc /c "tests/$name.cpp" "/Fobuild/api/$name.obj"
  if ($LASTEXITCODE) {throw "Compile failed: $name"}
  if ($name -eq 'api_harness') {
   & $ld /dll /entry:DllMain /machine:x64 /nodefaultlib /out:build/api/api_harness.dll build/api/api_harness.obj build/api/icon_gpu.obj build/api/embedded_payload.obj build/api/kernel32.lib
  } else {
   $objects=@("build/api/$name.obj",'build/api/kernel32.lib')
   if ($name -eq 'api_runtime_tests') {$objects+='build/api/icon_gpu.obj';$objects+='build/api/embedded_payload.obj'}
   & $ld /entry:mainCRTStartup /subsystem:console /machine:x64 /nodefaultlib "/out:build/api/$name.exe" @objects
  }
  if ($LASTEXITCODE) {throw "Link failed: $name"}
  if ($name -ne 'api_harness') {& ".\build\api\$name.exe";if ($LASTEXITCODE) {throw "Test failed: $name"}}
 }
 & $py tests/test_api_merge.py
 if ($LASTEXITCODE) {throw 'Compiled graph/cache tests failed'}
 & $py tests/test_api_loader.py --host development/crafting-v198-addon-api/build/nexus-v199/ds2_crafting_unlocks.asi --addon release/test16-single-asi/ds2_atlas_chiral_addon.asi --report evidence/test16-loader.json
 if ($LASTEXITCODE) {throw 'ASI loader/API checks failed'}
 & $cc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /c tests/icon_gpu_test.cpp /Fobuild/api/icon_gpu_test.obj
 if ($LASTEXITCODE) {throw 'GPU test compilation failed'}
 $sdk=Get-ChildItem 'C:\Program Files (x86)\Windows Kits\10\Lib' -Directory | Sort-Object Name -Descending | Select-Object -First 1
 & $ld /entry:mainCRTStartup /subsystem:console /machine:x64 /nodefaultlib /out:build/api/icon_gpu_test.exe build/api/icon_gpu_test.obj build/api/kernel32.lib (Join-Path $sdk.FullName 'um/x64/d3d12.lib')
 if ($LASTEXITCODE) {throw 'GPU test link failed'}
 & .\build\api\icon_gpu_test.exe
 if ($LASTEXITCODE) {throw 'GPU pixel readback failed'}
} finally {Pop-Location}
