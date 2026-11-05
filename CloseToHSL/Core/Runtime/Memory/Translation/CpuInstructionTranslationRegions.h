#pragma once
#include <limits>
#include "../../../Architecture/Registers/Architectural/RegisterIdentity.h"
namespace YAKSys_Hybrid_CPU::Core::Memory {
struct CpuInstructionTranslationScope final {
    bool operator==(const CpuInstructionTranslationScope&) const = default;
    unsigned long long DomainId = 0;
    int ContextId = 0, VirtualThreadId = 0;
    bool isValid() const noexcept { return DomainId != 0 && ContextId > 0 && Registers::VtId::IsRepresentable(VirtualThreadId); }
};
struct CpuInstructionTranslationRegion final {
    bool operator==(const CpuInstructionTranslationRegion&) const = default;
    unsigned long long VirtualBase = 0, PhysicalBase = 0, Length = 0;
    bool Readable = false, Writable = false, Executable = false;
    bool isValid() const noexcept {
        const auto max = std::numeric_limits<unsigned long long>::max();
        return Length != 0 && VirtualBase <= max - (Length - 1) && PhysicalBase <= max - (Length - 1) && (Readable || Writable || Executable);
    }
};
struct CpuInstructionStageOneRegion final {
    bool operator==(const CpuInstructionStageOneRegion&) const = default;
    unsigned long long VirtualBase = 0, GuestPhysicalBase = 0, Length = 0;
    bool Readable = false, Writable = false, Executable = false;
    bool isValid() const noexcept {
        const auto max = std::numeric_limits<unsigned long long>::max();
        return Length != 0 && VirtualBase <= max - (Length - 1) && GuestPhysicalBase <= max - (Length - 1) && (Readable || Writable || Executable);
    }
};
}
