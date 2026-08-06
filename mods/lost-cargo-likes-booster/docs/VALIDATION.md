# Validation

Validated on Steam PC DS2 build 1.10.89.0, timestamp `0x6A3DAE46`, image size `0x0B292000`.

- The lost-cargo writer used component sequence `34 + 17 + 12` and normally awarded `+63`.
- With `ExtraLikes=5000`, production runtime validation observed `+5063` and `APPLIED=1`.
- A regular-order comparison had `RBX=1`; the production guard requires `RBX=0x22` and excludes it.
- The production hook has no timeout or diagnostic hit cap and remains active until game exit.

Build evidence is in `build/public/PE_REPORT.txt`; the release hash is in `SHA256SUMS.txt`. A clean-copy build should be repeated before publishing and must produce the same SHA-256.
