$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationContour.h'; $s=Get-Content $p -Raw
$s=$s.Replace('#include <mutex>', "#include <mutex>`r`n#include <map>")
$s=$s.Replace('namespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; }', "namespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; }`r`nnamespace HybridCPU_ISE::CloseToHSL::Memory::Timing { class MemoryCycleController; }")
$s=$s.Replace('const std::shared_ptr<System::Runtime::CompilerServices::ConditionalWeakTable<std::shared_ptr<PhysicalReadBinding>, std::shared_ptr<PhysicalReadRequest>>> physicalReads = std::make_shared<System::Runtime::CompilerServices::ConditionalWeakTable<std::shared_ptr<PhysicalReadBinding>, std::shared_ptr<PhysicalReadRequest>>>();', @"
        struct PhysicalReadRequest { CpuInstructionTranslationRequest Request; unsigned long long Address; };
        std::mutex physicalReadsGate;
        std::map<std::weak_ptr<PhysicalReadBinding>, PhysicalReadRequest, std::owner_less<std::weak_ptr<PhysicalReadBinding>>> physicalReads;
"@)
$s=$s.Replace('std::any controller;', 'std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller;').Replace('std::any controller = std::any()', 'std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller = nullptr').Replace('std::any controller);', 'std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller);').Replace('TryClaim(std::any target,','TryClaim(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target,').Replace('Close(std::any target,','Close(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target,')
$s=$s.Replace('static CpuInstructionTranslationResult Success(std::uint64_t', "bool operator==(const CpuInstructionTranslationResult&) const = default;`r`n        static CpuInstructionTranslationResult Success(std::uint64_t")
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationContour.cpp'; $s=Get-Content $p -Raw
$s=$s.Replace('std::any controller) : owner(owner)', 'std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller) : owner(owner)').Replace('TryClaim(std::any target,', 'TryClaim(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target,').Replace('Close(std::any target,', 'Close(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target,').Replace('std::any controller)', 'std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller)').Replace('!this->ReferenceEquals(controller, target)', 'controller != target').Replace('if (Translate(request) != result)', 'if (*Translate(request) != *result)')
$s=$s.Replace('physicalReads->Add(binding, std::make_shared<PhysicalReadRequest>(request, result->PhysicalAddress));', @"
        {
            std::lock_guard lock(physicalReadsGate);
            for (auto it = physicalReads.begin(); it != physicalReads.end();) { if (it->first.expired()) it = physicalReads.erase(it); else ++it; }
            physicalReads.emplace(std::weak_ptr<PhysicalReadBinding>(binding), PhysicalReadRequest{*request, result->PhysicalAddress});
        }
"@)
$a=$s.IndexOf("`tbool CpuInstructionTranslationOwner::HasPhysicalRead("); $b=$s.IndexOf('// C# TO C++ CONVERTER TASK: Throw expressions', $a)
if($a -lt 0 -or $b -lt 0){throw 'Physical read methods missing'}
$methods=@"
    bool CpuInstructionTranslationOwner::HasPhysicalRead(std::shared_ptr<PhysicalReadBinding> binding, unsigned long long address, int size) {
        std::lock_guard lock(physicalReadsGate);
        auto it = physicalReads.find(std::weak_ptr<PhysicalReadBinding>(binding));
        return it != physicalReads.end() && it->second.Address == address && it->second.Request.AccessSize == size;
    }
    bool CpuInstructionTranslationOwner::ReadPhysical(std::shared_ptr<PhysicalReadBinding> binding, std::shared_ptr<Processor::MainMemoryArea> backing, unsigned long long address, std::vector<unsigned char>& destination) {
        if (backing != _physicalMemory || !HasPhysicalRead(binding, address, static_cast<int>(destination.size()))) throw std::logic_error("CPU physical read lost its original translation/backing/range binding.");
        return _physicalMemory->TryReadPhysicalRange(address, destination);
    }

"@
$s=$s.Substring(0,$a)+$methods+$s.Substring($b)
Set-Content $p $s -Encoding UTF8 -NoNewline
