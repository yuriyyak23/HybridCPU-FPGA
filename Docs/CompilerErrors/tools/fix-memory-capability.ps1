$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h'
$s=[IO.File]::ReadAllText($p)
if (!$s.Contains('#include "MemoryCapability.h"')) {
 $s=$s.Replace('#include "ExecutionPlacement.h"','#include "ExecutionPlacement.h"'+"`r`n"+'#include "MemoryCapability.h"')
 foreach($name in @('MemoryCapabilityKind','MemoryAccessDirection')) {
  $pattern='(?s)\tenum class '+$name+'\s*:\s*unsigned char\s*\{.*?\n\t\};'
  $matches=[regex]::Matches($s,$pattern); if($matches.Count -gt 1){throw "Duplicate enum $name"}; if($matches.Count -eq 1){$s=[regex]::Replace($s,$pattern,'')}
 }
 [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false));Write-Output 'Installed MemoryCapability'
} else {Write-Output 'Already installed'}
