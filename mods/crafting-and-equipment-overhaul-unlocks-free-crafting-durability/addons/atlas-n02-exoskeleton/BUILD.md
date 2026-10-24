# Building ATLAS N02 Exoskeleton 1.0.0

Use Windows x64, LLVM clang-cl/lld-link, Python 3 and Windows SDK headers. Run
`python tools/check_local_inputs.py`, then `./build.ps1`.
The script compiles the addon, both menu texture upload paths and embedded payload,
using its own import library. No other mod's precompiled object is required.

Three game-derived local inputs are deliberately omitted from Git. Their paths,
sizes and SHA-256 hashes are listed in `config/local-inputs.json`: the generated
model/graph payload translation unit and the two menu pixel includes. Provision
those exact locally derived inputs before building. A clean clone is not a complete
native asset extractor. It does not include original game meshes/textures or renders.

The release keeps the user-tested binary unchanged. Rebuilding these sources with
the recorded local inputs produced an identical binary except for the PE build
timestamp. Full sources and local input hashes distinguish this from the abandoned
earlier skeleton prototypes. No game process is used by the build.

Normal main-mod content changes must preserve the ABI, router capability and ATLAS
IDs; `source/tools/enforce_visual_router_v1.py` in the main mod enforces the frozen
integration source. The addon negotiates capabilities, not a main-mod version hash.
Game-layout changes remain separately guarded by the exact supported DS2 executable.
