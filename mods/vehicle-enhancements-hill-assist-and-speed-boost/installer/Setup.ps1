#requires -Version 5.1
[CmdletBinding()]
param(
    [ValidateSet("Install", "Uninstall")]
    [string]$Action = "Install",

    [ValidateSet("Menu", "Speed", "Hill", "Both")]
    [string]$Selection = "Menu",

    [string]$GamePath
)

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

$ExpectedGameVersion = "1.10.89.0"
$ExpectedGameHash = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
$ExpectedCoreHash = "78F4D90B6473F9163E65A227AE6444D516133ABAC34C9E6575BD2B13C3CB60B6"

function Write-Stage {
    param([string]$Text)
    Write-Host ("[DS2 Vehicle Mods] " + $Text)
}

function Find-GamePath {
    $libraries = New-Object System.Collections.Generic.List[string]
    $steamPath = $null

    foreach ($key in @(
        "HKCU:\Software\Valve\Steam",
        "HKLM:\SOFTWARE\WOW6432Node\Valve\Steam",
        "HKLM:\SOFTWARE\Valve\Steam"
    )) {
        try {
            $properties = Get-ItemProperty -Path $key -ErrorAction Stop

            if ($properties.SteamPath) {
                $steamPath = [string]$properties.SteamPath
            }
            elseif ($properties.InstallPath) {
                $steamPath = [string]$properties.InstallPath
            }

            if ($steamPath) {
                break
            }
        }
        catch {}
    }

    if ($steamPath -and (Test-Path -LiteralPath $steamPath)) {
        $libraries.Add($steamPath)
        $vdf = Join-Path $steamPath "steamapps\libraryfolders.vdf"

        if (Test-Path -LiteralPath $vdf) {
            $raw = Get-Content -LiteralPath $vdf -Raw

            foreach (
                $match in
                [regex]::Matches($raw, '"path"\s+"([^"]+)"')
            ) {
                $library = $match.Groups[1].Value -replace '\\\\', '\'

                if (
                    $library -and
                    -not $libraries.Contains($library)
                ) {
                    $libraries.Add($library)
                }
            }
        }
    }

    foreach ($library in $libraries) {
        foreach ($folderName in @(
            "DEATH STRANDING 2 - ON THE BEACH",
            "Death Stranding 2 - On the Beach",
            "DEATH STRANDING 2"
        )) {
            $candidate = Join-Path $library (
                "steamapps\common\" + $folderName
            )

            if (
                Test-Path -LiteralPath (
                    Join-Path $candidate "DS2.exe"
                )
            ) {
                return $candidate
            }
        }
    }

    return $null
}

function Select-GameFolder {
    try {
        $shell = New-Object -ComObject Shell.Application

        $folder = $shell.BrowseForFolder(
            0,
            "Select the folder containing DS2.exe.",
            0,
            0
        )

        if ($folder) {
            return $folder.Self.Path
        }
    }
    catch {}

    return $null
}

function Resolve-GamePath {
    param([string]$RequestedPath)

    $resolved = $RequestedPath

    if (-not $resolved) {
        $resolved = Find-GamePath
    }

    if (
        -not $resolved -or
        -not (
            Test-Path -LiteralPath (
                Join-Path $resolved "DS2.exe"
            )
        )
    ) {
        $resolved = Select-GameFolder
    }

    if (
        -not $resolved -or
        -not (
            Test-Path -LiteralPath (
                Join-Path $resolved "DS2.exe"
            )
        )
    ) {
        throw "The game folder containing DS2.exe was not found."
    }

    return (Resolve-Path -LiteralPath $resolved).Path
}

