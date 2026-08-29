# Validation record

Date: 2026-08-29

## Automated result

`scripts/test-synthetic.ps1` completed successfully with `/W4 /WX` enabled.

The synthetic suite covers the packaged INI, strict parser rejection cases, all supported descriptor mappings, all three bridge lengths, identical and distinct 30 m alias addresses, all-or-none readiness, exact VTables, idempotence, foreign-value conflicts, manager/config identity changes, atomic field races, conditional rollback, read-only-page restoration, protection races, an injected critical restore path, and resource recreation.

Production ASI SHA-256 (v1.0.0):

```text
BA49FAEBC661633945A19AFEDFAE0F3D7A88FA62DCA752735692AA3D436DFC2B
```

The production image is an x64 DLL with ASLR, high-entropy VA, and NX enabled. It exports `DllMain` and `InitializeASI` and imports only the required KERNEL32 functions.

## Target executable

`tools/validate_target.py` passed against the locally installed Steam executable:

```text
Version:     1.10.89.0
Timestamp:   0x6A3DAE46
Image size:  0x0B292000
SHA-256:     BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B
```

The global construction-root mapping, manager VTable signature, and all five exact and semantic `QpidCost + 0x4E4` consumers passed.

## Release package

The archive contains exactly:

```text
ds2_chiral_bandwidth_costs.asi
ds2_chiral_bandwidth_costs.ini
```

Release ZIP SHA-256:

```text
69C463304E3C7999663EF6982C4D6505AD21DFC0BA482F7166C256DC78A03D80
```

The ASI extracted from the ZIP matches the production ASI hash above. The generated sidecar matches the ZIP hash. Two consecutive clean packaging passes produced the same ASI and ZIP hashes.

## Gameplay result

On 2026-08-29, live screenshots from the supported Steam build confirmed that the placement HUD displays an exact INI value of `5` for both Generator and Zipline. The screenshots validate configuration application and the UI cost path for those two types; the displayed used bandwidth remains `725`, so they do not independently establish build charge or refund behavior.

## Remaining gameplay validation

Exact charge/refund behavior, save/reload, dismantling, online structures, map transitions, every other construction type, and the experimental `HotSpringDigger` path remain manual checks. Keep a save backup, begin offline, and do not change configured costs after constructing structures in that save.
