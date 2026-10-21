"""Build an isolated read-only Odradek instrumentation class; shared tool untouched."""
from pathlib import Path
import hashlib
import json
import subprocess

ROOT = Path(__file__).resolve().parents[1]
SOURCE = Path(r'D:\Odradek-DS2\odradek-game-ds2\src\main\java\sh\adelessfox\odradek\game\ds2\storage\StreamingObjectReader.java')
JAVA = Path(r'C:\Program Files\Eclipse Adoptium\jdk-25.0.4.101-hotspot\bin')
MODULES = Path(r'D:\Odradek-DS2\odradek-app\target\modules')


def build():
    output = ROOT / 'build/resource-audit'
    if (output / 'provenance.json').exists():
        raise ValueError('Completed reader already exists')
    output.mkdir(parents=True, exist_ok=True)
    original = SOURCE.read_bytes()
    text = original.decode().replace('\r\n', '\n')
    replacements = [
        ('private int atlasObjectId=-1;', 'private int atlasObjectId=-1;\n    private int labLocatorOrdinal=0;'),
        ('atlasLinkOrdinal=0;atlasObjectId=-1;', '''atlasLinkOrdinal=0;atlasObjectId=-1;labLocatorOrdinal=0;
        String labDir = System.getenv("DS2_LAB_AUDIT_DIR");
        String labGroups = "," + System.getenv("DS2_LAB_AUDIT_GROUPS") + ",";
        boolean labEnabled = labDir != null && labGroups.contains("," + group.id() + ",");
        var labBytes = new java.io.ByteArrayOutputStream();
        var labRows = new StringBuilder();
'''),
        ('long objectStart = reader.position();', '''long objectStart = reader.position();
                int labLinkStart=atlasLinkOrdinal,labLocatorStart=labLocatorOrdinal;'''),
        ('fillCompound(object.getType(), reader, factory, object);', '''fillCompound(object.getType(), reader, factory, object);
                if (labEnabled) {
                    int len=Math.toIntExact(reader.position()-objectStart);
                    labRows.append(objectIndex).append("\\t").append(object.getType()).append("\\t")
                        .append(labBytes.size()).append("\\t").append(len).append("\\t")
                        .append(labLinkStart).append("\\t").append(atlasLinkOrdinal).append("\\t")
                        .append(labLocatorStart).append("\\t").append(labLocatorOrdinal).append("\\n");
                    labBytes.write(data,Math.toIntExact(objectStart),len);
                }'''),
        ('return result;\n    }\n\n    @Override\n    protected void fillCompound', '''if (labEnabled) {
            var dir=java.nio.file.Path.of(labDir);
            java.nio.file.Files.createDirectories(dir);
            java.nio.file.Files.write(dir.resolve(group.id()+".bin"),labBytes.toByteArray(),
                java.nio.file.StandardOpenOption.CREATE_NEW);
            java.nio.file.Files.writeString(dir.resolve(group.id()+".tsv"),labRows,
                java.nio.file.StandardOpenOption.CREATE_NEW);
        }
        return result;
    }

    @Override
    protected void fillCompound'''),
        ('var locator = streamingLocators.next();', 'var locator = streamingLocators.next();\n            labLocatorOrdinal++;'),
    ]
    for old, new in replacements:
        if text.count(old) != 1:
            raise ValueError('Unexpected reader source: ' + old[:80])
        text = text.replace(old, new)
    patched = output / 'src/sh/adelessfox/odradek/game/ds2/storage/StreamingObjectReader.java'
    patched.parent.mkdir(parents=True, exist_ok=True)
    patched.write_text(text, encoding='utf-8')
    classes = output / 'classes'
    command = [str(JAVA / 'javac.exe'), '--enable-preview', '--release', '25', '-proc:none',
               '--class-path', str(MODULES / '*'),
               '-d', str(classes), str(patched)]
    result = subprocess.run(command, capture_output=True)
    (output / 'compile.log').write_bytes(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr.decode(errors='replace'))
    assert SOURCE.read_bytes() == original
    (output / 'provenance.json').write_text(json.dumps({
        'source': str(SOURCE), 'sourceSha256': hashlib.sha256(original).hexdigest(),
        'isolatedSourceSha256': hashlib.sha256(patched.read_bytes()).hexdigest(),
        'sharedExporterModified': False, 'purpose': 'Read-only object bytes, link and locator ranges',
        'command': command}, indent=2), encoding='utf-8')
    print('Isolated resource reader compiled:', classes)


if __name__ == '__main__':
    build()
