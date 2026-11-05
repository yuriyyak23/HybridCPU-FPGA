#include <cstdint>
#include <limits>
#include <optional>
#include <cassert>
struct Processor { struct CPU_Core { static constexpr int SmtWays = 4; }; };
enum class CpuInstructionTranslationAccessKind : unsigned char { None, InstructionFetch, ScalarLoad, ScalarStore };
enum class CpuInstructionTranslationFaultReason : unsigned char { None, UnmappedAddress, AccessDenied, AddressOverflow, OwnerScopeMismatch, SecondStageViolation, SecondStageMisconfiguration, SecondStageSourceStale };
enum class CpuInstructionTranslationFaultStage : unsigned char { StageOne = 1, SecondStage = 2 };
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
    };int main() {
 CpuInstructionTranslationScope scope{1,1,3}; assert(scope.isValid()); scope.VirtualThreadId=4; assert(!scope.isValid()); scope.VirtualThreadId=0;
 auto max=std::numeric_limits<std::uint64_t>::max(); CpuInstructionTranslationRegion bad{max,0,2,true,false,false}; assert(!bad.isValid());
 CpuInstructionTranslationRequest req{scope,CpuInstructionTranslationAccessKind::ScalarLoad,42,4,{1,2,3}}; assert(req.isValid());
 CpuInstructionTranslationFault fault{CpuInstructionTranslationFaultReason::AccessDenied,req,1,0,0}; assert(fault.isValid()); fault.FaultStage=CpuInstructionTranslationFaultStage::SecondStage; assert(!fault.isValid());
 fault.HasGuestPhysicalAddress=true; fault.MemoryDomainOwnerEpoch=1; fault.AddressSpaceGeneration=1; fault.AddressSpaceIdentity=1; assert(fault.isValid());
 auto result=CpuInstructionTranslationResult::Failed(fault); assert(!result.Succeeded && result.OwnerEpoch==1 && result.Fault.has_value());
}
