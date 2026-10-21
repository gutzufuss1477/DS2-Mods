#pragma once
// Internal integration boundary; do not include Windows headers in the legacy module.
// 0 waiting, 1 disabled, 2 active, 3 invalid config, 4 probe conflict,
// 5 unsupported executable, 6 native-context mismatch, 7 hook error.
extern "C" bool SamFootprintsInstall(void* imageBase,const wchar_t* iniPath,void* log);
extern "C" void SamFootprintsPoll(void* log);
extern "C" __declspec(dllexport) unsigned SamOverhaulFootprintsState();
namespace sam_footprints {
    bool ReadEnabled(const wchar_t* iniPath,bool* invalid);
}
