$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Execution\Memory\LoadStore'
$h=Join-Path $root 'MemoryUnit.h';$c=Join-Path $root 'MemoryUnit.cpp'
$ht=[IO.File]::ReadAllText($h);$ct=[IO.File]::ReadAllText($c)
$include='#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"'
if($ht.Contains($include)){$ht=$ht.Replace($include,'namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps { class InstructionIR; }'); if(!$ct.Contains($include)){$ct=$ct.Replace('#include "MemoryUnit.h"','#include "MemoryUnit.h"'+"`r`n"+$include)}}
$ht=$ht.Replace('using namespace YAKSys_Hybrid_CPU::Arch;','')
$ht=$ht.Replace('ResolvedMemoryAccess() = default;', 'ResolvedMemoryAccess() : EffectiveAddress(0), HasRegisterWrite(false), RegisterDestination(0), RegisterWriteValue(0), HasStoreCommit(false), StoreData(0), AccessSize(0) {}')
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));Write-Output 'Updated MemoryUnit dependencies'
