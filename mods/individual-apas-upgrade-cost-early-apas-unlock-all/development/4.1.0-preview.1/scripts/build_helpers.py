"""Build suite plugins with the normal Windows CRT and compiler protections.

Uses local sources, LLVM/MSVC and the Windows SDK. Does not download, install,
publish, modify game files, or change antivirus settings. Historical releases are
never overwritten. The output manifest records source and artifact hashes.
"""
import argparse
import hashlib
import json
import os
import re
from pathlib import Path
import subprocess
import struct

REPO = Path(__file__).resolve().parents[1]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def verify_pe(path):
    """Require real x64 protection metadata, not just linker command flags."""
    data=path.read_bytes()
    pe=struct.unpack_from('<I',data,0x3c)[0]; optional=pe+24
    if data[:2]!=b'MZ' or data[pe:pe+4]!=b'PE\0\0' or struct.unpack_from('<H',data,pe+4)[0]!=0x8664:
        raise ValueError('Not an x64 PE: '+str(path))
    if struct.unpack_from('<H',data,optional+70)[0]&0x4160!=0x4160:
        raise ValueError('Missing ASLR/NX/CFG: '+str(path))
    count=struct.unpack_from('<H',data,pe+6)[0]
    table=optional+struct.unpack_from('<H',data,pe+20)[0]
    sections=[]
    for index in range(count):
        at=table+index*40
        _,address,size,offset=struct.unpack_from('<IIII',data,at+8)
        flags=struct.unpack_from('<I',data,at+36)[0]
        if flags&0xA0000000==0xA0000000:raise ValueError('RWX section: '+str(path))
        sections.append((address,size,offset))
    def file_offset(rva):
        for address,size,offset in sections:
            if address<=rva<address+size:return offset+rva-address
        raise ValueError('Unmapped load configuration: '+str(path))
    load=file_offset(struct.unpack_from('<I',data,optional+112+10*8)[0])
    # IMAGE_LOAD_CONFIG_DIRECTORY64: cookie, CFG check/dispatch/table/count.
    fields=[struct.unpack_from('<Q',data,load+offset)[0] for offset in [88,112,120,128,136]]
    flags=struct.unpack_from('<I',data,load+144)[0]
    if not all(fields) or flags&0x500!=0x500:raise ValueError('Incomplete GS/CFG metadata: '+str(path))


def toolchain():
    pf86 = Path(os.environ['ProgramFiles(x86)'])
    vs = Path(subprocess.check_output([
        str(pf86 / 'Microsoft Visual Studio/Installer/vswhere.exe'), '-latest',
        '-products', '*', '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
        '-property', 'installationPath'], text=True).strip())
    vc = sorted((vs / 'VC/Tools/MSVC').iterdir(), key=lambda p: tuple(map(int, p.name.split('.'))))[-1]
    sdk = pf86 / 'Windows Kits/10'
    versions = [p for p in (sdk / 'Lib').iterdir() if (p / 'um/x64/kernel32.lib').exists()]
    version = sorted(versions, key=lambda p: tuple(map(int, p.name.split('.'))))[-1].name
    llvm = Path(os.environ.get('LLVM_BIN', r'C:\Program Files\LLVM\bin'))
    return vc, sdk, version, llvm


def run(command, log, cwd):
    result = subprocess.run([str(x) for x in command], cwd=cwd, capture_output=True, text=True, errors='replace')
    with log.open('a', encoding='utf-8') as out:
        out.write(subprocess.list2cmdline([str(x) for x in command]) + '\n')
        out.write(result.stdout + result.stderr + '\n')
    if result.returncode:
        raise RuntimeError(f'{command[0]} failed ({result.returncode}):\n{result.stdout}{result.stderr}')
    return result.stdout


