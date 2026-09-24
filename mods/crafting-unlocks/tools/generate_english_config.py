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
    expected = {
        row['recipe_key'] for row in source
        if row['status_at_this_facility'] == 'eligible' and '[{0}]' not in row['name']
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
    if keys != expected or len(rows) != 90:
        raise ValueError('Mapping does not cover the 90 reviewed recipes exactly.')

    lines = [
        '; DS2 Crafting Unlocks - compact English INI (1.0.0)',
        '; 1 = early access | 0 = normal progression (never relocks an unlocked recipe).',
        '; Restart the game after editing. Keep each 0x... recipe key unchanged and unique.',
        '; Normal fabrication + backpack modules/covers/charms. Native menu access is required.',
        '; Slots, layout, vehicles, patches and other customization remain unchanged.',
        '; LW = lightweight, SL = silenced, RD = remote-detonated. Labels are display comments.',
        '',
        '[CraftingUnlocks]',
        'Enabled=1',
        'DefaultUnlock=1       ; Default for unlisted recipes or entries set to inherit.',
        'ExportCatalogue=0     ; 1 = export diagnostic files with names in the game language.',
        '',
        '[Items]',
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
