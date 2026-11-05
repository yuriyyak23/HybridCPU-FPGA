$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Admission\Memory'; $p=Join-Path $r 'YAKSys_Hybrid_CPU.Core.MemoryDomainRuntime.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "MemoryDomainRuntime.CpuSecondStage.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"MemoryDomainRuntime.CpuSecondStage.h`"")}; $a='class MemoryDomainRuntime final'; if(-not $s.Contains('class CpuSecondStageSourceBindResult;')) {$s=$s.Replace($a,"class CpuSecondStageSourceBindResult;`r`n    class CpuSecondStageTranslationSnapshot;`r`n    $a")}
$def=@"
namespace YAKSys_Hybrid_CPU::Core {
struct CpuSecondStageSourceBindResult final {
    CpuSecondStageSourceBindDecision Decision;
    std::shared_ptr<MemoryDomainRuntime::CpuSecondStageTranslationBinding> Binding;
    std::wstring Reason;
    bool isBound() const noexcept { return Decision == CpuSecondStageSourceBindDecision::Bound && Binding != nullptr; }
};
struct CpuSecondStageTranslationSnapshot final {
    std::shared_ptr<MemoryDomainDescriptor> SourceOwner;
    unsigned long long SecondStageRoot = 0, DomainIdentity = 0, AddressSpaceIdentity = 0, MemoryDomainOwnerEpoch = 0, AddressSpaceGeneration = 0, BindingGeneration = 0;
    std::shared_ptr<AddressSpaceDescriptor> AddressSpace;
    bool isValid() const { return SecondStageRoot != 0 && DomainIdentity != 0 && AddressSpaceIdentity != 0 && MemoryDomainOwnerEpoch != 0 && AddressSpaceGeneration != 0 && BindingGeneration != 0 && AddressSpace && AddressSpace->isRuntimeAuthoritative(); }
};
}
"@; if(-not $s.Contains('struct CpuSecondStageSourceBindResult final')) {$s += "`r`n"+$def}; [IO.File]::WriteAllText($p,$s)
