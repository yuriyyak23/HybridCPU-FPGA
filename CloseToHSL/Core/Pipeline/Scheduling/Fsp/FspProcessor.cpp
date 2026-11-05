#include "FspProcessor.h"
#include "../../Metadata/BundleMetadata.h"
#include "../../MicroOps/Types/MicroOp.Misc.h"

namespace YAKSys_Hybrid_CPU::Core
{

	FspPilferDecision::FspPilferDecision(int slotIndex, int donorVtId) : SlotIndex(slotIndex), DonorVtId(donorVtId)
	{
	}

	const std::vector<FspPilferDecision> FspResult::getPilferDecisions() const
	{
		return _decisions;
	}

	bool FspResult::getHasPilfers() const
	{
		return _decisions.size() > 0;
	}

	int FspResult::getPilferCount() const
	{
		return _decisions.size();
	}

	void FspResult::AddPilfer(int slotIndex, int donorVtId)
	{
		_decisions.push_back(FspPilferDecision(slotIndex, donorVtId));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public FspResult TryPilfer(MicroOp?[] bundle, BundleMetadata bundleMeta)
	std::shared_ptr<FspResult> FspProcessor::TryPilfer(std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleMetadata> bundleMeta)
	{
		std::shared_ptr<FspResult> result = std::make_shared<FspResult>();

		if (bundle.empty() || bundleMeta == nullptr)
		{
			return result;
		}

		if (bundleMeta->FspBoundary)
		{
			return result;
		}

		for (int slotIdx = 0; slotIdx < bundle.size(); slotIdx++)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? existing = bundle[slotIdx];
			std::shared_ptr<MicroOp> existing = bundle[slotIdx];
			if (existing != nullptr && std::dynamic_pointer_cast<NopMicroOp>(existing) == nullptr)
			{
				continue;
			}

			std::shared_ptr<SlotMetadata> slotMeta = bundleMeta->GetSlotMetadata(slotIdx);
			if (slotMeta->StealabilityPolicy != StealabilityPolicy::Stealable)
			{
				continue;
			}

			int donorVt = slotMeta->DonorVtHint != 0xFF ? slotMeta->DonorVtHint : SelectLowestAvailableVt(bundle, slotIdx);

			if (donorVt == 0xFF)
			{
				continue;
			}

			result->AddPilfer(slotIdx, donorVt);
		}

		return result;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static int SelectLowestAvailableVt(MicroOp?[] bundle, int excludeSlot)
	int FspProcessor::SelectLowestAvailableVt(std::vector<std::shared_ptr<MicroOp>> &bundle, int excludeSlot)
	{
		int lowestVt = 0xFF;
		for (int i = 0; i < bundle.size(); i++)
		{
			if (i == excludeSlot)
			{
				continue;
			}
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? op = bundle[i];
			std::shared_ptr<MicroOp> op = bundle[i];
			if (op == nullptr || std::dynamic_pointer_cast<NopMicroOp>(op) != nullptr)
			{
				continue;
			}
			int vtId = op->getVirtualThreadId();
			if (vtId < lowestVt)
			{
				lowestVt = vtId;
			}
		}

		return lowestVt;
	}
}


