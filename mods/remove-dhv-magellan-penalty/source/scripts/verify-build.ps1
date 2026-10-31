param([Parameter(Mandatory=$true)][string]$GameExe)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$exe = $GameExe
$expectedHash = 'BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B'
$actualHash = (Get-FileHash $exe -Algorithm SHA256).Hash
if ($actualHash -ne $expectedHash) { throw "DS2 reference hash mismatch: $actualHash" }
$stream = [IO.File]::OpenRead($exe)
$reader = [IO.BinaryReader]::new($stream)
try {
  $stream.Position = 0x3c
  $pe = $reader.ReadUInt32()
  $stream.Position = $pe + 6
  $sectionCount = $reader.ReadUInt16()
  $stream.Position = $pe + 8
  $timestamp = $reader.ReadUInt32()
  if ($timestamp -ne 0x6A3DAE46) { throw 'PE timestamp mismatch' }
  $stream.Position = $pe + 20
  $optionalSize = $reader.ReadUInt16()
  $stream.Position = $pe + 0x50
  $imageSize = $reader.ReadUInt32()
  if ($imageSize -ne 0x0B292000) { throw 'PE image size mismatch' }
  $sectionBase = $pe + 24 + $optionalSize
  $textSection = $null
  for ($i=0; $i -lt $sectionCount; $i++) {
    $stream.Position = $sectionBase + $i*40
    $name = [Text.Encoding]::ASCII.GetString($reader.ReadBytes(8)).Trim([char]0)
    $virtualSize = $reader.ReadUInt32()
    $virtualRva = $reader.ReadUInt32()
    $rawSize = $reader.ReadUInt32()
    $rawOffset = $reader.ReadUInt32()
    if ($name -eq '.text') { $textSection = @{Rva=$virtualRva; Size=$rawSize; Offset=$rawOffset} }
  }
  if (!$textSection) { throw 'Missing .text section' }
  $source = Get-Content (Join-Path $root 'src\no_magellan_evaluation.cpp') -Raw
  $expression = '\{0x([0-9A-F]+)ULL,\{([^}]+)\},([2345]),KIND_(LIKES|RANK|RECORD|PERSIST|EVAL|BESTRANK),'
  $matches = [regex]::Matches($source,$expression)
  if ($matches.Count -ne 16) { throw "Expected 16 hook definitions, got $($matches.Count)" }
  $count = 0
  foreach ($match in $matches) {
    $rva = [Convert]::ToInt64($match.Groups[1].Value,16)
    $pattern = @($match.Groups[2].Value.Split(',') | Where-Object {$_} | ForEach-Object {[Convert]::ToByte($_.Trim().Substring(2),16)})
    $length = [int]$match.Groups[3].Value
    $kind = $match.Groups[4].Value
    if ($rva -lt $textSection.Rva -or $rva + $length -gt ($textSection.Rva + $textSection.Size)) { throw ('Hook RVA outside .text: {0:X}' -f $rva) }
    $stream.Position = $textSection.Offset + ($rva - $textSection.Rva)
    $actual = $reader.ReadBytes($length)
    for($j=0;$j -lt $length;$j++){if($actual[$j] -ne $pattern[$j]){throw ('Hook signature mismatch: RVA 0x{0:X}' -f $rva)}}
    $prefix = if ($kind -eq 'LIKES') {@(0x0F,0x44,0xC8)} elseif ($kind -eq 'RANK') {@(0xD1,0xE8)} elseif ($kind -eq 'RECORD') {@(0x24,0x01)} elseif ($kind -eq 'PERSIST') {@(0x48,0x8D,0x55,0xB8,0x49,0x8B,0xCD)} elseif ($kind -eq 'EVAL') {@(0x48,0x8B,0x01,0x48,0x8D,0x55,0xB8)} else {@(0x40,0x84,0x75,0x18,0x41,0x8B,0xCE,0x41,0x0F,0xB6,0xC0)}
    $stream.Position = $textSection.Offset + ($rva - $textSection.Rva) - $prefix.Count
    $actualPrefix = $reader.ReadBytes($prefix.Count)
    for($j=0;$j -lt $prefix.Count;$j++){if($actualPrefix[$j] -ne $prefix[$j]){throw ('Hook prefix mismatch: RVA 0x{0:X}' -f $rva)}}
    $count++
  }
  $binary = Join-Path $root 'build\public\ds2_no_magellan_evaluation.asi'
  if (!(Test-Path $binary)) {throw 'Compiled ASI missing'}
  Write-Host "STATIC OK: PE hash/build, $count runtime hook signatures, source and ASI present."
  Write-Host "Candidate ASI: $((Get-FileHash $binary -Algorithm SHA256).Hash)"
  Write-Host 'Note: static validation does not prove game rank, achievement, or thread behavior.'
}
finally { $reader.Dispose(); $stream.Dispose() }
