Infrastructure One Unit v1.0.0

Supported game:
- DEATH STRANDING 2: ON THE BEACH
- Steam PC v1.10.89.0

Install:
1. Install a compatible x64 ASI loader.
2. Copy ds2_infrastructure_one_unit.asi and ds2_infrastructure_one_unit.ini
   next to DS2.exe / the ASI loader.
3. Restart the game.

Supported infrastructure:
- Roads: Stage 1 construction and Stage 2 upgrades
- Monorails: Stage 1 construction
- Mines: Stage 1 construction and Stage 2/3 upgrades

Configuration:
- RemainingUnits=1 means one remaining unit per still-open material category.
- Higher values are supported.
- Existing online/player contributions are preserved.
- A smaller native remainder is never increased.

The game keeps native container splitting and overflow handling.
Repairs and fabrication are not modified.

Save warning:
The game can persist the reduced progress. Back up a save before first use.
