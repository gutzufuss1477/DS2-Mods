# Building PickupCargoCapacity.asi

## Supported build environment

- Windows 10 or Windows 11, x64
- Visual Studio 2022 Build Tools or Visual Studio 2022
- Workload: **Desktop development with C++**
- Component: a Windows 10/11 SDK

The release was compiled as a small native x64 DLL/ASI without the C/C++ runtime library. It links only against `kernel32.lib` from the installed Windows SDK.

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

## Method 2: clang-cl/lld-link — release-equivalent toolchain

Install the optional Visual Studio component **C++ Clang tools for Windows** in addition to the C++ workload and Windows SDK.

From **x64 Native Tools Command Prompt for VS 2022**, run:

```bat
scripts\build-llvm.cmd
```

Output:

```text
build\PickupCargoCapacity.asi
```

The PE linker timestamp changes on every build. To compare a clang-cl/lld-link build with the v1.0.0 reference binary while ignoring only that four-byte timestamp, run:

```bat
py tools\verify_pe_equivalence.py build\PickupCargoCapacity.asi reference\PickupCargoCapacity_v1.0.0.asi
```

Expected result:

```text
MATCH: files are byte-identical after zeroing only the PE COFF timestamp.
Normalized SHA-256: ed9de1451f3bb0845c3933ca50e12b87403b0c62050631f4bbd94c2dc3b83b69
```

## Manual compiler commands

The scripts execute these essential steps:

```bat
clang-cl /nologo /c /O2 /GS- /Gs999999 /GR- /EHs-c- /Zl /Oi /std:c++17 /DWIN32 /D_WINDOWS /D_USRDLL /Fo:"build\pickup_cargo_capacity_patch.obj" "src\pickup_cargo_capacity_patch.cpp"

lld-link /dll /entry:DllMain /nodefaultlib /machine:x64 /out:"build\PickupCargoCapacity.asi" "build\pickup_cargo_capacity_patch.obj" kernel32.lib
```

No downloaded libraries, package manager, generated source files or closed-source SDKs are required beyond the Microsoft Windows SDK import library.