function Backup-File {
    param(
        [string]$Path,
        [string]$BackupFolder
    )

    if (-not (Test-Path -LiteralPath $Path)) {
        return
    }

    New-Item -ItemType Directory -Path $BackupFolder -Force |
        Out-Null

    $stamp = Get-Date -Format "yyyyMMdd_HHmmss"
    $name = [IO.Path]::GetFileName($Path)

    Copy-Item `
        -LiteralPath $Path `
        -Destination (
            Join-Path $BackupFolder ($stamp + "_" + $name)
        ) `
        -Force
}

function Test-PeX64 {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        return $false
    }

    $stream = $null
    $reader = $null

    try {
        $stream = [IO.File]::Open(
            $Path,
            [IO.FileMode]::Open,
            [IO.FileAccess]::Read,
            [IO.FileShare]::ReadWrite
        )

        $reader = New-Object IO.BinaryReader($stream)

        if ($reader.ReadUInt16() -ne 0x5A4D) {
            return $false
        }

        $stream.Position = 0x3C
        $peOffset = $reader.ReadInt32()

        if (
            $peOffset -lt 0x40 -or
            $peOffset -gt ($stream.Length - 6)
        ) {
            return $false
        }

        $stream.Position = $peOffset

        if ($reader.ReadUInt32() -ne 0x00004550) {
            return $false
        }

        return $reader.ReadUInt16() -eq 0x8664
    }
    finally {
        if ($reader) {
            $reader.Dispose()
        }
        elseif ($stream) {
            $stream.Dispose()
        }
    }
}

function Get-ExistingAsiLoader {
    param([string]$ResolvedGamePath)

    foreach ($name in @(
        "dinput8.dll",
        "version.dll",
        "winmm.dll",
        "winhttp.dll"
    )) {
        $candidate = Join-Path $ResolvedGamePath $name

        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    return $null
}

function Get-UltimateAsiLoaderAsset {
    $apiUrl = (
        "https://api.github.com/repos/ThirteenAG/" +
        "Ultimate-ASI-Loader/releases/latest"
    )

    $headers = @{
        "User-Agent" = "DS2-Vehicle-Mods-Installer"
        "Accept" = "application/vnd.github+json"
        "X-GitHub-Api-Version" = "2022-11-28"
    }

    $release = Invoke-RestMethod `
        -Uri $apiUrl `
        -Headers $headers `
        -Method Get

    $assets = @(
        $release.assets |
        Where-Object {
            $_.name -match '(?i)\.zip$' -and
            $_.name -notmatch '(?i)(source|symbols|pdb)'
        }
    )

    $asset = $assets |
        Where-Object {
            $_.name -match '(?i)(x64|win64)'
        } |
        Sort-Object `
            @{ Expression = {
                if ($_.name -match '(?i)ultimate.*asi.*loader') {
                    0
                }
                else {
                    1
                }
            } }, `
            name |
        Select-Object -First 1

    if (-not $asset) {
        throw (
            "No Win64 ZIP asset was found in the latest official " +
            "Ultimate ASI Loader release."
        )
    }

    return $asset
}