def build(spec, out_root, tc):
    vc, sdk, sdk_version, llvm = tc
    bin_dir = vc / 'bin/Hostx64/x64'
    root = REPO
    out = out_root / spec['id']
    out.mkdir(parents=True, exist_ok=True)
    log = out / 'build.log'
    log.write_text('', encoding='utf-8')
    compiler = bin_dir / 'cl.exe' if spec['compiler'] == 'MSVC' else llvm / 'clang-cl.exe'
    includes = [vc / 'include'] + [sdk / 'Include' / sdk_version / sub for sub in ['ucrt','shared','um']]
    includes += [root / 'src'] + [root / p for p in spec.get('includes', [])]
    common = ['/nologo','/c','/O2','/Ob0','/MT','/GS','/guard:cf','/GR-','/EHsc','/utf-8','/W4','/Brepro','/DDS2_STANDARD_CRT=1']
    common += ['/I' + str(p) for p in includes] + ['/D' + d for d in spec.get('defines', [])]
    if spec['compiler'] == 'LLVM':
        common += ['/clang:-mcx16', '/clang:-Wno-unused-function']
    objects = []
    for index, relative in enumerate(spec['sources']):
        source = root / relative
        obj = out / (str(index) + '-' + source.stem + '.obj')
        if source.suffix == '.s':
            command = [llvm / 'clang-cl.exe','--target=x86_64-pc-windows-msvc','/nologo','/c', '/Fo' + str(obj), source]
        elif source.suffix == '.asm':
            command = [bin_dir / 'ml64.exe','/nologo','/c','/Fo' + str(obj),source]
        else:
            command = [compiler] + common + (['/TC'] if source.suffix == '.c' else ['/std:c++17']) + ['/Fo' + str(obj),source]
        run(command, log, root)
        objects.append(obj)
    numeric = spec['version'].replace('.', ',') + ',0'
    resource_source = out / 'version.rc'
    strings = {'CompanyName':'gutzufuss1477', 'FileDescription':spec['description'],
               'FileVersion':spec['version'], 'ProductVersion':spec['version'],
               'InternalName':Path(spec['file']).stem,'OriginalFilename':spec['file'], 'ProductName':'DS2 Mods'}
    values = '\n'.join('VALUE "' + k + '", "' + v.replace('"','') + '\\0"' for k,v in strings.items())
    resource_source.write_text(f'''1 VERSIONINFO
FILEVERSION {numeric}
PRODUCTVERSION {numeric}
FILEFLAGSMASK 0x3fL
FILEFLAGS 0
FILEOS 0x40004L
FILETYPE 0x2L
FILESUBTYPE 0
BEGIN
 BLOCK "StringFileInfo"
 BEGIN
  BLOCK "040904b0"
  BEGIN
   {values}
  END
 END
 BLOCK "VarFileInfo"
 BEGIN
  VALUE "Translation", 0x409, 1200
 END
END
''', encoding='utf-8')
    resource = out / 'version.res'
    run([sdk / 'bin' / sdk_version / 'x64/rc.exe','/nologo','/fo' + str(resource),resource_source],log,root)
    binary = out / spec['file']
    kind = ['/subsystem:console'] if spec.get('executable') else ['/dll','/subsystem:windows']
    link = [bin_dir / 'link.exe','/nologo'] + kind + ['/machine:x64','/Brepro',
            '/incremental:no','/opt:ref','/opt:icf','/dynamicbase','/highentropyva','/nxcompat','/guard:cf',
            '/libpath:' + str(vc / 'lib/x64'),'/libpath:' + str(sdk / 'Lib' / sdk_version / 'ucrt/x64'),
            '/libpath:' + str(sdk / 'Lib' / sdk_version / 'um/x64'),'/out:' + str(binary),
            '/map:' + str(out / 'link.map')]
    run(link + objects + [resource] + spec['libraries'],log,root)
    verify_pe(binary)
    imports = run([bin_dir / 'dumpbin.exe','/headers','/loadconfig','/imports',binary],log,root)
    (out / 'pe-report.txt').write_text(imports,encoding='utf-8')
    link_map = (out / 'link.map').read_text(errors='replace')
    if spec['id'] == 'apas-memory-costs' or spec.get('cfgEntryPoints'):
        guard_table = imports.split('Guard CF Function Table', 1)[1].split('\n\n', 1)[1].split('\n\n', 1)[0]
        for name in spec.get('cfgEntryPoints', ['APASConstructorOriginal', 'APASRingOriginal']):
            match = re.search(r'\b' + name + r'\s+([0-9A-Fa-f]{16})\s+f\b', link_map)
            if not match or match[1].upper() not in guard_table.upper():
                raise RuntimeError(f'APAS bridge missing from CFG targets: {name}')
            if int(match[1], 16) % 16:
                raise RuntimeError(f'APAS bridge is not 16-byte aligned: {name}')
        # Test DLLs allocate isolated executable ABI fixtures; shipped ASIs must not.
        if binary.suffix == '.asi' and re.search(r'\bVirtual(?:Alloc|Free)(?:Ex)?\b', imports):
            raise RuntimeError('APAS production binary still imports executable-allocation APIs')
    for required in ['mainCRTStartup' if spec.get('executable') else '_DllMainCRTStartup','__security_init_cookie','__security_check_cookie']:
        if required not in link_map:
            raise RuntimeError(f'{spec["id"]}: missing runtime symbol {required}')
    sources = {str(p.relative_to(REPO)).replace('\\','/'):sha(p)
               for folder in ['src','vendor'] if (root / folder).exists()
               for p in sorted((root / folder).rglob('*')) if p.is_file() and p.suffix.lower() in ['.cpp','.c','.h','.hpp','.inl','.s','.asm']}
    result = {'id':spec['id'],'version':spec['version'],'file':str(binary),
              'sha256':sha(binary),'bytes':binary.stat().st_size,'compiler':str(compiler),
              'msvc':vc.name,'windowsSdk':sdk_version,'sourceHashes':sources}
    (out / 'manifest.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(f'BUILT {spec["id"]} {result["bytes"]} bytes {result["sha256"]}',flush=True)
    return result
