# Security Notes

`DS2VehicleCore.asi` is an in-process native plugin loaded by an external x64
ASI loader.

The source:

- registers one event-driven listener for streamed game resources;
- validates resource types, signatures, counts and the expected streaming
  window before changing values;
- modifies vehicle-physics data only inside the current game process;
- reads two local INI files beside the ASI;
- writes a local diagnostic log named `DS2VehicleCore.log`.

It contains no networking, downloading, telemetry, registry persistence,
service installation or modification of unrelated processes.

The external ASI loader is not part of this repository.
