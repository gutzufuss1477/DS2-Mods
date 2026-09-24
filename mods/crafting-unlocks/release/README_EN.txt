Crafting Unlocks - Equipment, Backpack and Charms

Version 1.0.0 for DEATH STRANDING 2: ON THE BEACH (PC, Windows x64).

Make supported equipment available before its normal progression unlock. Normal
fabrication is available as soon as the game provides a suitable fabrication
terminal. Backpack equipment appears in the native backpack customization menu
once that menu is accessible; this mod does not unlock the menu itself.

Included

The compact English INI contains 119 individual selections, all enabled by
default: 90 normal-fabrication equipment/weapon entries, 19 backpack modules,
two backpack covers and eight charms. Equipment levels and variants can be
controlled separately. The numerical keys identify recipes; English comments
are labels and do not depend on the selected game language.

Existing material costs, placement restrictions, capacity checks, backpack
space and charm-slot limits are not removed. The mod grants recipe access,
not free materials, automatic inventory items or automatic charm effects.

Vehicles, vehicle customization, costume customization, backpack patches,
mission-only objects and protected DLC-only recipes are not newly unlocked.
The Coffin Board is included through its native fabrication route; it retains
its native resource and placement requirements. This is not a vehicle unlocker.

Installation

1. Close the game and back up the save you will use.
2. Keep a working compatible 64-bit ASI loader installed. The loader is not
   bundled; reuse the loader already used for your other DS2 ASI mods.
3. Copy ds2_crafting_unlocks.asi and ds2_crafting_unlocks.ini directly beside
   DS2.exe. Do not put the Git/source directory in the game directory.
4. Keep exactly one Crafting Unlocks ASI active. Remove older copies from any
   other active loader folders; do not leave a renamed old .asi there.
5. Start the game and use fabrication or backpack customization normally.

When updating from 0.2.0-alpha, the same 119 settings remain valid. Replace the
ASI. Keep a customized 119-entry INI, or use the new default INI and transfer your
existing 0 exceptions. An older 90-entry INI still parses: unlisted supported
backpack items follow DefaultUnlock. Do not overwrite a working ASI loader.

The standalone archive contains no loader or Mod Manager executable. The
all-in-one DS2 Mod Manager is not modified by this release.

Configuration

Edit only ds2_crafting_unlocks.ini, then restart the game.

- Enabled=1: enable this mod. Enabled=0: install no hooks.
- DefaultUnlock=1: early access for supported unlisted or inherit entries.
- An item value of 1: request early access for this recipe.
- An item value of 0: leave this recipe's unlock conditions to the game.
- ExportCatalogue=0: omit diagnostic catalogue/template exports (default).

Each listed item starts at 1. Find its English name and change that existing
line to 0 to exclude it from early access. Do not duplicate the recipe key.
For example, 0x3BDDCD8E=0 leaves the Battle Guitar on normal progression.

Zero never bans a normally unlocked item. Native list entries are retained
before optional early-access copies are appended. An excluded item appears
normally once the game's own progression makes it available.

Changing DefaultUnlock to 0 does not override the explicit 1 values in the
shipped list. For an opt-in setup, also change unwanted item entries to 0 or
inherit. To disable the entire mod, use Enabled=0 instead.

Do not edit generated .tsv or .generated.ini files as the main configuration.
They are optional diagnostic exports and are not read as settings.

Progression and saves

The hooks do not set or erase original catalogue unlock bits or global story
facts. They preserve the native list and add menu-local recipe copies. Normal
progression remains able to unlock an item regardless of its INI exception.

Crafting, resource consumption and applying a backpack loadout remain real game
operations. Their inventory, equipment and native purchase/manufacture flags can
be saved. Removing this mod does not refund resources or delete crafted items.
The mod does not restore a save to its earlier state. Every later story reward
and unlock notification has not been exhaustively tested.

Supported executable and compatibility

Analysis target: DS2.exe 1.10.89.0, SHA-256:
bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b.
The full executable hash and both hook signatures are required. A different
storefront, patch or identically labelled but different executable is not
certified; a mismatch leaves the mod inactive. Do not bypass this check.

Other mods that touch the same callsites can conflict. This release does not
change backpack layout or the number of charm slots. Compatibility with every
other mod, including High-Density Backpack Modules, has not been established.

Validation

The user confirmed the early normal fabrication list at the first terminal in
Episode 2 and successfully crafted the Battle Guitar into inventory. The user
then confirmed all backpack equipment, rain covers and charms were unlocked
with the 0.2.0-alpha candidate.

Release 1.0.0 keeps that candidate's hook and selection logic. The only change in
runtime source is the startup version text. It is a rebuilt binary, not a
byte-identical copy of the candidate. Host policy/parser tests, sanitizers and
PE checks were rerun. The release binary was not independently run in DS2 in
the build environment. The observations do not establish that every item was
crafted/equipped, all payments were checked or every later save/reward event
was tested. See docs/VALIDATION.md for the exact scope.

Troubleshooting and removal

Inspect ds2_crafting_unlocks.log beside the ASI. A successful start includes
two HOOK_INSTALLED messages. VERSION_BLOCKED, SIGNATURE_BLOCKED or
CONFIG_ERROR is a reason to check the reported cause, not remove safety gates.
A PARTIAL message means not both hooks installed; close the game and retain
the log. A menu log confirms list construction, not payment or equipment use.

For support, send the mod log and the INI; no save upload is required.
The optional collector is in the source package under scripts/.

To remove the mod, close the game and remove ds2_crafting_unlocks.asi and its
INI. Optional diagnostic files can also be removed. Leave a shared ASI loader
in place while other mods need it. Already saved equipment and resource changes
are not undone.

Development

src/ contains the complete runtime source; config/ contains English name
mappings. build.ps1 builds on Windows with LLVM. tools/validate.sh runs host
tests and a Windows-x64 cross-build. tools/generate_english_config.py regenerates
the INI from the included, verified catalogue mapping. evidence/ retains the
source analysis and validation/ records the release checks and baseline diff.
No game executable or asset archive is included.
