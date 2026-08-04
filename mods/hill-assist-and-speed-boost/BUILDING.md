# Building DS2 Vehicle Physics Core

## Requirements

- Windows x64
- Visual Studio 2022 Build Tools or Visual Studio 2022
- Desktop development with C++
- Windows SDK
- C++17 support

For the original release-equivalent command line, install the LLVM tools for
Visual Studio so that `clang-cl.exe` and `lld-link.exe` are available.

## Release-equivalent LLVM build

Run from a developer command prompt:

`scripts\build-llvm.cmd`

Equivalent original commands:

`clang-cl /c /O2 /GS- /GR- /EHs-c- /Zl /std:c++17 src\DS2VehicleCore.cpp`

`lld-link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows build\DS2VehicleCore.obj kernel32.lib /out:build\DS2VehicleCore.asi`

## MSVC fallback build

Open an **x64 Native Tools Command Prompt for Visual Studio 2022** and run:

`scripts\build-msvc.cmd`

The runtime and exception model remain disabled in the same manner as the
original build. A different compiler or linker can produce a functionally
equivalent file with a different SHA-256 hash.

## Output

`build\DS2VehicleCore.asi`
