$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\MicroOps\Memory\MicroOp.LoadStore.h'; $s=Get-Content $p -Raw
$s=$s.Replace('#include "../../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"','namespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; enum class CpuInstructionTranslationAccessKind : unsigned char; }').Replace('CpuInstructionTranslationOwner::PhysicalReadBinding','PhysicalReadBinding')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\MicroOps\Memory\MicroOp.LoadStore.cpp'; $s=Get-Content $p -Raw
$s=$s.Replace('CpuInstructionTranslationOwner::PhysicalReadBinding','PhysicalReadBinding')
if (!$s.Contains('#include "../../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"')) { $s=$s.Replace('#include "MicroOp.LoadStore.h"', "#include `"MicroOp.LoadStore.h`"`r`n#include `"../../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h`"") }
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Memory\Subsystem\YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h'; $s=Get-Content $p -Raw; $s=$s.Replace('#include "../../Core/Runtime/Memory/Translation/CpuInstructionTranslationContour.h"',''); Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Memory\Subsystem\YAKSys_Hybrid_CPU.Memory.MemorySubsystem.cpp'; $s=Get-Content $p -Raw
if (!$s.Contains('#include "../../Core/Runtime/Memory/Translation/CpuInstructionTranslationContour.h"')) { $s=$s.Replace('#include "YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"', "#include `"YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h`"`r`n#include `"../../Core/Runtime/Memory/Translation/CpuInstructionTranslationContour.h`"") }; Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\State\CpuInstructionTranslationState.h'; $s=Get-Content $p -Raw; $s=$s.Replace('#include "../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"', 'namespace YAKSys_Hybrid_CPU::Core::Memory { class CpuInstructionTranslationOwner; struct CpuInstructionTranslationFault; }'); Set-Content $p $s -Encoding UTF8 -NoNewline
