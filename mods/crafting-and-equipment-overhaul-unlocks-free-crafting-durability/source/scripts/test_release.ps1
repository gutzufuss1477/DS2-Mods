[CmdletBinding()]
param([string]$LlvmBin='C:\Program Files\LLVM\bin')
$ErrorActionPreference='Stop'
Push-Location (Split-Path -Parent $PSScriptRoot)
try {
    New-Item -ItemType Directory -Path build -Force | Out-Null
    $compiler=Join-Path $LlvmBin 'clang-cl.exe'
    python tools/enforce_addon_api_v1.py
    if($LASTEXITCODE -ne 0){throw 'Addon ABI gate failed'}
    $cases=@('core_tests','backpack_tests','atlas_config_tests','silenced_config_tests',
      'coffin_direct_model_tests','coffin_menu_alias_tests','coffin_story_loaded_lookup_tests',
      'coffin_view_scope_tests','coffin_archive_path_scope_tests','coffin_menu_tests',
      'coffin_preview_tests','coffin_session_tests','coffin_persistence_tests',
      'cosmetics_tests','cosmetics_runtime_tests')
    foreach($case in $cases){
        & $compiler /nologo /std:c++17 /EHsc /MT /O2 "tests/$case.cpp" "/Febuild/$case.exe"
        if($LASTEXITCODE -ne 0){throw "Compile failed: $case"}
        if($case -in @('backpack_tests','atlas_config_tests','silenced_config_tests','cosmetics_tests')){
            & "build/$case.exe" config/default.ini
        }else{& "build/$case.exe"}
        if($LASTEXITCODE -ne 0){throw "Test failed: $case"}
    }
    python tools/test_persistence_runtime.py
    if($LASTEXITCODE -ne 0){throw 'Persistence runtime test failed'}
    python tools/test_repeat_fabrication.py
    if($LASTEXITCODE -ne 0){throw 'Repeated fabrication test failed'}
}finally{Pop-Location}
