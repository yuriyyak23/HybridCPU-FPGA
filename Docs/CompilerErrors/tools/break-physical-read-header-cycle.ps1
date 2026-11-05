$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationContour.h'
$s=Get-Content $p -Raw
$owner=$s.IndexOf("`tclass CpuInstructionTranslationOwner final"); $start=$s.IndexOf("`t`tclass PhysicalReadBinding final",$owner); $end=$s.IndexOf("`r`n`t`t};",$start)
if($end -lt 0){$end=$s.IndexOf("`n`t`t};",$start);$newline=1}else{$newline=2}
if($owner -lt 0 -or $start -lt 0 -or $end -lt 0){throw 'Binding class boundaries missing'}
$end += $newline + 5
$binding=$s.Substring($start,$end-$start).Replace("`t`tclass PhysicalReadBinding final", "`tclass PhysicalReadBinding final")
$s=$s.Substring(0,$start)+"`t`tusing PhysicalReadBinding = YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding;"+$s.Substring($end)
$owner=$s.IndexOf("`tclass CpuInstructionTranslationOwner final")
$s=$s.Insert($owner,"`tclass CpuInstructionTranslationOwner;`r`n"+$binding+"`r`n`r`n")
$s=$s.Replace("`tclass CpuInstructionTranslationOwner final : public std::enable_shared_from_this<CpuInstructionTranslationOwner>`r`n`t{", "`tclass CpuInstructionTranslationOwner final : public std::enable_shared_from_this<CpuInstructionTranslationOwner>`r`n`t{`r`n        friend class PhysicalReadBinding;")
Set-Content $p $s -Encoding UTF8 -NoNewline
$paths=@('CloseToHSL\\Memory\Timing\MemoryCycleController.h','CloseToHSL\\Memory\Timing\MemoryCycleController.cpp','CloseToHSL\\Memory\Subsystem\YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h','CloseToHSL\\Memory\Subsystem\YAKSys_Hybrid_CPU.Memory.MemorySubsystem.cpp')
foreach($rel in $paths){$p=Join-Path $root $rel;$s=Get-Content $p -Raw;$s=$s.Replace('YAKSys_Hybrid_CPU::Core::Memory::CpuInstructionTranslationOwner::PhysicalReadBinding','YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding');if($rel.EndsWith('.h') -and !$s.Contains('namespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; }')){$s=$s.Replace('#pragma once', "#pragma once`r`nnamespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; }")};Set-Content $p $s -Encoding UTF8 -NoNewline}
