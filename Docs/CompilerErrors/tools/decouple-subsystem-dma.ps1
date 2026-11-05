$ErrorActionPreference='Stop'
$dir=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Memory\Subsystem'
$p=Join-Path $dir 'YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h'; $s=Get-Content $p -Raw
$s=$s.Replace('#include "../DMA/DMAController.h"', 'namespace HybridCPU_ISE::CloseToHSL::Memory::DMA { class DMAController; }')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $dir 'YAKSys_Hybrid_CPU.Memory.MemorySubsystem.cpp'; $s=Get-Content $p -Raw
if (!$s.Contains('#include "../DMA/DMAController.h"')) { $s=$s.Replace('#include "YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"', "#include `"YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h`"`r`n#include `"../DMA/DMAController.h`"") }
Set-Content $p $s -Encoding UTF8 -NoNewline
