$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Commit'; $p=Join-Path $r 'NeutralArchitecturalCompletionFacts.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct NeutralScalarFact final')) { $s=$s.Replace('#pragma once',"#pragma once`r`n#include <stdexcept>"); $i=$s.LastIndexOf('}'); $def=@"
struct NeutralScalarFact final { bool IsPresent = false; unsigned long long Value = 0; static NeutralScalarFact Present(unsigned long long value) noexcept { return {true, value}; } };
struct NeutralAddressFact final {
    bool IsPresent = false; unsigned long long Value = 0; NeutralFaultAddressSemantic Semantic = NeutralFaultAddressSemantic::None;
    static NeutralAddressFact Present(unsigned long long value, NeutralFaultAddressSemantic semantic) { if (semantic == NeutralFaultAddressSemantic::None) throw std::invalid_argument("semantic"); return {true, value, semantic}; }
};
struct NeutralAuxiliaryFact final {
    bool IsPresent = false; unsigned long long Value = 0; NeutralFaultAuxiliarySemantic Semantic = NeutralFaultAuxiliarySemantic::None;
    static NeutralAuxiliaryFact Present(unsigned long long value, NeutralFaultAuxiliarySemantic semantic) { if (semantic == NeutralFaultAuxiliarySemantic::None) throw std::invalid_argument("semantic"); return {true, value, semantic}; }
};
struct NeutralArchitecturalCompletionFacts final {
    NeutralArchitecturalCompletionClass CompletionClass = NeutralArchitecturalCompletionClass::None;
    NeutralScalarFact Reason, Qualification; NeutralAddressFact FaultAddress; NeutralAuxiliaryFact FaultAuxiliary;
};
struct NeutralTranslationProvenance final {
    bool IsPresent = false; unsigned long long MemoryOperationId = 0, TranslationOwnerEpoch = 0, MemoryDomainOwnerEpoch = 0, AddressSpaceGeneration = 0, AddressSpaceIdentity = 0;
    static NeutralTranslationProvenance Present(unsigned long long operation, unsigned long long translationEpoch, unsigned long long ownerEpoch = 0, unsigned long long generation = 0, unsigned long long addressIdentity = 0) noexcept { return {true, operation, translationEpoch, ownerEpoch, generation, addressIdentity}; }
    bool hasStageOneIdentity() const noexcept { return IsPresent && MemoryOperationId && TranslationOwnerEpoch; }
    bool hasSecondStageIdentity() const noexcept { return hasStageOneIdentity() && MemoryDomainOwnerEpoch && AddressSpaceGeneration && AddressSpaceIdentity; }
};
struct ArchitecturalCompletionCandidate final {
    unsigned long long DomainId = 0; int ContextId = 0, VirtualThreadId = 0; unsigned long long AttemptId = 0, EventId = 0;
    NeutralArchitecturalCompletionFacts Facts; NeutralTranslationProvenance TranslationProvenance;
};
"@; $s=$s.Insert($i,$def+"`r`n") }; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'ArchitecturalCompletionCommitOwner.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "NeutralArchitecturalCompletionFacts.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"NeutralArchitecturalCompletionFacts.h`"")}; [IO.File]::WriteAllText($p,$s)
