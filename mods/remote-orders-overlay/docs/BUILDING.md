# Building and Reproducing the Release

## Exact runtime-tested release

Requirements:

- Windows or Linux
- Python 3.9 or newer

Run on Windows:

```powershell
.\scripts\build_release.ps1
```

or:

```cmd
scripts\build_release.cmd
```

The script transforms the included validated baseline ASI into the exact no-log release. It refuses to continue unless:

- the baseline SHA-256 is correct;
- every expected log path occurs exactly once;
- `.text`, `.data` and `.pdata` remain unchanged;
- the final SHA-256 is exactly `696D05285C23F2D73EE27C23827A27BA347C76D79DF0A84F7DB29E164DCB695C`.

## Source reference build

Requirements:

- Windows 10 or Windows 11 x64
- LLVM for Windows with `clang-cl.exe` and `lld-link.exe`
- Windows 10/11 SDK x64 libraries
- PowerShell 5.1 or newer

Run:

```powershell
.\scripts\build_source_reference.ps1
```

This compiles the recovered source for review. It is deliberately named `REFERENCE` and is not substituted for the runtime-tested public release. The exact original historical toolchain invocation was not preserved, so a source-only build is not claimed to reproduce the public binary byte-for-byte.
