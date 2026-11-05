#pragma once

#include "../../../Pipeline/Scheduling/SlotPlacementMetadata.h"
#include "../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "VirtualizationLaneBindingPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationLaneBindingPolicy final : public std::enable_shared_from_this<VirtualizationLaneBindingPolicy>
	{
	public:
		static constexpr unsigned char Lane6Id = 6;
		static constexpr unsigned char Lane7Id = 7;

	private:
		static inline const SlotPlacementMetadata CompatibilityFrontendPlacement = [] { SlotPlacementMetadata placement; placement.RequiredSlotClass = SlotClass::SystemSingleton; placement.PinningKind = SlotPinningKind::HardPinned; placement.PinnedLaneId = Lane7Id; return placement; }();
		static inline const SlotPlacementMetadata Lane6DescriptorPlacement = [] { SlotPlacementMetadata placement; placement.RequiredSlotClass = SlotClass::DmaStreamClass; placement.PinningKind = SlotPinningKind::HardPinned; placement.PinnedLaneId = Lane6Id; return placement; }();

	public:

		static SlotPlacementMetadata getCompatibilityFrontendPlacement();

		static SlotPlacementMetadata getLane6DescriptorPlacement();

		VirtualizationLaneBindingDecision Evaluate(std::shared_ptr<VirtualizationLaneBindingRequest> request);

		bool CanBind(std::shared_ptr<VirtualizationLaneBindingRequest> request);
	};
}
