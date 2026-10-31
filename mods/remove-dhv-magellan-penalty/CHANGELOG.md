# Changelog

## v1.1.0 — 31 October 2026

- Fixed the Magellan penalty for all supported native reward selectors (1–8), rather than only selector 2.
- Restored the game's **actual calculated rank** for qualifying Magellan deliveries, including its saved Standard Order best-rank history. Does not force S or maximum Likes.
- Corrected the result screen's Magellan-only evaluation and the delivery-record flag handling.
- Added independent INI toggles for Likes and rank correction, plus optional diagnostic logging.
- Restricted the runtime changes to verified Magellan conditions and the exact Steam build `1.10.89.0`, with instruction-signature validation.
- Confirmed with Order 310: native Legend/S grade stored as `0x20` instead of Magellan-only `0x01`. Further order types, selector 7, difficulty settings and achievement counting are not individually verified.

## v1.0.0

- Initial Like penalty correction for selector 2. The Magellan-only evaluation rank was still applied.
