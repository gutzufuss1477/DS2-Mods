#pragma once
// Preserves vanilla item and vehicle identity in every save.
namespace coffin_save_identity {
constexpr unsigned originalCoffinRecipe=0x0184CF05u;
constexpr unsigned originalCoffinBaggage=0x2A889790u;
constexpr bool variantIsRuntimeOnly=true;
// v1.9.56 persists only SHA256 fingerprints in a separate mod-owned file.
// The native game/save resource identities remain the original ones.
constexpr bool separateVariantMetadata=true;
constexpr bool mayRegisterPersistentResourceId=false;
constexpr bool mayMutateGameOrSaveFiles=false;
}
