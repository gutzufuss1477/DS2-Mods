# Test plan

Use a backed-up save and begin offline. Version 1.0.0 has automated coverage for every supported descriptor and live placement-HUD confirmation for Generator and Zipline; the remaining rows document additional manual coverage.

## 1. Offline validation

Run from PowerShell:

```powershell
./scripts/test-synthetic.ps1
python ./tools/validate_target.py "C:\path\to\DS2.exe"
```

Expected results:

- the production ASI builds with warnings treated as errors;
- packaged settings and synthetic runtime exports return success;
- the target executable matches the expected SHA-256, PE timestamp, image size, construction-root RVA, manager VTable, and all five exact plus semantic QpidCost-site checks;
- an unsupported or modified executable fails validation.

## 2. Disabled baseline

1. Set `Enabled=0` and restart DS2.
2. Record available and used chiral bandwidth.
3. Inspect several construction costs and build one disposable native structure.
4. Confirm native UI, charge, and dismantling behavior.

## 3. Parser and fail-closed behavior

Test one invalid configuration at a time and fully restart DS2:

- missing or malformed `Enabled`;
- negative, signed, fractional, hexadecimal, overflow, or trailing-text cost;
- unknown or duplicate key;
- truncated INI section.

No requested construction cost should be partially applied. Preserve the log for each failure.

## 4. Per-construction matrix

Test each supported key independently while every other key is `Native`:

| Key | Native reference | Custom test value | UI | Build charge | Save/reload | Dismantle credit |
| --- | ---: | ---: | --- | --- | --- | --- |
| `Postbox` | 200 | 101 | [ ] | [ ] | [ ] | [ ] |
| `SafeHouse` | 2500 | 102 | [ ] | [ ] | [ ] | [ ] |
| `Watchtower` | 250 | 103 | [ ] | [ ] | [ ] | [ ] |
| `CargoCatapult` | 450 | 104 | [ ] | [ ] | [ ] | [ ] |
| `Generator` | 350 | 5 | [x] | [ ] | [ ] | [ ] |
| `RainShelter` | 500 | 106 | [ ] | [ ] | [ ] | [ ] |
| `Zipline` | 500 | 5 | [x] | [ ] | [ ] | [ ] |
| `Bridge` | 1000 | 108 | [ ] | [ ] | [ ] | [ ] |
| `ChiralBridge` | 250 | 109 | [ ] | [ ] | [ ] | [ ] |
| `JumpRamp` | 550 | 110 | [ ] | [ ] | [ ] | [ ] |
| `HotSpringDigger` | 0 | 111 | [ ] | [ ] | [ ] | [ ] |

For each row:

1. Record bandwidth immediately before construction.
2. Confirm the placement UI shows the configured value where DS2 normally presents a cost.
3. Build exactly one disposable structure.
4. Confirm used bandwidth changes by the configured amount.
5. Save, fully restart, and reload without changing the INI.
6. Confirm the structure and bandwidth state remain coherent.
7. Dismantle it and confirm the credited amount equals the configured charge.

The checked HUD cells for Generator and Zipline were confirmed from live screenshots on 2026-08-29. They do not by themselves confirm build charge, persistence, or dismantle credit.

For `Bridge`, repeat the complete sequence for the 30 m, 45 m, and 80 m variants. Confirm the one INI key reaches all three configurations and that the two 30 m manager aliases do not cause a double charge or duplicate physical write. Synthetic tests cover both identical and distinct valid runtime addresses for those aliases.

## 5. Multiple configured types

1. Configure at least four types with distinct values.
2. Enter a region where all selected resources are available.
3. Confirm all requested values become active together.
4. Transition between maps or major regions and revisit the build menus.
5. Confirm recreated resources receive the same startup values without log spam.

## 6. Zero cost and capacity boundary

1. Set one ordinarily nonzero type to `0`.
2. Fill available chiral bandwidth using native structures.
3. Confirm the zero-cost type can be placed without underflow or wraparound.
4. Save, reload, and dismantle it; confirm bandwidth remains coherent.
5. Restore the backed-up save after the experiment.

## 7. Existing structures and cost changes

This is a characterization test, not a recommended workflow:

1. Use a disposable backed-up save.
2. Build one structure using cost A and save.
3. Exit completely, change only that type to cost B, and reload.
4. Record used bandwidth before and after load.
5. Dismantle the old structure and record the credited amount.
6. Restore the backup afterwards.

If the credited amount differs from the original charge, retain the permanent warning that users must not change costs after structures exist.

## 8. Native systems regression

Confirm that the mod does not intentionally alter:

- construction material requirements;
- upgrades and repairs;
- placement distance, terrain, collision, facility, road, or chiral-network checks;
- total available chiral bandwidth;
- roads, monorails, mines, story structures, or unsupported construction types;
- ordinary save/load with an unchanged INI.

Test online/shared construction behavior separately only after offline tests pass.

## 9. Conflict behavior

With a controlled test ASI or debugger, place a third value in one requested `QpidCost` field. The mod should report a conflict and perform no new writes for that resource generation. It must not overwrite the third-party value.

Stop testing and remove the ASI if DS2 crashes, bandwidth becomes negative or implausible, unrelated structures change, a rollback is reported as critical, or save/load behavior becomes inconsistent. Preserve the INI, log, exact action sequence, and save backup.
