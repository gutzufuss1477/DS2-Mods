# Sneaky Sam v1.0.1 validation

Target:
- DS2.exe v1.10.89.0
- SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B

Native path:
- DSPlayerEquipmentManage visibility refresh RVA 0xF6BF30
- visibility callsite RVA 0xF6C09D
- original instruction bytes: E8 7E 84 22 00
- original target RVA 0x1194520

Target slot types:
- 4 RightArm
- 5 LeftArm
- 6 RightWaist
- 7 LeftWaist

In-game validation:
- all four target slots hidden
- cargo remains fully carried and functional
- cargo moved back to backpack becomes visible again
- inventory/weight/condition/gameplay remain unchanged

v1.0.1 build changes:
- Microsoft Visual C++ / Windows SDK build
- standard Windows version resource
- explicit InitializeASI and SneakySamVersion exports
- relay allocation is RW followed by VirtualProtect to RX
- no RWX allocation for relay memory

Security/scanner validation:
- Microsoft Defender: ASI clean
- Microsoft Defender: release ZIP clean
- Nexus archive scanner: accepted

Release ASI SHA-256:
C23B727A08227B786C2C3DDAF77DECCCFE43ABF68D8CD3D69B27CE5B493E4DAB
