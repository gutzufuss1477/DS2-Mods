# Building DS2 Mod Suite 1.13.0

The source import omits precompiled payloads. Supply a local directory containing
the exact `payload/...` paths referenced by `catalog.json`, and invoke
`powershell -File build.ps1 -PayloadRoot <directory>`. Every payload hash is
checked against the published suite catalog before packaging. The suite's pinned
component versions are distinct from the latest independent Nexus mod downloads.

No payload is fetched automatically, and no quarantined file should be restored
to make the build pass. The source import is not a new malware or gameplay verdict.
The `WithoutApas` option builds a comparison package from the same inputs.
Current upload and scan outcomes are recorded under `publication/v1.13.0/`.

On Windows PowerShell 5.1, use `-OutputRoot D:\DS2-Suite-Build` when the checkout
path would put release files beyond the Windows path limit. The build removes
only its named DS2-Mod-Suite child and the exact versioned output ZIP within that
directory. Reparse points are rejected. Do not use a game folder as the output.

Version 1.13.0 uses the unchanged published APAS 3.0.0, not the experimental 4.0
rewrite. `release-inputs-v1.13.0.json` records exact standalone archives and
payload hashes. Unchanged components come from accepted Nexus file 379.
Keep prebuilt payloads local; only source, manifests and licenses belong in Git.

The build compiles the production executable and a separate regression runner,
then packages only production files after all self-tests pass. Also run
`python tools/check-locales.py`. An offline test pass is not an in-game test or
an antivirus approval; check the exact final archive separately.
