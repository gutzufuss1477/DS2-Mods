"""Build private overlay files. Game archives are input-only, always opened rb.

This prepares files, but neither installs hooks nor starts the game. Only a
complete, checksum-verified generation is published. A later runtime must
activate the whole generation, not individual files from the staging folder.
"""
from pathlib import Path, PurePosixPath
import hashlib
import json
import os
import shutil
import stat
import tempfile

ALLOWED = {
    'LocalCacheWinGame/package/package.00.00.core',
    'LocalCacheWinGame/package/package.42.02.core.stream',
    'LocalCacheWinGame/package/streaming_graph.core',
    'LocalCacheWinGame/package/streaming_links.stream',
}


class Refused(ValueError):
    pass


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for data in iter(lambda: stream.read(4 << 20), b''):
            h.update(data)
    return h.hexdigest()


def require(ok, message):
    if not ok:
        raise Refused(message)


def plain_path(path):
    """Reject existing junction/symlink components, including Windows reparse points."""
    p = Path(os.path.abspath(path))
    for item in [*reversed(p.parents), p]:
        if item.exists() or item.is_symlink():
            s = item.lstat()
            require(not stat.S_ISLNK(s.st_mode) and not (getattr(s, 'st_file_attributes', 0) & 0x400),
                    'Symlink/junction/reparse path refused: ' + str(item))
    return p.resolve()


def relative_file(root, name):
    require(isinstance(name, str) and '\\' not in name and ':' not in name, 'Invalid relative path')
    parts = PurePosixPath(name)
    require(not parts.is_absolute() and name not in ('', '.') and all(x not in ('..', '.') for x in parts.parts),
            'Path escape refused')
    p = plain_path(root.joinpath(*parts.parts))
    require(p.is_relative_to(root) and p != root and p.is_file(), 'Missing file or path escape: ' + name)
    return p


def expected_hash(value):
    require(isinstance(value, str) and len(value) == 64 and all(c in '0123456789abcdef' for c in value), 'Invalid SHA-256')
    return value


def validate_manifest(game, bundle, manifest):
    require(manifest.get('schemaVersion') == 1, 'Unsupported overlay format')
    routes = manifest.get('files')
    require(isinstance(routes, list) and 0 < len(routes) <= len(ALLOWED), 'Invalid file list')
    require(len({x['gamePath'] for x in routes}) == len(routes), 'Duplicate route')
    inputs = {}
    plans = []
    for row in routes:
        name = row['gamePath']
        require(name in ALLOWED, 'Game path is not an allowed visual resource')
        src = relative_file(game, name)
        want = expected_hash(row['originalSha256'])
        require(digest(src) == want, 'Unsupported or modified original: ' + name)
        inputs[src] = want
        expected_hash(row['resultSha256'])
        require(type(row['resultSize']) is int and 0 < row['resultSize'] <= 4 << 30, 'Invalid result size')
        require(isinstance(row['segments'], list) and len(row['segments']) > 0, 'Empty segment list')
        segments = []
        total = 0
        for piece in row['segments']:
            require(set(piece) in ({'original', 'length'}, {'payload', 'sha256', 'offset', 'length'}), 'Unknown segment')
            if 'original' in piece:
                file, offset = src, piece['original']
            else:
                file, offset = relative_file(bundle, piece['payload']), piece['offset']
                want = expected_hash(piece['sha256'])
                require(digest(file) == want, 'Payload checksum mismatch')
                require(file not in inputs or inputs[file] == want, 'Inconsistent input identity')
                inputs[file] = want
            length = piece['length']
            require(type(offset) is int and type(length) is int and offset >= 0 and length > 0, 'Invalid range')
            require(offset + length <= file.stat().st_size, 'Range exceeds source')
            total += length
            segments.append((file, offset, length))
        require(total == row['resultSize'], 'Result length mismatch')
        plans.append((row, segments))
    return plans, inputs


def verify_generation(folder, manifest):
    folder = plain_path(folder)
    require(folder.is_dir(), 'Missing generation')
    marker = relative_file(folder, 'ready.json')
    require(json.loads(marker.read_text(encoding='utf-8')) == manifest, 'Existing cache manifest differs')
    for row in manifest['files']:
        p = relative_file(folder, Path(row['gamePath']).name)
        require(p.stat().st_size == row['resultSize'] and digest(p) == row['resultSha256'], 'Cache checksum mismatch')
    return folder


def build_cache(game_root, bundle_root, manifest, cache_root):
    game, bundle, cache = map(plain_path, (game_root, bundle_root, cache_root))
    require(game.is_dir() and bundle.is_dir(), 'Missing input root')
    # Cache must be outside both input trees, including their parent directories.
    for root in (game, bundle):
        require(not cache.is_relative_to(root) and not root.is_relative_to(cache), 'Cache overlaps an input tree')
    plans, inputs = validate_manifest(game, bundle, manifest)
    canonical = json.dumps(manifest, sort_keys=True, separators=(',', ':')).encode('utf-8')
    generation = hashlib.sha256(canonical).hexdigest()
    target = cache / generation
    if target.exists():
        return verify_generation(target, manifest)
    cache.mkdir(parents=True, exist_ok=True)
    plain_path(cache)
    staging = Path(tempfile.mkdtemp(prefix='.building-', dir=cache))
    try:
        for row, segments in plans:
            output = staging / Path(row['gamePath']).name
            with output.open('xb') as out:
                for source, offset, length in segments:
                    with source.open('rb') as src:
                        src.seek(offset)
                        while length:
                            data = src.read(min(length, 4 << 20))
                            require(bool(data), 'Input truncated during build')
                            out.write(data)
                            length -= len(data)
                out.flush()
                os.fsync(out.fileno())
            require(output.stat().st_size == row['resultSize'] and digest(output) == row['resultSha256'], 'Generated checksum mismatch')
        require(all(digest(p) == h for p, h in inputs.items()), 'Inputs changed while generating cache')
        with (staging / 'ready.json').open('x', encoding='utf-8') as out:
            json.dump(manifest, out, indent=2)
            out.flush()
            os.fsync(out.fileno())
        verify_generation(staging, manifest)
        require(not target.exists(), 'Concurrent generation detected; retry to validate it')
        # Atomic publication on the same filesystem. No original path is a destination.
        staging.rename(target)
        return target
    finally:
        # Only our freshly created staging directory may be removed on failure.
        if staging.exists():
            resolved = plain_path(staging)
            require(resolved.parent == cache and resolved.name.startswith('.building-'), 'Invalid cleanup path')
            shutil.rmtree(resolved)
