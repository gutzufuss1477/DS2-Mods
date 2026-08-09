# Building PickupCargoCapacity.asi

## Supported build environment

- Windows 10 or Windows 11, x64
- Visual Studio 2022 Build Tools or Visual Studio 2022
- Workload: **Desktop development with C++**
- Component: a Windows 10/11 SDK

The recommended v1.0.1 release is compiled as a conventional native x64 DLL/ASI with the static C/C++ runtime, normal DLL startup, stack protection and Control Flow Guard. It includes Windows product/version metadata and enables ASLR, DEP and high-entropy ASLR.

## Method 1: MSVC

1. Open **x64 Native Tools Command Prompt for VS 2022**.
2. Change to the repository root.
3. Run:

```bat
scripts\build-msvc.cmd
```

Output:

```text
build\PickupCargoCapacity.asi
```

## Method 2: clang-cl/lld-link — self-contained fallback

Install LLVM for Windows with `clang-cl`, `lld-link`, `llvm-rc` and `llvm-dlltool`. This fallback generates its own minimal `kernel32.lib` and does not require a Windows SDK library directory.

From a command prompt where the LLVM tools are on `PATH`, run:

```bat
scripts\build-llvm.cmd
```

Output:

```text
build\PickupCargoCapacity.asi
```

The fallback intentionally keeps the minimal no-CRT startup for environments without the MSVC runtime libraries. It still includes version metadata and enables ASLR, DEP and high-entropy ASLR. The recommended public release build is the MSVC method above.

The legacy equivalence tool is retained for auditing version 1.0.0. It must be run against a v1.0.0 checkout and is not expected to match the hardened v1.0.1 binary:

```bat
py tools\verify_pe_equivalence.py build\PickupCargoCapacity.asi reference\PickupCargoCapacity_v1.0.0.asi
```

## Security verification

After building, verify the signature status and metadata with PowerShell:

```powershell
Get-Item build\PickupCargoCapacity.asi | Select-Object -ExpandProperty VersionInfo
Get-FileHash build\PickupCargoCapacity.asi -Algorithm SHA256
```

The binary remains unsigned unless you apply a trusted Authenticode code-signing certificate after the build.
