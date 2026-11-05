$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common'
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.h'; $s=[IO.File]::ReadAllText($p)
$m1='#include "../../Execution/DmaStreamCompute/DmaStreamComputeTokenStore.h"'; $m2='#include "../../Execution/DmaStreamCompute/DmaStreamComputeToken.h"'
if(-not $s.Contains($m1) -or -not $s.Contains($m2)){throw 'Expected DMA headers missing'}
$s=$s.Replace($m1,'namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { class DmaStreamComputeTokenStore; class DmaStreamComputeCommitResult; class DmaStreamComputeToken; class DmaStreamComputeOwnerGuardDecision; }').Replace($m2,'')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($m)){throw 'Expected processor include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../Execution/DmaStreamCompute/DmaStreamComputeTokenStore.h"'+"`r`n"+'#include "../../Execution/DmaStreamCompute/DmaStreamComputeToken.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
