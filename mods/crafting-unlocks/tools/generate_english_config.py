#!/usr/bin/env python3
"""Generate the compact INI without changing the mod or its accepted grammar."""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
from pathlib import Path

BASE = Path(__file__).resolve().parents[1]


def generate(mapping: Path, catalogue: Path, output: Path) -> dict[str, object]:
    with mapping.open(encoding='utf-8-sig', newline='') as handle:
        rows = list(csv.DictReader(handle, delimiter='\t'))
    with catalogue.open(encoding='utf-8-sig', newline='') as handle:
        source = list(csv.DictReader(handle, delimiter='\t'))
    by_key = {row['recipe_key']: (i + 2, row) for i, row in enumerate(source)}
    if len(by_key) != len(source):
        raise ValueError('Duplicate source recipe keys; manual review required.')
    special_normal = {'0x75D99124', '0x07B21227', '0x6C3A478B', '0x1EA3AF0B', '0x6CC82C08', '0x1E51C488', '0x380248E3', '0x4A69CBE0', '0x38F02360', '0x14889C47', '0x4B7F7765', '0x3914F466', '0x0CE5E07A'}  # Omnireflector + Chiral Boots: targeted Usage=None fabrication exceptions.
    expected = {
        row['recipe_key'] for row in source
        if (row['status_at_this_facility'] == 'eligible' and '[{0}]' not in row['name'])
        or row['recipe_key'] in special_normal
    }
    keys: set[str] = set()
    for row in rows:
        key = row['recipe_key']
        if not re.fullmatch(r'0x[0-9A-F]{8}', key) or key in keys:
            raise ValueError(f'Invalid or duplicate mapped key: {key}')
        keys.add(key)
        if key not in expected:
            raise ValueError(f'Not a reviewed equipment/weapon recipe: {key}')
        line, original = by_key[key]
        if int(row['source_line']) != line:
            raise ValueError(f'Source-line mismatch: {key}')
        for field in ('label_en', 'category'):
            value = row[field]
            if not value or not value.isascii() or '\n' in value or '\r' in value:
                raise ValueError(f'Invalid {field} for {key}')
    if keys != expected or len(rows) != 103:
        raise ValueError('Mapping does not cover the 103 reviewed normal fabrication recipes exactly.')

    lines = [
        '; DS2 Crafting & Equipment Overhaul 1.6.0',
        '; Restart the game after changing this file.',
        '; 1 = enabled | 0 = disabled',
        '',
        '[CraftingUnlocks]',
        'Enabled=1              ; Global master switch. 0 = no patches from this mod.',
        'DefaultUnlock=1        ; 1 = listed items are available early unless their item value is 0.',
        'FreeCrafting=0         ; 1 = supported recipes cost no materials.',
        'ExportCatalogue=0      ; 1 = export diagnostic catalogue files.',
        '',
        '[AtlasEquipment]',
        'Enabled=0              ; 1 = fabricate ATLAS Boots and ATLAS Skeleton without progression gates.',
        '                       ; 0 = hide their recipes; existing ATLAS items remain usable.',
        '                       ; Requires the global master switch; remove the old standalone ATLAS ASI.',
        '',
        '[Durability]',
        'Enabled=0              ; 1 = enable durability for supported equipment, boots and backpack covers.',
        'Multiplier=2.0         ; 1.0 = vanilla life, 2.0 = about 2x life, max 1000.0.',
        'Unbreakable=0          ; 1 = no future durability loss; overrides Multiplier.',
        '                       ; Cargo/order cargo/material containers remain vanilla.',
        '',
        '[Items]',
        '; 1 = available early | 0 = keep native progression/acquisition.',
        '; 0 never removes an item that the game unlocks normally.',
    ]
    category = None
    for row in rows:
        if row['category'] != category:
            if category is not None:
                lines.append('')
            category = row['category']
            lines.append('; --- ' + category.upper() + ' ---')
        lines.append(row['recipe_key'] + '=1  ; ' + row['label_en'])
    with (BASE / 'config/backpack.en.tsv').open(encoding='utf-8-sig', newline='') as handle:
        backpack = list(csv.DictReader(handle, delimiter='\t'))
    counts = {'Module': 0, 'Cover': 0, 'Charm': 0}
    for row in backpack:
        key = row['recipe_key']
        if key in keys or key not in by_key or by_key[key][1]['usage'] != 'BackPackCustomize':
            raise ValueError(f'Invalid backpack key: {key}')
        if row['kind'] not in counts or not row['english_label'].isascii():
            raise ValueError(f'Invalid backpack category/label: {key}')
        counts[row['kind']] += 1
        keys.add(key)
    if counts != {'Module': 19, 'Cover': 2, 'Charm': 8}:
        raise ValueError('Backpack mapping must contain exactly 19 modules, 2 covers and 8 charms.')
    for kind, title in [('Module', 'BACKPACK MODULES'), ('Cover', 'BACKPACK COVERS'), ('Charm', 'BACKPACK CHARMS')]:
        lines.extend(['', '; --- ' + title + ' ---'])
        for row in backpack:
            if row['kind'] == kind:
                lines.append(row['recipe_key'] + '=1  ; ' + row['english_label'])
    payload = ('\r\n'.join(lines) + '\r\n').encode('ascii')
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(payload)
    return {
        'output': str(output),
        'recipe_count': len(rows) + len(backpack),
        'line_count': len(lines),
        'bytes': len(payload),
        'sha256': hashlib.sha256(payload).hexdigest(),
        'catalogue_sha256': hashlib.sha256(catalogue.read_bytes()).hexdigest(),
        'all_item_values': 1,
        'encoding': 'ASCII (UTF-8 compatible), CRLF, no BOM',
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--mapping', type=Path, default=BASE / 'config/items.en.tsv')
    parser.add_argument('--catalogue', type=Path,
                        default=BASE / 'evidence/config-r1/ds2_crafting_catalogue.tsv')
    parser.add_argument('--output', type=Path, default=BASE / 'release/ds2_crafting_unlocks.ini')
    args = parser.parse_args()
    try:
        result = generate(args.mapping, args.catalogue, args.output)
    except (OSError, ValueError, KeyError, csv.Error) as error:
        parser.exit(1, f'Generation failed: {error}\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
