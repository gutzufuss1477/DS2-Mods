# Build instructions

## Requirements

- Windows x64
- PowerShell
- LLVM for Windows containing `clang-cl.exe` and `lld-link.exe`

The build uses the checked-in minimal Win32 declarations and a small `kernel32.def`, so it does not depend on project-specific SDK libraries.

## Build

From the mod directory:

```powershell
.\scripts\build-llvm.ps1
```

If LLVM is not installed in `C:\Program Files\LLVM\bin`, set `LLVM_HOME` to the directory containing `clang-cl.exe` and `lld-link.exe`.

## Output

`build\public\ds2_no_magellan_evaluation.asi`

The script also copies the release INI and writes the ASI SHA-256 hash.

The linker uses `/Brepro` for reproducible PE output with the same toolchain and inputs.
