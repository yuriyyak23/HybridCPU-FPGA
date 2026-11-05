$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common'
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.h'; $s=[IO.File]::ReadAllText($p)
$m='#include "../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileExecuteCaptureAbi.h"'
if(-not $s.Contains($m)){throw 'Expected capture header include not found'}
$decl='namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile { class MatrixTileTileImage; class MatrixTileCapturedMemoryWrite; }'
$s=$s.Replace($m,$decl); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($m)){throw 'Expected processor header missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileExecuteCaptureAbi.h"'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