function Install-UltimateAsiLoader {
    param(
        [string]$ResolvedGamePath,
        [string]$BackupFolder
    )

    $existing = Get-ExistingAsiLoader `
        -ResolvedGamePath $ResolvedGamePath

    if ($existing) {
        Write-Stage (
            "Existing ASI Loader detected: " +
            [IO.Path]::GetFileName($existing)
        )
        return
    }

    $tempRoot = Join-Path $env:TEMP (
        "DS2_UltimateASILoader_" +
        [Guid]::NewGuid().ToString("N")
    )

    $zipPath = Join-Path $tempRoot "Ultimate-ASI-Loader-x64.zip"
    $extractPath = Join-Path $tempRoot "Extracted"

    try {
        New-Item -ItemType Directory -Path $extractPath -Force |
            Out-Null

        Write-Stage (
            "No ASI Loader found. Downloading the latest official " +
            "Ultimate ASI Loader Win64 release."
        )

        [Net.ServicePointManager]::SecurityProtocol = (
            [Net.ServicePointManager]::SecurityProtocol -bor
            [Net.SecurityProtocolType]::Tls12
        )

        $asset = Get-UltimateAsiLoaderAsset

        Invoke-WebRequest `
            -Uri $asset.browser_download_url `
            -OutFile $zipPath `
            -UseBasicParsing `
            -Headers @{
                "User-Agent" = "DS2-Vehicle-Mods-Installer"
            }

        if (
            -not (Test-Path -LiteralPath $zipPath) -or
            (Get-Item -LiteralPath $zipPath).Length -lt 1024
        ) {
            throw "The ASI Loader download is empty or incomplete."
        }

        Expand-Archive `
            -LiteralPath $zipPath `
            -DestinationPath $extractPath `
            -Force

        $loader = Get-ChildItem `
            -LiteralPath $extractPath `
            -Filter "dinput8.dll" `
            -File `
            -Recurse |
            Where-Object {
                Test-PeX64 -Path $_.FullName
            } |
            Select-Object -First 1

        if (-not $loader) {
            throw (
                "A valid Win64 dinput8.dll was not found in the " +
                "official release archive."
            )
        }

        $target = Join-Path $ResolvedGamePath "dinput8.dll"

        if (Test-Path -LiteralPath $target) {
            Backup-File `
                -Path $target `
                -BackupFolder $BackupFolder
        }

        Copy-Item `
            -LiteralPath $loader.FullName `
            -Destination $target `
            -Force

        if (-not (Test-PeX64 -Path $target)) {
            throw "The installed dinput8.dll failed Win64 validation."
        }

        Write-Stage (
            "Ultimate ASI Loader installed as dinput8.dll."
        )
    }
    finally {
        if (Test-Path -LiteralPath $tempRoot) {
            Remove-Item `
                -LiteralPath $tempRoot `
                -Recurse `
                -Force `
                -ErrorAction SilentlyContinue
        }
    }
}

function Test-GameBuild {
    param([string]$ResolvedGamePath)

    $exe = Join-Path $ResolvedGamePath "DS2.exe"
    $version = (Get-Item -LiteralPath $exe).VersionInfo.FileVersion

    if ($version -ne $ExpectedGameVersion) {
        throw (
            "Unsupported game version: " +
            $version +
            ". Supported version: " +
            $ExpectedGameVersion +
            "."
        )
    }

    $hash = (
        Get-FileHash -LiteralPath $exe -Algorithm SHA256
    ).Hash

    if ($hash -ne $ExpectedGameHash) {
        throw "DS2.exe does not match the tested Steam build."
    }
}

function Resolve-Selection {
    param(
        [string]$RequestedSelection,
        [string]$RequestedAction
    )

    if ($RequestedSelection -ne "Menu") {
        return $RequestedSelection
    }

    Write-Host ""
    Write-Host "DEATH STRANDING 2 Vehicle Mods" `
        -ForegroundColor Cyan
    Write-Host ""

    if ($RequestedAction -eq "Install") {
        Write-Host "1 - Install Vehicle Speed Boost only"
        Write-Host "2 - Install Pickup Truck Hill Assist only"
        Write-Host "3 - Install both mods"
    }
    else {
        Write-Host "1 - Remove Vehicle Speed Boost only"
        Write-Host "2 - Remove Pickup Truck Hill Assist only"
        Write-Host "3 - Remove both mods"
    }

    Write-Host ""
    $choice = Read-Host "Select 1, 2 or 3"

    switch ($choice) {
        "1" { return "Speed" }
        "2" { return "Hill" }
        "3" { return "Both" }
        default { throw "Invalid selection." }
    }
}

function Install-SharedCore {
    param(
        [string]$ResolvedGamePath,
        [string]$PackagePath,
        [string]$BackupFolder
    )

    $sourceCore = Join-Path $PackagePath "DS2VehicleCore.asi"
    $targetCore = Join-Path $ResolvedGamePath "DS2VehicleCore.asi"

    $sourceHash = (
        Get-FileHash -LiteralPath $sourceCore -Algorithm SHA256
    ).Hash

    if ($sourceHash -ne $ExpectedCoreHash) {
        throw "DS2VehicleCore.asi failed package integrity validation."
    }

    if (Test-Path -LiteralPath $targetCore) {
        $installedHash = (
            Get-FileHash -LiteralPath $targetCore -Algorithm SHA256
        ).Hash

        if ($installedHash -eq $ExpectedCoreHash) {
            Write-Stage "Compatible Shared Core already installed."
            return
        }

        Backup-File `
            -Path $targetCore `
            -BackupFolder $BackupFolder

        Write-Stage "Different Shared Core backed up."
    }

    Copy-Item `
        -LiteralPath $sourceCore `
        -Destination $targetCore `
        -Force

    Write-Stage "Shared Core installed."
}

function Install-ModuleIni {
    param(
        [string]$ResolvedGamePath,
        [string]$PackagePath,
        [string]$IniName
    )

    $source = Join-Path $PackagePath $IniName
    $target = Join-Path $ResolvedGamePath $IniName

    if (Test-Path -LiteralPath $target) {
        Write-Stage (
            "Existing configuration preserved: " +
            $IniName
        )
        return
    }

    Copy-Item `
        -LiteralPath $source `
        -Destination $target `
        -Force

    Write-Stage ("Installed module: " + $IniName)
}

function Remove-ModuleIni {
    param(
        [string]$ResolvedGamePath,
        [string]$IniName
    )

    $target = Join-Path $ResolvedGamePath $IniName

    if (Test-Path -LiteralPath $target) {
        Remove-Item -LiteralPath $target -Force
        Write-Stage ("Removed module: " + $IniName)
    }
    else {
        Write-Stage ("Module not installed: " + $IniName)
    }
}

