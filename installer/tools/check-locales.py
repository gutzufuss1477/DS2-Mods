"""Check UTF-8 resource coverage and formatting contracts (Python standard library only)."""
import json
import pathlib
import re
import sys

root = pathlib.Path(__file__).resolve().parents[1]
codes = ['en', 'de', 'fr', 'es', 'it', 'zh-CN', 'ja', 'ko', 'pt-BR', 'ru']

def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError('Duplicate resource key: ' + key)
        result[key] = value
    return result

source = json.loads((root / 'locales/en.json').read_text(encoding='utf-8'), object_pairs_hook=unique_object)
errors = []
for code in codes:
    translations = json.loads((root / ('locales/' + code + '.json')).read_text(encoding='utf-8'), object_pairs_hook=unique_object)
    if set(translations) != set(source):
        errors.append(code + ': resource key set differs from English')
    for key, value in translations.items():
        if not isinstance(value, str) or not value.strip():
            errors.append(code + ': empty/non-string value for ' + key)
            continue
        if '\ufffd' in value:
            errors.append(code + ': replacement character in ' + key)
        if sorted(re.findall(r'\{\d+[^{}]*\}', key)) != sorted(re.findall(r'\{\d+[^{}]*\}', value)):
            errors.append(code + ': format placeholders differ: ' + key)
        for pattern in [r'^\s*', r'\s*$']:
            if re.search(pattern, key).group() != re.search(pattern, value).group():
                errors.append(code + ': boundary whitespace differs: ' + key)
        # Windows OpenFileDialog filter delimiters and machine patterns are structural.
        if '|DS2.exe|' in key and key.split('|')[1::2] != value.split('|')[1::2]:
            errors.append(code + ': file dialog filter patterns differ')
    print(code + ': ' + str(len(translations)) + ' strings')

literal = r'"(?:[^"\\]|\\.)*"'
for path in (root / 'src').glob('*.cs'):
    if 'Test' in path.name:
        continue
    for match in re.finditer(r'Localization\.(?:T|Format)\(\s*(' + literal + r')', path.read_text(encoding='utf-8')):
        key = json.loads(match.group(1))
        if key and key not in source:
            errors.append(path.name + ': untracked UI string: ' + key)

if errors:
    for error in errors:
        print(error, file=sys.stderr)
    sys.exit(1)
print('PASS language coverage, UTF-8, placeholders, concatenation and file-dialog contracts')
