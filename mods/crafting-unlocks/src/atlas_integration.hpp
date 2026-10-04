#pragma once
// Called only after the host verifies the executable and pins its module.
// Recipe visibility can be disabled without invalidating previously saved items.
bool InstallAtlasEquipment(unsigned long long image,bool enableRecipes,bool goldSkeletonSkin,void (*logger)(const char*));
