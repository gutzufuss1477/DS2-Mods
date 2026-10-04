# Crafting & Equipment Overhaul 1.7.0 validation

Based on origin/main commit `0f172ff3503b2809a07f507b6988876af6539c62`, retaining
the complete 1.6.1 gold ATLAS implementation. Weapon resources, native clone
builder, visible attachments, material/icon logic and pixel payloads come
from the user-accepted standalone More Silenced Guns 0.5.2 build.

The host now supplies configuration and logging to the weapon component on
its existing initialization worker after the exact executable gate and module
pinning. No second ASI, DllMain, startup worker, INI or log is required.
The original ATLAS callsite and weapon constructor-entry hooks remain separate;
all 94 installed-site signatures match the reviewed executable with no overlap.

Four active recipe identities are added to the shared FreeCrafting allowlist
(138 recipes total). Their four baggage identities and the hidden legacy 302
baggage are added to the native durability allowlist (139 entries total).
ATLAS gameplay and gold selection are unchanged. Public defaults enable ATLAS,
gold and all four red weapons; missing keys in older INIs retain their previous
opt-in recipe and normal-skin behavior.

Completed local checks:

- Windows host policy/layout/selection/SHA suite: 335437 assertions.
- Original configuration regression fixture: 9297 assertions.
- Current 132-item backpack/configuration suite: 13603 assertions.
- FreeCrafting native instruction suite: 101376 cases, 304628 assertions,
  88 unwind checks; 138 supported and six excluded identities.
- Production FreeCrafting transaction tests: 26900 assertions.
- Targeted Usage=None UI filter: 35 cases.
- Native durability suite: 1002001 assertions, including 139 scoped baggage IDs.
- ATLAS shared configuration/identity checks: 202 assertions.
- New weapon/shared configuration and policy checks: 829 assertions.
- ATLAS native suite: 20 tests and 110208 native branch cases.
- Weapon native/clone/material/attachment/icon suite: 44 tests, including real
  D3D12 uploads with full BC1/BC7 readback and both shotgun sound paths.
- Integrated ASI loader: disabled, wrong executable and invalid configuration
  cases in disposable Windows processes. No hooks installed in those processes.
- AMD64 PE, ASLR/NX, unwind/relocation data, KERNEL32-only imports, no writable
  executable section, 94 exact hook signatures and no overlapping hook spans.

The standalone weapon loader test was replaced by the integrated host loader
checks. Test logs and hashes are listed in `validation/summary-1.7.0.json`.

The user's acceptance of the standalone weapons and the existing ATLAS gold
gameplay/save-load confirmation do not establish a gameplay pass for this
newly combined binary. Combined startup, fabrication, existing saved custom
items and save/load still need a game session after migration. No game process,
game installation, save file or Nexus publication was modified for this release.
