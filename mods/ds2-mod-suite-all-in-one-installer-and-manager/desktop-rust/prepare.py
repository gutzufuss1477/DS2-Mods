"""Generate shared UI/backend data from the existing verified Suite catalog."""
import argparse, hashlib, json, re
from pathlib import Path

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--payload-root',type=Path,required=True)
    parser.add_argument('--catalog',type=Path,help='Override the Rust release catalog for development builds.')
    args=parser.parse_args()
    base=Path(__file__).resolve().parent
    source=base.parent/'source'
    catalog=json.loads((args.catalog or base/'catalog.json').read_text(encoding='utf-8-sig'))
    schema=json.loads((source/'settings-schema.json').read_text(encoding='utf-8-sig'))
    templates={}
    for mod in catalog['mods']:
        for f in mod['files']:
            if f['kind']!='config':continue
            raw=(args.payload_root/f['payload']).read_bytes()
            assert hashlib.sha256(raw).hexdigest().lower()==f['sha256'].lower(),f['target']
            # Shared variant INIs use one canonical template and common fields.
            templates.setdefault(f['target'],raw.decode('utf-8-sig'))
    fields=[]
    for mod in schema['mods']:
        for file in mod['files']:
            defaults={}; comments={}; section=''; pending=[]
            for line in templates[file['target']].splitlines():
                s=line.strip()
                if s.startswith(';'):pending.append(s.lstrip(';').strip());continue
                if s.startswith('[') and s.endswith(']'):section=s[1:-1].strip();pending=[];continue
                if '=' in s and not s.startswith('#'):
                    key,value=s.split('=',1); defaults[(section.lower(),key.strip().lower())]=value.split(';')[0].strip()
                    comments[(section.lower(),key.strip().lower())]=' '.join(pending)
                pending=[]
            for sec in file['sections']:
                for field in sec['fields']:
                    key=(sec['name'].lower(),field['key'].lower())
                    default=field.get('default',defaults.get(key))
                    if default is None:
                        for alias in field.get('aliases',[]):
                            default=defaults.get((sec['name'].lower(),alias.lower()))
                            if default is not None:break
                    assert default is not None,key
                    fields.append({**field,'id':'|'.join([mod['modId'],file['target'],sec['name'],field['key']]),'modId':mod['modId'],'target':file['target'],'section':sec['name'],'defaultValue':default,'label':field.get('label',re.sub(r'(?<=[a-z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])',' ',field['key'])),'description':field.get('description',comments.get(key,''))})
    fields.extend(catalog.get('managedSettings',[]))
    data={'catalog':catalog,'fields':fields,'templates':templates,'locales':{p.stem:json.loads(p.read_text(encoding='utf-8-sig')) for p in (source/'locales').glob('*.json')}}
    assert len({f['id'] for f in fields})==len(fields)
    expected=json.loads((base/'package.json').read_text())['version']
    assert catalog['suiteVersion']==expected, 'Catalog/package version mismatch'
    for mod in catalog['mods']:
        for key in mod.get('settingOverrides',{}):
            assert any(f['id']==key and f.get('managedByVariant') for f in fields), key
    for locale, translations in json.loads((base/'release-locales.json').read_text(encoding='utf-8')).items():
        data['locales'][locale].update(translations)
    generated=base/'src-tauri/generated';generated.mkdir(parents=True,exist_ok=True)
    serialized=json.dumps(data,ensure_ascii=False,separators=(',',':'))
    (generated/'data.json').write_text(serialized,encoding='utf-8')
    (base/'ui').mkdir(exist_ok=True)
    (base/'ui/data.js').write_text('window.DS2_DATA = '+serialized+';\n',encoding='utf-8')
    print(f'Generated {len(fields)} fields, {len(templates)} INIs, {len(data["locales"])} languages.')

if __name__=='__main__':main()
