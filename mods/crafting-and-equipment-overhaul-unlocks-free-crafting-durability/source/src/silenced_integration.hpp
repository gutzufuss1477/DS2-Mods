#pragma once
// Shared host configuration; disabling recipes preserves saved weapon resources.
struct SilencedWeaponSettings {
 bool enabled=false;
 bool recipes[4]={true,true,true,true};
 bool blackRed=true;
 bool attachments[3]={true,true,true};
 unsigned int language=0; // 0=game, 1=German, 2=English
 unsigned int warriorAmmoMultiplier=1; // 1=vanilla, 2..10: native Warrior magazines
 unsigned int warriorReserveMode=0; // 0=vanilla, 1=ammo-loader backpack, 2=unlimited reserve reloads
 unsigned int warriorMagazineRounds[3]={0,0,0}; // 0=legacy multiplier; otherwise explicit 1..300
 unsigned int warriorReserveMagazineLimit=0; // 0=uncapped native stock; 1..30=maximum magazines held in stock
 bool freeChiralGuitarShots=false; // both Sam and red guitar; no chiral cost (independent of crafting cost)
 bool redGuitarEnabled=false;
};
bool InstallSilencedWeapons(unsigned long long image,const SilencedWeaponSettings& settings,void (*logger)(const char*));
