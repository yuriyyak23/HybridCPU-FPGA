$ErrorActionPreference='Stop'
$header='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Certificates\ArchLegalityMask.h'
$s=[IO.File]::ReadAllText($header)
$old='const SafetyMask128 Mask;'
if (([regex]::Matches($s,[regex]::Escape($old))).Count -ne 2) { throw 'Expected exactly two mask fields' }
$s=$s.Replace($old,'SafetyMask128 Mask;')
[IO.File]::WriteAllText($header,$s,[Text.UTF8Encoding]::new($false))
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$s=[IO.File]::ReadAllText($p)
$old='ArchLegalityMask getArchLegality() const { return ArchLegality; }'
if (-not $s.Contains($old) -or $s.Contains('void setArchLegality(')) { throw 'Unexpected MicroOp state' }
$s=$s.Replace($old,$old+"`r`n"+'        void setArchLegality(ArchLegalityMask value) { ArchLegality = value; }')
$old='BackendHazardMask getBackendHazard() const { return BackendHazard; }'
if (-not $s.Contains($old)) { throw 'Missing BackendHazard accessor' }
$s=$s.Replace($old,$old+"`r`n"+'        void setBackendHazard(BackendHazardMask value) { BackendHazard = value; }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
