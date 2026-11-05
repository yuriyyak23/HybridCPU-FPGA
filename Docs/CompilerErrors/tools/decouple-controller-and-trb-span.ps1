$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Memory\Timing\MemoryCycleController.h'; $s=Get-Content $p -Raw
$s=$s.Replace('#include "../Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"','').Replace('#include "../../Core/Runtime/Memory/Translation/CpuInstructionTranslationContour.h"','')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Memory\Timing\MemoryCycleController.cpp'; $s=Get-Content $p -Raw
if (!$s.Contains('#include "../Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"')) { $s=$s.Replace('#include "MemoryCycleController.h"', "#include `"MemoryCycleController.h`"`r`n#include `"../Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h`"`r`n#include `"../../Core/Runtime/Memory/Translation/CpuInstructionTranslationContour.h`"") }
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Execution\StreamEngine\BurstIO\TransactionReorderBuffer.h'; $s=Get-Content $p -Raw
if (!$s.Contains('#include <span>')) { $s=$s.Replace('#pragma once', "#pragma once`r`n#include <span>") }
$s=$s.Replace('ReadOnlySpan<bool> bankBusy','std::span<const bool> bankBusy')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Execution\StreamEngine\BurstIO\TransactionReorderBuffer.cpp'; $s=Get-Content $p -Raw
$s=$s.Replace('ReadOnlySpan<bool> bankBusy','std::span<const bool> bankBusy').Replace('bankBusy.Length','bankBusy.size()')
Set-Content $p $s -Encoding UTF8 -NoNewline
