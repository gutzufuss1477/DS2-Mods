# Early APAS menu research status

Scope: determine whether the APAS item can be made visible on a new save without
changing global story or facility facts.

## Verified on DS2.exe 1.10.89.0

- The APAS system resource is object `1178:5921` in the installed game's streaming
  graph.  It holds 54 APAS node resources and its `ChangeActiveGraph` is
  `1178:10673`.
- `DSApasEnhancementSystemResource` has no `InitFact` or menu-visibility field.  Its
  serialized state is node references, memory/placement values, the activation graph,
  and endgame exception lists.
- `DSUIRingDeviceMenuDataSourceItemApasResource` is object `56:42556`.  It contains
  labels/icons only; it has no fact, unlock, or visibility field.
- The native APAS updater at `0xBE38D0` creates nodes through the existing native
  locate/unlock function at `0xBE1640`.  The current `UnlockAll` option safely skips
  each APAS node's own EnableFact/grade prerequisite while preserving manager and
  resource guards.
- The APAS script predicate `IsApasEnhancementLocated` is native code at
  `0xBE4B10`.  It returns true only when the APAS manager contains a live entry for
  the requested APAS ID.  It does not read a story fact.

## Conclusion

The early ring-menu gate is not an editable APAS resource property.  It is in the
native UI/graph invocation path and has not yet been isolated to the exact APAS ID
or additional predicate it evaluates.

Do not add an `UnlockMenu` INI key until that call site is identified.  A generic
fact write or a broad UI patch could alter unrelated tutorial/story state and would
not meet the mod's safety requirements.

The existing `UnlockAll=1` remains the only supported test configuration. It is
independent of `GlobalCost` and uses the native APAS creation path. The first
Episode-2 attempt did not reach this code: rc.1 rejected startup with
`UNSUPPORTED_OR_CONFLICT`. A fresh-save test is only valid after the local log says
`READY`; it is still needed to establish whether native node creation alone causes
the ring-menu item to appear on this game build.
