$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime'
$p=Join-Path $root 'Memory\Translation\MemoryDomainTranslationControl.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <limits>',"#include <limits>`r`n#include `"../AddressSpaces/AddressSpaceId.h`"")
$anchor='class MemoryDomainTranslationEpoch final'; $def=@"
struct RuntimeEpochAdvanceResult final {
    RuntimeEpochAdvanceDecision Decision; unsigned long long Epoch; bool RequiresDomainFlush;
    bool isSucceeded() const noexcept { return Decision == RuntimeEpochAdvanceDecision::Advanced; }
    static std::shared_ptr<RuntimeEpochAdvanceResult> Advanced(unsigned long long epoch) { return std::make_shared<RuntimeEpochAdvanceResult>(RuntimeEpochAdvanceResult{RuntimeEpochAdvanceDecision::Advanced, epoch, false}); }
    static std::shared_ptr<RuntimeEpochAdvanceResult> DeniedWraparound(unsigned long long epoch) { return std::make_shared<RuntimeEpochAdvanceResult>(RuntimeEpochAdvanceResult{RuntimeEpochAdvanceDecision::DeniedWraparound, epoch, true}); }
};

"@; $def=$def.TrimEnd(); if(-not $s.Contains('struct RuntimeEpochAdvanceResult final')) {$s=$s.Replace($anchor,$def+"`r`n`r`n`t"+$anchor)}
$def=@"
struct MemoryDomainTranslationControl final {
    bool TranslationEnabled = false, AddressSpaceTaggingEnabled = false;
    unsigned long long AddressSpaceRoot = 0, SecondStageRoot = 0;
    unsigned short DomainTag = 0, AddressSpaceTag = 0;
    unsigned long long AddressSpaceGeneration = 0;
    unsigned char DefaultMemoryType = 6, AddressSpaceTargetCount = 0;
    static constexpr unsigned char WriteBackMemoryType = 6, MaxAddressSpaceTargetCount = 4;
    bool isValid() const noexcept { return !TranslationEnabled || ((AddressSpaceRoot & 0xfffULL) == 0 && (SecondStageRoot & 0xfffULL) == 0 && (!AddressSpaceTaggingEnabled || AddressSpaceTag != 0) && AddressSpaceTargetCount <= MaxAddressSpaceTargetCount); }
    YAKSys_Hybrid_CPU::Memory::AddressSpaceId ToAddressSpaceId(unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch) const noexcept { return {DomainTag, static_cast<unsigned short>(AddressSpaceTaggingEnabled ? AddressSpaceTag : 0), SecondStageRoot, secondStageEpoch, addressSpaceTagEpoch, AddressSpaceGeneration}; }
    static const std::shared_ptr<MemoryDomainTranslationControl> Disabled;
};
inline const std::shared_ptr<MemoryDomainTranslationControl> MemoryDomainTranslationControl::Disabled = std::make_shared<MemoryDomainTranslationControl>();
"@
if(-not $s.Contains('struct MemoryDomainTranslationControl final')) { $pos=$s.LastIndexOf('}'); $s=$s.Insert($pos,$def+"`r`n") }; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'Domains\Descriptors\MemoryDomain\YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "../../../Memory/Translation/MemoryDomainTranslationControl.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../../Memory/Translation/MemoryDomainTranslationControl.h`"")}; [IO.File]::WriteAllText($p,$s)
