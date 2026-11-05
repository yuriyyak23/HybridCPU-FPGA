$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common'
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"'
if(-not $s.Contains($m)){throw 'Expected translation include missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core::Memory { class CpuInstructionTranslationRequest; class CpuInstructionTranslationResult; class CpuInstructionTranslationOwner; class CpuInstructionTranslationFault; enum class CpuInstructionTranslationAccessKind : unsigned char; }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($m)){throw 'Expected processor include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
