$ErrorActionPreference='Stop'
$dir=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Memory\DMA'
$p=Join-Path $dir 'DMAController.h'; $s=Get-Content $p -Raw
$s=$s.Replace('#include "../../Core/Memory/MemoryCoherencyObserver.h"', 'namespace YAKSys_Hybrid_CPU::Core::Memory { class MemoryCoherencyObserver; }')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $dir 'DMAController.cpp'; $s=Get-Content $p -Raw
if (!$s.Contains('#include "../../Core/Memory/MemoryCoherencyObserver.h"')) { $s=$s.Replace('#include "DMAController.h"', "#include `"DMAController.h`"`r`n#include `"../../Core/Memory/MemoryCoherencyObserver.h`"") }
Set-Content $p $s -Encoding UTF8 -NoNewline
