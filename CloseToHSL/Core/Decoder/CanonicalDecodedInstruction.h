#pragma once
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../NonRTL/Arch/SerializationClass.h"
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
class CanonicalPayloadSnapshot;
class GeneratedStaticBinding;
class CanonicalScalarLoadAddressPlan;
enum class CanonicalSecurityOperationKind : unsigned char;

// Frozen semantic projection of one VLIW slot. Additional init-only references
// are populated by the decoder while constructing its snapshot.
class CanonicalDecodedInstruction final {
public:
    const int SlotIndex;
    const bool IsOccupied;
    const std::uint32_t Opcode;
    const std::optional<YAKSys_Hybrid_CPU::Arch::InstructionClass> InstructionClass;
    const std::optional<YAKSys_Hybrid_CPU::Arch::SerializationClass> SerializationClass;
    const std::uint8_t Rd, Rs1, Rs2;
    const std::int64_t Immediate;
    const std::optional<std::uint16_t> CsrAddress;
    const bool AcquireOrdering, ReleaseOrdering;
    const std::shared_ptr<CanonicalPayloadSnapshot> RawSlot, InstructionPayload, SlotSideband;
    std::shared_ptr<GeneratedStaticBinding> StaticBinding;
    std::shared_ptr<CanonicalScalarLoadAddressPlan> ScalarLoadAddressPlan;

    CanonicalDecodedInstruction(int slotIndex, bool isOccupied, std::uint32_t opcode,
        std::optional<YAKSys_Hybrid_CPU::Arch::InstructionClass> instructionClass,
        std::optional<YAKSys_Hybrid_CPU::Arch::SerializationClass> serializationClass,
        std::uint8_t rd, std::uint8_t rs1, std::uint8_t rs2, std::int64_t immediate,
        std::optional<std::uint16_t> csrAddress, bool acquireOrdering, bool releaseOrdering,
        std::shared_ptr<CanonicalPayloadSnapshot> rawSlot,
        std::shared_ptr<CanonicalPayloadSnapshot> instructionPayload,
        std::shared_ptr<CanonicalPayloadSnapshot> slotSideband)
        : SlotIndex(slotIndex), IsOccupied(isOccupied), Opcode(opcode),
          InstructionClass(instructionClass), SerializationClass(serializationClass),
          Rd(rd), Rs1(rs1), Rs2(rs2), Immediate(immediate), CsrAddress(csrAddress),
          AcquireOrdering(acquireOrdering), ReleaseOrdering(releaseOrdering),
          RawSlot(std::move(rawSlot)), InstructionPayload(std::move(instructionPayload)),
          SlotSideband(std::move(slotSideband)) {}

    CanonicalSecurityOperationKind getSecurityOperationKind() const;
};
}