#pragma once
#include "FrozenBytes.h"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
namespace HybridCpu::ShadowFrontend {
struct Snapshot final {
    const std::wstring Kind;
    const std::shared_ptr<const FrozenBytes> Content;
    Snapshot(std::wstring kind, std::shared_ptr<const FrozenBytes> content) : Kind(std::move(kind)), Content(std::move(content)) {}
    bool operator==(const Snapshot& other) const {
        if (Kind != other.Kind || bool(Content) != bool(other.Content)) return false;
        return !Content || (Content->getSpan().size() == other.Content->getSpan().size() && std::equal(Content->getSpan().begin(), Content->getSpan().end(), other.Content->getSpan().begin()));
    }
};
struct TypedDecodeFailure final {
    std::uint16_t Code; int SlotIndex; std::wstring Field, RawHash, Message;
    bool operator==(const TypedDecodeFailure&) const = default;
};
struct SemanticSlot final {
    int SlotIndex; bool IsOccupied; std::uint32_t Opcode;
    std::optional<int> InstructionClass, SerializationClass;
    std::uint8_t Rd, Rs1, Rs2; std::int64_t Immediate;
    std::optional<std::uint16_t> CsrAddress;
    bool AcquireOrdering, ReleaseOrdering;
    std::shared_ptr<const Snapshot> RawSlot, InstructionPayload, SlotSideband, StaticBinding, ScalarLoadAddressPlan;
    bool operator==(const SemanticSlot& other) const {
        const auto equalSnapshot = [](const auto& a, const auto& b) { return (!a && !b) || (a && b && *a == *b); };
        return SlotIndex == other.SlotIndex && IsOccupied == other.IsOccupied && Opcode == other.Opcode &&
            InstructionClass == other.InstructionClass && SerializationClass == other.SerializationClass &&
            Rd == other.Rd && Rs1 == other.Rs1 && Rs2 == other.Rs2 && Immediate == other.Immediate &&
            CsrAddress == other.CsrAddress && AcquireOrdering == other.AcquireOrdering && ReleaseOrdering == other.ReleaseOrdering &&
            equalSnapshot(RawSlot, other.RawSlot) && equalSnapshot(InstructionPayload, other.InstructionPayload) &&
            equalSnapshot(SlotSideband, other.SlotSideband) && equalSnapshot(StaticBinding, other.StaticBinding) &&
            equalSnapshot(ScalarLoadAddressPlan, other.ScalarLoadAddressPlan);
    }
};
}
