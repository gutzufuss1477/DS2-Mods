$ErrorActionPreference = 'SilentlyContinue'

$candidates = [System.Collections.Generic.List[string]]::new()

function Add-Candidate([string]$path) {
    if ([string]::IsNullOrWhiteSpace($path)) { return }
    try { $full = [System.IO.Path]::GetFullPath($path) } catch { return }
    if (-not $candidates.Contains($full)) { $candidates.Add($full) }
}

Add-Candidate $PSScriptRoot
Add-Candidate (Split-Path -Parent $PSScriptRoot)
Add-Candidate (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
Add-Candidate 'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH'
Add-Candidate 'C:\Program Files\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH'

$steamRoots = [System.Collections.Generic.List[string]]::new()
$hkcu = (Get-ItemProperty 'HKCU:\Software\Valve\Steam').SteamPath
$hklm = (Get-ItemProperty 'HKLM:\SOFTWARE\WOW6432Node\Valve\Steam').InstallPath
foreach ($root in @($hkcu, $hklm, 'C:\Program Files (x86)\Steam', 'C:\Program Files\Steam')) {
    if (-not [string]::IsNullOrWhiteSpace($root) -and -not $steamRoots.Contains($root)) {
        $steamRoots.Add($root)
    }
}

foreach ($root in $steamRoots) {
    Add-Candidate (Join-Path $root 'steamapps\common\DEATH STRANDING 2 - ON THE BEACH')
    $vdf = Join-Path $root 'steamapps\libraryfolders.vdf'
    if (Test-Path -LiteralPath $vdf) {
        $text = Get-Content -LiteralPath $vdf -Raw
        foreach ($match in [regex]::Matches($text, '"path"\s+"([^"]+)"')) {
            $library = $match.Groups[1].Value -replace '\\\\','\'
            Add-Candidate (Join-Path $library 'steamapps\common\DEATH STRANDING 2 - ON THE BEACH')
        }
    }
}

foreach ($candidate in $candidates) {
    if (Test-Path -LiteralPath (Join-Path $candidate 'DS2.exe')) {
        Write-Output $candidate
        exit 0
    }
}
exit 1
