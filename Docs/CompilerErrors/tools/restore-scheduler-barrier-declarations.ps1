$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h';$s=Get-Content $p -Raw
$s=$s.Replace('#include <vector>',"#include <vector>`r`n#include <array>")
$s=$s.Replace('namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; }', "namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; }`r`nnamespace HybridCPU_ISE::CloseToHSL::Memory::Banks { class BankArbitrator; }")
$s=$s.Replace('std::shared_ptr<BankArbitrator> arbitrator','std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Banks::BankArbitrator> arbitrator').Replace('std::shared_ptr<> bundle', 'std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> bundle').Replace('(int vtId, int instrIndex, int[] threads)[] _barriers = new(int, int, int[])[16];', 'struct BarrierEntry { int vtId = 0, instrIndex = 0; std::vector<int> threads; };' + "`r`n`t`tstd::array<BarrierEntry, 16> _barriers{};").Replace('readonly MicroOp[]? _bundleSlots;', 'std::vector<std::shared_ptr<MicroOp>> _bundleSlots;')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp';$s=Get-Content $p -Raw
$s=$s.Replace('std::shared_ptr<> bundle', 'std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> bundle').Replace('_barriers->Length', '_barriers.size()').Replace('_barriers[_barrierCount] = (vtId, instructionIndex, threads);', '_barriers[_barrierCount] = BarrierEntry{vtId, instructionIndex, threads};').Replace('_barriers[writeIdx - 1]->vtId', '_barriers[writeIdx - 1].vtId')
Set-Content $p $s -Encoding UTF8 -NoNewline
