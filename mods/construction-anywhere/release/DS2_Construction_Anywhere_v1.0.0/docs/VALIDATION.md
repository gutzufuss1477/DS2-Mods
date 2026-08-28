# Static validation

Target validation was performed against the locally installed `DS2.exe` version `1.10.89.0`.

```text
PE timestamp: 0x6A3DAE46
Image size: 0x0B292000
SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B
NearbyFacility signature occurrences: 1
PermittedArea signature occurrences: 1
UnconstructableArea signature occurrences: 1
All metadata, signature and uniqueness checks: OK
```

The runtime guard checks PE timestamp, image size, and every enabled 20-byte signature. The SHA-256 above is checked by the offline validator and is not calculated inside the ASI. The ASI was compiled as a PE32+ x86-64 DLL with entry point `DllMain`, no C/C++ runtime dependency, and imports only the required Kernel32 functions. The exact gameplay coverage completed for 1.0.0 is recorded below; `TEST_PLAN.md` remains the regression matrix.

## Runtime validation completed on 2026-08-28

- Multiple ordinary PCC structures were successfully built far closer together than the original minimum distance allows.
- Multiple structures were successfully built inside the tested facility/building no-build area.
- The user reported no crash or immediate gameplay failure during those checks.
- A dedicated monorail proximity warning remained active. This is intentional because specialized monorail/track constraints are outside the patched generic checks.

Not yet claimed as validated: the default-off `PermittedArea` whitelist bypass, every structure category, online synchronization, and long-term save/reload persistence.
