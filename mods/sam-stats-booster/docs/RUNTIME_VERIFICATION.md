# Runtime Verification Summary

The release uses before/after reads around DS2's original play-record mutators. This proved that the scaled deltas, rather than only the hook counters, reached persistent game storage.

- Double path: exact `20.0x` storage confirmed for Terrain Traversal, Carrying Capacity, Endurance and KO Resistance.
- Q64 path: exact `50.0x` storage confirmed for Lung Capacity.
- Q32 path: exact `50.0x` storage confirmed for Melee Combat and Assault Rifles.
- Visual confirmation: Lung Capacity reached level 5 quickly and the new levels appeared in Sam's Stats menu.

See `VALIDATION.md` for the recorded values.
