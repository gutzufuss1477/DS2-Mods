"""Read-only original/private object array census using isolated Odradek classes."""
import json
import os
from pathlib import Path
import subprocess
from prepare_resource_audit import ROOT, JAVA, MODULES


def main():
    build = ROOT/'build/array-audit'
    build.mkdir()
    original = ROOT/'build/resource-audit/src/sh/adelessfox/odradek/game/ds2/storage/StreamingObjectReader.java'
    source = original.read_text()
    source = source.replace('private int labLocatorOrdinal=0;',
                            'private int labLocatorOrdinal=0;\n    private StringBuilder labArrays;')
    source = source.replace('var labRows = new StringBuilder();',
                            'var labRows = new StringBuilder();\n        labArrays = labEnabled ? new StringBuilder() : null;')
    needle = 'java.nio.file.Files.write(dir.resolve(group.id()+".bin"),labBytes.toByteArray(),'
    assert source.count(needle) == 1
    source = source.replace(needle, '''java.nio.file.Files.writeString(dir.resolve(group.id()+".arrays.tsv"),labArrays,
                java.nio.file.StandardOpenOption.CREATE_NEW);
            '''+needle)
    needle = '    @Override\n    protected Object readPointer'
    assert source.count(needle) == 1
    source = source.replace(needle, '''    @Override
    protected Object readContainer(sh.adelessfox.odradek.rtti.ContainerTypeInfo info,
                                    BinaryReader reader, TypeFactory factory) throws IOException {
        long at=reader.position();
        int count=reader.readInt();
        reader.position(at);
        if(labArrays!=null && count>0) {
            labArrays.append(atlasObjectId).append("\\t").append(info.itemType().name())
                .append("\\t").append(count).append("\\t").append(info.containerType()).append("\\n");
        }
        return super.readContainer(info,reader,factory);
    }

'''+needle)
    target=build/'src/sh/adelessfox/odradek/game/ds2/storage/StreamingObjectReader.java'
    target.parent.mkdir(parents=True)
    target.write_text(source)
    classes=build/'classes'
    command=[str(JAVA/'javac.exe'),'--enable-preview','--release','25','-proc:none',
             '--class-path',str(MODULES/'*'),'-d',str(classes),str(target)]
    compiled=subprocess.run(command,capture_output=True)
    (build/'compile.log').write_bytes(compiled.stdout+compiled.stderr)
    assert compiled.returncode==0,compiled.stderr.decode()
    cache=Path(json.loads((ROOT/'staging/test07-resident-readonly-textures/report.json').read_text())['cache'])
    for label,override in [('original',False),('test07',True)]:
        output=ROOT/f'evidence/test07-arrays-{label}'
        output.mkdir()
        env={k:v for k,v in os.environ.items() if not k.startswith('DS2_')}
        env['DS2_LAB_AUDIT_DIR']=str(output)
        env['DS2_LAB_AUDIT_GROUPS']='67696,31126,39987'
        if override:
            env.update(DS2_KESTREL_GRAPH_OVERRIDE=str(cache/'streaming_graph.core'),
                       DS2_ATLAS_LINKTABLE_OVERRIDE=str(cache/'streaming_links.stream'),
                       DS2_KESTREL_EXT_STREAM_OVERRIDE=str(cache/'package.42.02.core.stream'))
        command=[str(JAVA/'java.exe'),'--enable-preview','--enable-native-access=odradek.core',
                 '--add-opens=java.base/sun.nio.ch=odradek.core',
                 '--patch-module','odradek.game.ds2='+str(classes),'--module-path',str(MODULES),
                 '-m','odradek.app/sh.adelessfox.odradek.app.Main','export','-s',
                 r'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH',
                 '-o',str(output),'-f','json','67696:0','31126:0','39987:0']
        with (output/'export.log').open('xb') as log:
            result=subprocess.run(command,env=env,stdout=log,stderr=subprocess.STDOUT)
        assert result.returncode==0,(output/'export.log').read_text()[-3000:]
        assert all((output/f'{g}.arrays.tsv').exists() for g in [67696,31126,39987])
        print('Recorded',label,'arrays')


if __name__=='__main__':
    main()
