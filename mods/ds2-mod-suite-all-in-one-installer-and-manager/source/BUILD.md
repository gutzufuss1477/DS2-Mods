# Building DS2 Mod Suite 1.12.1

The source import omits precompiled payloads. Supply a local directory containing
the exact `payload/...` paths referenced by `catalog.json`, and invoke
`powershell -File build.ps1 -PayloadRoot <directory>`. Every payload hash is
checked against the published suite catalog before packaging. The suite's pinned
component versions are distinct from the latest independent Nexus mod downloads.

No payload is fetched automatically, and no quarantined file should be restored
to make the build pass. The source import is not a new malware or gameplay verdict.
The local `WithoutApas` diagnostic build option is not an additional repository
release or a listed Nexus download.
