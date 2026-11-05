$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationContour.h'; $s=Get-Content $p -Raw
$a=$s.IndexOf('    struct CpuInstructionTranslationScope final'); $b=$s.IndexOf('    struct CpuInstructionTranslationOperationIdentity final',$a)
if($a -lt 0 -or $b -lt 0){throw 'Duplicate region definitions missing'}
$s=$s.Substring(0,$a)+$s.Substring($b); Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationRegions.h'; $s=Get-Content $p -Raw
foreach($name in @('CpuInstructionTranslationScope','CpuInstructionTranslationRegion','CpuInstructionStageOneRegion')) { $s=$s.Replace("struct $name final {", "struct $name final {`r`n    bool operator==(const $name&) const = default;") }
Set-Content $p $s -Encoding UTF8 -NoNewline
