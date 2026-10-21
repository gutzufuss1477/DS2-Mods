# Validation checklist – Infrastructure One Unit v1.1.0

Previously verified original v1.0.0 infrastructure functions:
- [x] Road construction, stage 1
- [x] Road upgrades, stage 2
- [x] Monorail construction
- [x] Mine construction, stage 1
- [x] Mine upgrades, stages 2 and 3
- [x] Existing other-player/online contributions
- [x] Partial container handling and native overflow
- [x] Save and reload after construction/upgrade
- [x] World and overview refresh

New v1.1.0 mine material extraction:
- [x] Native cost and UI selection reduced to 1 crystal
- [x] Normal output and 1-crystal deduction at multiple mines
- [x] Corrected 1/1 denominator and fill bar in the active mine menu
- [x] Offline cost matrix: 0, 1, 2, 5, 10, 25, 50, 100, 500, 1000, 1500
- [x] Native opcode/PE signature guards for Steam DS2.exe v1.10.89.0
- [x] Existing infrastructure-policy regression tests
- [ ] In-game check of individual positive values other than 1 (such as 5/10/100)
- [ ] Every mine material and level separately verified after this update
- [ ] Independent repair/fabrication regression on v1.1.0

Passing offline tests does not imply a value was tested in-game.
