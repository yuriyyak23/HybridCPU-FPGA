$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$p=Join-Path $r 'Decoder\DecodedBundleDescriptor.h'; $s=[IO.File]::ReadAllText($p)
$pattern='enum class HazardEffectKind\s*:\s*unsigned char\s*\{[^}]*\}\s*;'; $matches=[regex]::Matches($s,$pattern)
$h=Join-Path $r 'Decoder\HazardEffectKind.h'
if($matches.Count -ne 1 -or (Test-Path -LiteralPath $h)){throw 'Unexpected hazard enum state'}
[IO.File]::WriteAllText($h,"#pragma once`r`n`r`nnamespace YAKSys_Hybrid_CPU::Core`r`n{`r`n"+$matches[0].Value+"`r`n}`r`n",[Text.UTF8Encoding]::new($false))
$s=$s.Remove($matches[0].Index,$matches[0].Length)
$s=$s.Replace('#pragma once',"#pragma once`r`n`r`n#include `"HazardEffectKind.h`"")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Legality\DecodedSlotLegality.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../Decoder/DecodedBundleDescriptor.h"'
if(-not $s.Contains($m)){throw 'Expected descriptor include missing'}
$s=$s.Replace($m,'#include "../Decoder/HazardEffectKind.h"'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