try {
    if (Get-Process -Name "DS2" -ErrorAction SilentlyContinue) {
        throw "Close DEATH STRANDING 2 before continuing."
    }

    $resolvedSelection = Resolve-Selection `
        -RequestedSelection $Selection `
        -RequestedAction $Action

    $resolvedGamePath = Resolve-GamePath `
        -RequestedPath $GamePath

    $packagePath = Split-Path -Parent $MyInvocation.MyCommand.Path
    $backupFolder = Join-Path $resolvedGamePath "DS2VehicleMods_Backup"

    $speedIniName = "VehicleSpeedBoost.ini"
    $hillIniName = "PickupTruckHillAssist.ini"

    if ($Action -eq "Install") {
        Test-GameBuild -ResolvedGamePath $resolvedGamePath

        Install-UltimateAsiLoader `
            -ResolvedGamePath $resolvedGamePath `
            -BackupFolder $backupFolder

        # Prevent the old standalone Hill Assist ASI from creating a
        # second streaming listener.
        $legacyHillAsi = Join-Path `
            $resolvedGamePath `
            "PickupTruckHillAssist.asi"

        if (Test-Path -LiteralPath $legacyHillAsi) {
            Backup-File `
                -Path $legacyHillAsi `
                -BackupFolder $backupFolder

            Remove-Item -LiteralPath $legacyHillAsi -Force

            Write-Stage (
                "Legacy standalone Hill Assist ASI disabled to avoid " +
                "a duplicate streaming listener."
            )
        }

        Install-SharedCore `
            -ResolvedGamePath $resolvedGamePath `
            -PackagePath $packagePath `
            -BackupFolder $backupFolder

        if (
            $resolvedSelection -eq "Speed" -or
            $resolvedSelection -eq "Both"
        ) {
            Install-ModuleIni `
                -ResolvedGamePath $resolvedGamePath `
                -PackagePath $packagePath `
                -IniName $speedIniName
        }

        if (
            $resolvedSelection -eq "Hill" -or
            $resolvedSelection -eq "Both"
        ) {
            Install-ModuleIni `
                -ResolvedGamePath $resolvedGamePath `
                -PackagePath $packagePath `
                -IniName $hillIniName
        }

        Write-Host ""
        Write-Host "Installation completed." -ForegroundColor Green

        if ($resolvedSelection -eq "Both") {
            Write-Host (
                "Vehicle Speed Boost and Pickup Truck Hill Assist are " +
                "both active through one Shared Core."
            ) -ForegroundColor Green
        }
    }
    else {
        if (
            $resolvedSelection -eq "Speed" -or
            $resolvedSelection -eq "Both"
        ) {
            Remove-ModuleIni `
                -ResolvedGamePath $resolvedGamePath `
                -IniName $speedIniName
        }

        if (
            $resolvedSelection -eq "Hill" -or
            $resolvedSelection -eq "Both"
        ) {
            Remove-ModuleIni `
                -ResolvedGamePath $resolvedGamePath `
                -IniName $hillIniName
        }

        $speedStillInstalled = Test-Path -LiteralPath (
            Join-Path $resolvedGamePath $speedIniName
        )
        $hillStillInstalled = Test-Path -LiteralPath (
            Join-Path $resolvedGamePath $hillIniName
        )

        if (-not $speedStillInstalled -and -not $hillStillInstalled) {
            $corePath = Join-Path $resolvedGamePath "DS2VehicleCore.asi"

            if (Test-Path -LiteralPath $corePath) {
                $installedHash = (
                    Get-FileHash `
                        -LiteralPath $corePath `
                        -Algorithm SHA256
                ).Hash

                if ($installedHash -eq $ExpectedCoreHash) {
                    Remove-Item -LiteralPath $corePath -Force
                    Write-Stage (
                        "No vehicle module remains; Shared Core removed."
                    )
                }
                else {
                    Write-Stage (
                        "Shared Core was not removed because a different " +
                        "version is installed."
                    )
                }
            }
        }
        else {
            Write-Stage (
                "Another vehicle module remains active; Shared Core kept."
            )
        }

        Write-Host ""
        Write-Host "Uninstallation completed." -ForegroundColor Green
        Write-Host (
            "The ASI Loader is intentionally kept because other mods " +
            "may still require it."
        )
    }
}
catch {
    Write-Host ""
    Write-Host ("ERROR: " + $_.Exception.Message) `
        -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter"
    exit 1
}
