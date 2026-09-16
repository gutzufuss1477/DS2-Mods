# v1.0.3 Performance Notes

## Reason for the change

Feedback on the earlier main release reported a major FPS reduction while the APAS mod
was active.

The earlier design kept rescanning the APAS table on a permanent interval.

## v1.0.3 changes

- No permanent 1-second scan during normal gameplay.
- Initial scan delayed by 3 seconds.
- Partial resource tables are polled only every 5 seconds.
- Once at least 40 resources are present, the worker checks only until stable.
- Five stable passes with no writes cause the worker thread to exit.
- Entry-array memory is validated once per pass rather than once per slot.
- APAS cost logic is unchanged.

## Promotion status

This build began as a focused performance test.

After user testing indicated that it runs better, the exact tested binary was promoted
to the main v1.0.3 release.

Do not silently replace the binary with a fresh rebuild when investigating future
performance or antivirus differences; first compare against the SHA-256 recorded in
`SHA256SUMS.txt`.
