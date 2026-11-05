$ErrorActionPreference='Stop'
$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$ns='namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile { class MatrixTileReplayRollbackJournal; }'
foreach($item in @(@('Architecture\Common\YAKSys_Hybrid_CPU.Processor.h','#include "../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileReplayRollbackAbi.h"'),@('State\ExtensionState.h','#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileReplayRollbackAbi.h"'))) {
 $p=Join-Path $root $item[0]; $s=[IO.File]::ReadAllText($p)
 if(-not $s.Contains($item[1])){throw "Include not found: $p"}
 $s=$s.Replace($item[1],$ns)
 [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
}
$p=Join-Path $root 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'; $s=[IO.File]::ReadAllText($p)
$m='#include "YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($m)){throw 'Processor source include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileReplayRollbackAbi.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
