# Technical Notes - Sam Overhaul v1.0.0

Target executable:
- Steam DS2.exe 1.10.89.0
- PE timestamp: 0x6A3DAE46
- SizeOfImage: 0x0B292000
- SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B

Key validated systems:
- DSPlayerEquipmentManage visibility routing for carried cargo.
- Backpack outline/bind-effect suppression for hidden backpack cargo.
- Spare-shoe visibility path while preserving worn shoes.
- Monorail final blocker bypass only; earlier CanJumpDown state checks stay native.
- Zipline detach action gate while retaining native input/animation.
- Landing classifier/animation path with temporary native-state substitution and restoration.
- Vehicle Autodrive readiness timer with configurable seconds.
- DSVehicleWeaponPartsResource streaming patcher for IDs 118/119, 163, 165 and 169.
- Chiral Cannon Weapon ID 163 -> Ammo ID 275 -> DSAmmoParameter +0x80 charge value.
  Vanilla 3.0 seconds; default release value 0.75 seconds.

The public v1.0.0 source intentionally excludes the failed footprint and worn-path experiments used during development.
