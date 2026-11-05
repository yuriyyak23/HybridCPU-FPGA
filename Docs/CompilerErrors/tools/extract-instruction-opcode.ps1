$ErrorActionPreference='Stop'
$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common'
$p=Join-Path $dir 'YAKSys_Hybrid_CPU.Processor.h'
$header=Join-Path $dir 'InstructionOpcode.h'
$s=[IO.File]::ReadAllText($p)
$pattern='enum class InstructionsEnum\s*:\s*int\s*\{[^}]*\}\s*;'
$matches=[regex]::Matches($s,$pattern)
if($matches.Count -ne 1 -or (Test-Path -LiteralPath $header)){throw 'Unexpected processor or opcode header state'}
$enum=$matches[0].Value
$declaration="#pragma once`r`n`r`nnamespace YAKSys_Hybrid_CPU`r`n{`r`n"+$enum+"`r`n}`r`n"
[IO.File]::WriteAllText($header,$declaration,[Text.UTF8Encoding]::new($false))
$s=$s.Remove($matches[0].Index,$matches[0].Length).Insert($matches[0].Index,'using InstructionsEnum = ::YAKSys_Hybrid_CPU::InstructionsEnum;')
$s=$s.Replace('#pragma once',"#pragma once`r`n`r`n#include `"InstructionOpcode.h`"")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileResourceContour.h'
$s=[IO.File]::ReadAllText($p)
$old='#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($old)){throw 'Expected processor include not found'}
$s=$s.Replace($old,'#include "../../../../../Architecture/Common/InstructionOpcode.h"')
$ns='namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile'
$s=$s.Replace($ns,$ns+"`r`n")
$s=$s.Replace("`r`n{`r`n`r`n`t enum", "`r`n{`r`n`r`n`t enum")
$s=$s.Replace('using namespace YAKSys_Hybrid_CPU::Core;',"using namespace YAKSys_Hybrid_CPU::Core;`r`nusing YAKSys_Hybrid_CPU::InstructionsEnum;")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
