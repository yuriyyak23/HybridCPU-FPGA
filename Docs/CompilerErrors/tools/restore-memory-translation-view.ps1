$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime'
$p=Join-Path $r 'Memory\Translation\MemoryDomainReadOnlyTranslationView.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct MemoryDomainReadOnlyTranslationView final')) { $s += @"

#include "MemoryDomainTranslationControl.h"
#include <memory>
#include <stdexcept>
namespace YAKSys_Hybrid_CPU::Core {
struct MemoryDomainReadOnlyTranslationView final {
    const bool TranslationEnabled, AddressSpaceTaggingEnabled, OwnsSecondStageTranslation;
    const unsigned long long AddressSpaceRoot, SecondStageRoot;
    const unsigned short DomainTag, AddressSpaceTag;
    const unsigned long long AddressSpaceGeneration;
    const unsigned char DefaultMemoryType, AddressSpaceTargetCount;
    static std::shared_ptr<MemoryDomainReadOnlyTranslationView> From(std::shared_ptr<MemoryDomainTranslationControl> control, bool owns) {
        if (!control) throw std::invalid_argument("control");
        return std::make_shared<MemoryDomainReadOnlyTranslationView>(MemoryDomainReadOnlyTranslationView{control->TranslationEnabled, control->AddressSpaceTaggingEnabled, owns, control->AddressSpaceRoot, control->SecondStageRoot, control->DomainTag, control->AddressSpaceTag, control->AddressSpaceGeneration, control->DefaultMemoryType, control->AddressSpaceTargetCount});
    }
};
}
"@ }; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Domains\Descriptors\MemoryDomain\YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "../../../Memory/Translation/MemoryDomainReadOnlyTranslationView.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../../Memory/Translation/MemoryDomainReadOnlyTranslationView.h`"")}; [IO.File]::WriteAllText($p,$s)
