# Native Menu Research Notes

The overlay confirms reusable components for the long-term native-menu implementation.

## Mission source

```text
missionSource = *(*(DS2 base + 0x0623EB30) + 0x10)
mission count = uint32 at missionSource + 0x38
mission pointer array = missionSource + 0x40
array element width = 8 bytes
```

## Native MissionMenu display record

The existing `DSUIMissionMenuController` provides a reusable output record at controller offset `+0x5D0`.

Confirmed sequence:

```text
DS2 base + 0x01734CE0  reset/initialize display record
DS2 base + 0x01738AA0  build display record from mission pointer
```

Relevant output fields:

```text
+0x08 localized fallback mission text
+0x10 localized recipient/facility name
+0x28 mission title
+0x30 destination/facility candidate
```

The next native approach should retain the proven world-map selection and UI-thread handoff, then trace the remaining controller activation/refresh state used by the normal terminal **Accept Orders** flow.
