DS2 Sneaky Sam v1.0.0

Target
- DS2.exe v1.10.89.0
- SHA-256 BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B

Release ASI
- SHA-256 57EB2FBA2B33AF665C5B9F3C24213E10C26B3BC3FA154263A6C1F60914D0A1A7
- Size 5632 bytes
- Build A == Build B byte-for-byte: YES
- PE32+ / x86-64 DLL
- PE timestamp: 0
- HIGH_ENTROPY_VA: YES
- DYNAMIC_BASE: YES
- NX_COMPAT: YES
- Imports: KERNEL32.dll only

Validated native path
- DSPlayerEquipmentManage visibility refresh RVA 0xF6BF30
- visibility callsite RVA 0xF6C09D
- original bytes E8 7E 84 22 00
- original native target RVA 0x1194520
- target slot types:
  4 RightArm
  5 LeftArm
  6 RightWaist
  7 LeftWaist

In-game validation of v0.1.0-test1
- all four target slots became invisible
- carried cargo remained functional
- moving cargo back to the backpack restored normal visibility
- runtime log confirmed all four slot types reached the hook with native visible=1

Release delta from tested build
- hook, slot filter and native target unchanged
- per-slot development telemetry removed
- release version/name and mutex updated
- startup/fail-closed log retained

Release deliberately does not modify
- inventory
- weight/capacity
- cargo condition
- slot ownership
- save data
- AI/BT/enemy detection
- movement/gameplay stats
