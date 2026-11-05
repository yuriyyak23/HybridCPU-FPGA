#include "FspAdmissionPolicy.h"

using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Scheduling
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SlotAssignment Evaluate(IReadOnlyList<System.Nullable<InstructionIR>> bundle, BundleMetadata bundleMeta, int donorVtId)
	std::shared_ptr<SlotAssignment> FspAdmissionPolicy::Evaluate(const std::vector<std::optional<std::shared_ptr<InstructionIR>>> &bundle, std::shared_ptr<BundleMetadata> bundleMeta, int donorVtId)
	{
		if (bundleMeta == nullptr)
		{
			throw std::invalid_argument("bundleMeta");
		}

		if (bundle.empty() || bundle.empty())
		{
			return SlotAssignment::Empty;
		}

		if (bundleMeta->FspBoundary)
		{
			return SlotAssignment::Empty;
		}

		std::shared_ptr<SlotAssignment> result = std::make_shared<SlotAssignment>();

		for (int i = 0; i < bundle.size(); i++)
		{
			if (bundle[i])
			{
				continue;
			}

			std::shared_ptr<SlotMetadata> slotMeta = bundleMeta->GetSlotMetadata(i);
			if (slotMeta->StealabilityPolicy != StealabilityPolicy::Stealable)
			{
				continue;
			}

			result->Add(i, donorVtId);
		}

		return result;
	}
}
