# Building Infrastructure One Unit v1.1.0

## Windows / LLVM

Requirements: Windows PowerShell, LLVM (clang-cl.exe and lld-link.exe), Python 3 for regression tests.

Run:

~~~powershell
.\scripts\build-llvm.ps1
~~~

Outputs:
- build/public/ds2_infrastructure_one_unit.asi
- build/public/ds2_infrastructure_one_unit.ini
- build/public/SHA256.txt

This builds the existing mod with the new optional mining-price feature. No separate mining ASI is created.

## Offline tests

~~~powershell
py -3 .\tests\test_policy.py
py -3 .\tests\test_mine_cost_scaling.py
~~~

The test suite checks existing infrastructure material policies and selected/required/consumed mine crystals across multiple INI values. Further Ghidra and exact DS2.exe opcode checks were conducted in the development environment; those require a local supported game executable.

## Nexus release package

~~~powershell
.\scripts\package-release.ps1
~~~

The packaging script validates the version, builds the ASI from the current source, and creates:

- release/DS2_Infrastructure_One_Unit_v1.1.0/ (ASI, INI, README, CHANGELOG, SHA256)
- release/DS2_Infrastructure_One_Unit_v1.1.0.zip
- release/DS2_Infrastructure_One_Unit_v1.1.0.zip.sha256.txt

The reference ASI and the ASI inside the Nexus ZIP are the exact same v1.1.0 build.
