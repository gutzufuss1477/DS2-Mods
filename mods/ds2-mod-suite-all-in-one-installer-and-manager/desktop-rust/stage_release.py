"""Stage the offline Suite from the previous release and verified standalone files."""
import argparse
import hashlib
import json
import shutil
import zipfile
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", type=Path, required=True,
                        help="Suite 1.14 input folder, or extracted DS2-Mod-Suite folder")
    parser.add_argument("--container-asi", type=Path, required=True)
    parser.add_argument("--boot-archive", type=Path, required=True)
    parser.add_argument("--remote-archive", type=Path, required=True)
    parser.add_argument("--lost-cargo-archive", type=Path, required=True)
    parser.add_argument("--bandwidth-archive", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    archives = {
        "crafting-boot-selector": args.boot_archive,
        "remote-orders-scaled": args.remote_archive,
        "lost-cargo-likes-booster": args.lost_cargo_archive,
        "chiral-bandwidth-costs": args.bandwidth_archive,
    }
    catalog = json.loads(Path(__file__).with_name("catalog.json").read_text("utf-8"))
    args.out.mkdir(parents=True, exist_ok=False)
    specs = [(None, catalog["loader"])]
    specs.extend((mod["id"], f) for mod in catalog["mods"] for f in mod["files"])
    for mod, f in specs:
        if mod in archives:
            with zipfile.ZipFile(archives[mod]) as archive:
                names = [n for n in archive.namelist() if Path(n).name == f["target"]]
                if len(names) != 1:
                    raise ValueError(f"Missing or ambiguous archive target: {f['target']}")
                raw = archive.read(names[0])
        elif mod == "all-containers-small":
            raw = args.container_asi.read_bytes()
        else:
            raw = (args.baseline / f["payload"]).read_bytes()
        if hashlib.sha256(raw).hexdigest().lower() != f["sha256"].lower():
            raise ValueError(f"Hash mismatch: {f['payload']}")
        dest = args.out / f["payload"]
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(raw)
    licenses = args.baseline / "licenses" / "native"
    for f in (licenses if licenses.is_dir() else args.baseline).iterdir():
        if f.is_file() and f.name.startswith(("LICENSE", "THIRD_PARTY")):
            shutil.copyfile(f, args.out / f.name)
    print(f"Staged {len(specs)} verified payload entries.")


if __name__ == "__main__":
    main()
