#pragma once
// Shared host configuration; disabling recipes preserves saved weapon resources.
struct SilencedWeaponSettings {
 bool enabled=false;
 bool recipes[4]={true,true,true,true};
 bool blackRed=true;
 bool attachments[3]={true,true,true};
 unsigned int language=0; // 0=game, 1=German, 2=English
};
bool InstallSilencedWeapons(unsigned long long image,const SilencedWeaponSettings& settings,void (*logger)(const char*));
