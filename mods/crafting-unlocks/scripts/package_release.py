#!/usr/bin/env python3
"""Package only the player files; require a matching PE verification report."""
from __future__ import annotations
import argparse
import hashlib
import json
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERSION = "1.0.0"
FILES = ("ds2_crafting_unlocks.asi", "ds2_crafting_unlocks.ini", "README_EN.txt",
         "README_DE.txt", "CHANGELOG.txt")

def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

def package(destination: Path) -> Path:
    release = ROOT / "release"
    for name in FILES:
        if not (release / name).is_file():
            raise FileNotFoundError(f"Missing release file: {name}")
    report = json.loads((ROOT / "docs/BUILD_VALIDATION.json").read_text())
    if (report.get("status") != "PASS_STRUCTURAL_ONLY" or
            report.get("version") != VERSION or
            report.get("sha256") != sha(release / FILES[0])):
        raise ValueError("Binary does not match the release PE validation; rerun validation.")
    checksums = "".join(f"{sha(release / name)}  {name}\n" for name in FILES)
    (release / "SHA256SUMS.txt").write_text(checksums, encoding="ascii")
    destination.mkdir(parents=True, exist_ok=True)
    archive = destination / f"DS2_Crafting_Unlocks_v{VERSION}.zip"
    with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as handle:
        for name in (*FILES, "SHA256SUMS.txt"):
            handle.write(release / name, name)
    with zipfile.ZipFile(archive) as handle:
        if handle.testzip() is not None or set(handle.namelist()) != set((*FILES, "SHA256SUMS.txt")):
            raise ValueError("Packaged archive failed integrity/member verification")
    return archive

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--destination", type=Path, default=ROOT / "dist")
    args = parser.parse_args()
    try:
        archive = package(args.destination)
    except (OSError, ValueError, KeyError, zipfile.BadZipFile) as error:
        print(f"Package failed: {error}", file=sys.stderr)
        return 1
    print(f"Created {archive}")
    print(f"SHA-256 {sha(archive)}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
