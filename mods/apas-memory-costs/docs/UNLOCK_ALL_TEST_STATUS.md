# Tested Unlock All Alpha

The first live Unlock All build was tested successfully before the release rebuild.

Observed result:
- locked APAS Enhancements became available on an Episode 9 save;
- all displayed APAS Enhancements could be activated;
- GlobalCost=1 continued to work.

Tested alpha ASI SHA-256:
EC7387FA5DD17970154B084B87351C0F82A9198480E0B15111C7A27D9DB8F2CE

Release v1.1.0 uses the same source logic but was rebuilt with a conventional x64 PE
layout including `.pdata` and `.reloc` sections to reduce the Nexus antivirus/quarantine
risk seen with earlier ultra-minimal ASI builds.

Release ASI SHA-256:
C8042768729D3F1FA07FFE20A9B0A8CCC854C793FB102B187CF365B0D004ACD8
