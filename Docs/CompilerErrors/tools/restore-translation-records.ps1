$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationContour.h'
$s=Get-Content $p -Raw
if($s.Contains('struct CpuInstructionTranslationScope final')) { throw 'Records already restored' }
$s=$s.Replace('#include <limits>', "#include <limits>`r`n#include <optional>`r`n#include <cstdint>")
$marker="`t/// <summary>`r`n`t/// Immutable CPU-owned address-translation policy."
if(!$s.Contains($marker)){ $marker="`t/// <summary>`n`t/// Immutable CPU-owned address-translation policy." }
if(!$s.Contains($marker)){throw 'Policy insertion point missing'}
$records=@"
    struct CpuInstructionTranslationScope final {
        std::uint64_t DomainId; int ContextId, VirtualThreadId;
        bool isValid() const { return DomainId != 0 && ContextId > 0 && VirtualThreadId >= 0 && VirtualThreadId < Processor::CPU_Core::SmtWays; }
        bool operator==(const CpuInstructionTranslationScope&) const = default;
    };
    struct CpuInstructionTranslationRegion final {
        std::uint64_t VirtualBase, PhysicalBase, Length; bool Readable, Writable, Executable;
        bool isValid() const { auto max = std::numeric_limits<std::uint64_t>::max(); return Length != 0 && VirtualBase <= max - (Length - 1) && PhysicalBase <= max - (Length - 1) && (Readable || Writable || Executable); }
        bool operator==(const CpuInstructionTranslationRegion&) const = default;
    };
    struct CpuInstructionStageOneRegion final {
        std::uint64_t VirtualBase, GuestPhysicalBase, Length; bool Readable, Writable, Executable;
        bool isValid() const { auto max = std::numeric_limits<std::uint64_t>::max(); return Length != 0 && VirtualBase <= max - (Length - 1) && GuestPhysicalBase <= max - (Length - 1) && (Readable || Writable || Executable); }
        bool operator==(const CpuInstructionStageOneRegion&) const = default;
    };
    struct CpuInstructionTranslationOperationIdentity final {
        std::uint64_t AttemptId, EventId, MemoryOperationId;
        bool isValid() const { return AttemptId != 0 && EventId != 0 && MemoryOperationId != 0; }
        bool operator==(const CpuInstructionTranslationOperationIdentity&) const = default;
    };
    struct CpuInstructionTranslationRequest final {
        CpuInstructionTranslationScope Scope; CpuInstructionTranslationAccessKind AccessKind;
        std::uint64_t VirtualAddress; std::uint16_t AccessSize; CpuInstructionTranslationOperationIdentity OperationIdentity;
        bool hasOperationIdentity() const { return AccessKind != CpuInstructionTranslationAccessKind::None && AccessSize != 0 && OperationIdentity.isValid(); }
        bool isValid() const { return Scope.isValid() && hasOperationIdentity(); }
        bool operator==(const CpuInstructionTranslationRequest&) const = default;
    };
    struct CpuInstructionTranslationFault final {
        CpuInstructionTranslationFaultReason Reason; CpuInstructionTranslationRequest Request;
        std::uint64_t OwnerEpoch, Qualification, Auxiliary;
        CpuInstructionTranslationFaultStage FaultStage = CpuInstructionTranslationFaultStage::StageOne;
        bool HasGuestPhysicalAddress = false;
        std::uint64_t GuestPhysicalAddress = 0, MemoryDomainOwnerEpoch = 0, AddressSpaceGeneration = 0, AddressSpaceIdentity = 0;
        bool isValid() const { return Reason != CpuInstructionTranslationFaultReason::None && Request.isValid() && OwnerEpoch != 0 && (FaultStage != CpuInstructionTranslationFaultStage::SecondStage || (HasGuestPhysicalAddress && MemoryDomainOwnerEpoch != 0 && AddressSpaceGeneration != 0 && AddressSpaceIdentity != 0)); }
        bool operator==(const CpuInstructionTranslationFault&) const = default;
    };
    struct CpuInstructionTranslationResult final {
        bool Succeeded; std::uint64_t PhysicalAddress, OwnerEpoch;
        std::optional<CpuInstructionTranslationFault> Fault;
        bool HasGuestPhysicalAddress = false; std::uint64_t GuestPhysicalAddress = 0;
        static CpuInstructionTranslationResult Success(std::uint64_t address, std::uint64_t epoch) { return {true, address, epoch, std::nullopt}; }
        static CpuInstructionTranslationResult Failed(const CpuInstructionTranslationFault& fault) { return {false, 0, fault.OwnerEpoch, fault}; }
    };

"@
$s=$s.Replace($marker,$records+$marker)
Set-Content $p $s -Encoding UTF8 -NoNewline
