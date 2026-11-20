#pragma once

namespace equipment {
// Native EDSItemId of the boot supplying the model.
enum class BootAppearance : unsigned char {
 Transporter=11, Trekking=12, Combat=13, Sneaking=14, Bandit=15,
 Brigand=16, Survivalist=17, Absolute=18, Mule=19, PizzaChef=20,
 Omnireflector=93, Chiral=94, Default=PizzaChef
};

struct BootModel {
 BootAppearance appearance;
 const char *key, *english, *german, *nativeName;
};

inline constexpr BootModel BootModels[]={
 {BootAppearance::Transporter,"Transporter","Transporter Boots",u8"Transportstiefel","TransporterBoots"},
 {BootAppearance::Trekking,"Trekking","Trekking Boots",u8"Wanderstiefel","TrekkingBoots"},
 {BootAppearance::Combat,"Combat","Combat Boots",u8"Kampfstiefel","CombatBoots"},
 {BootAppearance::Sneaking,"Sneaking","Sneaking Boots",u8"Schleichstiefel","SneakingBoots"},
 {BootAppearance::Bandit,"Bandit","Bandit Boots",u8"Banditenstiefel","BanditBoots"},
 {BootAppearance::Brigand,"Brigand","Brigand Boots",u8"Brigand-Stiefel","BrigandBoots"},
 {BootAppearance::Survivalist,"Survivalist","Survivalist Boots",u8"Waffennarren-Stiefel","TerroristBoots"},
 {BootAppearance::Absolute,"Absolute","Absolute Boots",u8"Allround-Stiefel","AbsoluteBoots"},
 {BootAppearance::Mule,"Mule","MULE Boots",u8"MULE-Stiefel","MuleBoots"},
 {BootAppearance::PizzaChef,"PizzaChef","Pizza Chef's Boots",u8"Stiefel des Pizzabäckers","PizzeriaBoots"},
 {BootAppearance::Omnireflector,"Omnireflector","Omnireflector Boots",u8"Omnireflektor-Stiefel","AllReflectorBoots"},
 {BootAppearance::Chiral,"Chiral","Chiral Boots",u8"Chirale Stiefel","ChiralBoots"},
};
BootAppearance configuredBootAppearance();
bool configuredCombatBokkaRecipe();
}
