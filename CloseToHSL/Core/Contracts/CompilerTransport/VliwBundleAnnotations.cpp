#include "VliwBundleAnnotations.h"
#include "../../Pipeline/Metadata/BundleMetadata.h"
#include "InstructionSlotMetadata.h"

namespace HybridCPU_ISE::Arch
{


	std::shared_ptr<VliwBundleAnnotations> VliwBundleAnnotations::getEmpty()
	{
		static const auto empty = std::make_shared<VliwBundleAnnotations>(std::vector<std::shared_ptr<InstructionSlotMetadata>>{}, YAKSys_Hybrid_CPU::Core::BundleMetadata::GetDefault());
		return empty;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> VliwBundleAnnotations::getBundleMetadata() const
	{
		return BundleMetadata;
	}

	VliwBundleAnnotations::VliwBundleAnnotations(const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata) : VliwBundleAnnotations(slotMetadata, BuildBundleMetadata(slotMetadata))
	{
	}

	VliwBundleAnnotations::VliwBundleAnnotations(const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata, std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> bundleMetadata) : BundleMetadata(std::move(bundleMetadata)), _slotMetadata(slotMetadata)
	{
		if (!BundleMetadata)
		{
			throw std::invalid_argument("bundleMetadata");
		}
	}

	int VliwBundleAnnotations::getCount() const
	{
		return _slotMetadata.size();
	}

	bool VliwBundleAnnotations::TryGetInstructionSlotMetadata(int instructionIndex, std::shared_ptr<InstructionSlotMetadata> &metadata)
	{
		if (static_cast<unsigned int>(instructionIndex) < static_cast<unsigned int>(_slotMetadata.size()))
		{
			metadata = _slotMetadata[instructionIndex];
			return true;
		}

		metadata = nullptr;
		return false;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> VliwBundleAnnotations::BuildBundleMetadata(const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata)
	{
		if (slotMetadata.empty())
		{
			return YAKSys_Hybrid_CPU::Core::BundleMetadata::GetDefault();
		}

		std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Core::SlotMetadata>> slots(slotMetadata.size());
		bool hasExplicitMetadata = false;
		for (int index = 0; index < slotMetadata.size(); index++)
		{
			std::shared_ptr<YAKSys_Hybrid_CPU::Core::SlotMetadata> metadata = slotMetadata[index]->SlotMetadata;
			slots[index] = metadata;
			hasExplicitMetadata |= metadata != YAKSys_Hybrid_CPU::Core::SlotMetadata::getDefault();
		}

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> temp_var = std::make_shared<YAKSys_Hybrid_CPU::Core::BundleMetadata>();
		temp_var->SlotMetadata = std::make_shared<std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Core::SlotMetadata>>>(std::move(slots));
		return hasExplicitMetadata ? temp_var : YAKSys_Hybrid_CPU::Core::BundleMetadata::GetDefault();
	}
}
