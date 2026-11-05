#pragma once
#include "../Pipeline/Scheduling/LaneId.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <memory>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
class ExecutionPlacement final {
    ExecutionPlacement(SlotClass cls, SlotPinningKind pin, unsigned char lane, unsigned long long tag)
        : RequiredSlotClass(cls), PinningKind(pin), PinnedLaneId(lane), DomainTag(tag) {}
public:
    const SlotClass RequiredSlotClass;
    const SlotPinningKind PinningKind;
    const unsigned char PinnedLaneId;
    const unsigned long long DomainTag;
    static std::shared_ptr<ExecutionPlacement> Create(SlotClass cls, SlotPinningKind pin, unsigned char lane = 0, unsigned long long tag = 0) {
        if (pin == SlotPinningKind::HardPinned && lane >= LaneId::LaneCount) throw std::out_of_range("A hard-pinned lane must be in the physical eight-lane bundle.");
        return std::shared_ptr<ExecutionPlacement>(new ExecutionPlacement(cls, pin, pin == SlotPinningKind::HardPinned ? lane : 0, tag));
    }
    static std::shared_ptr<ExecutionPlacement> CreateHardPinned(SlotClass cls, LaneId lane, unsigned long long tag = 0) {
        return Create(cls, SlotPinningKind::HardPinned, lane.ToRawValue(), tag);
    }
    bool TryGetHardPinnedLaneId(LaneId &lane) const {
        if (PinningKind == SlotPinningKind::HardPinned && LaneId::TryCreate(PinnedLaneId, lane)) return true;
        lane = LaneId(); return false;
    }
};
}
