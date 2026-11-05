#include "DeterministicLaneChooser.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeReplayEvidence.h"
#include "../SlotLegality/SlotClassDefinitions.h"
#include <bit>

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;

namespace YAKSys_Hybrid_CPU::Core
{

	int DeterministicLaneChooser::SelectLowestFree(unsigned char freeLanes)
	{
		if (freeLanes == 0)
		{
			return -1;
		}
		return static_cast<int>(std::countr_zero(freeLanes));
	}

	int DeterministicLaneChooser::SelectWithReplayHint(unsigned char freeLanes, bool replayActive, int previousLane)
	{
		if (freeLanes == 0)
		{
			return -1;
		}

		// Tier 2: replay lane reuse
		if (replayActive && previousLane >= 0 && previousLane < 8)
		{
			if ((freeLanes & (1 << previousLane)) != 0)
			{
				return previousLane;
			}
		}

		// Tier 1: lowest free
		return static_cast<int>(std::countr_zero(freeLanes));
	}

	bool DeterministicLaneChooser::TrySelectDmaStreamLaneWithReplayEvidence(unsigned char freeLanes, int previousLane, std::shared_ptr<DmaStreamComputeReplayEvidence> expected, std::shared_ptr<DmaStreamComputeReplayEvidence> live, int &selectedLane, ReplayPhaseInvalidationReason &invalidationReason)
	{
		selectedLane = -1;
		invalidationReason = ReplayPhaseInvalidationReason::None;

		std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> comparison = DmaStreamComputeReplayEvidenceComparer::Compare(expected, live);
		if (!comparison->CanReuse)
		{
			invalidationReason = comparison->InvalidationReason;
			return false;
		}

		unsigned char lane6Mask = SlotClassLaneMap::GetLaneMask(SlotClass::DmaStreamClass);
		unsigned char freeLane6 = static_cast<unsigned char>(freeLanes & lane6Mask);
		if (freeLane6 == 0)
		{
			invalidationReason = ReplayPhaseInvalidationReason::DmaStreamComputeLanePlacementMismatch;
			return false;
		}

		selectedLane = SelectWithReplayHint(freeLane6, true, previousLane);
		if (selectedLane == 6)
		{
			return true;
		}

		selectedLane = -1;
		invalidationReason = ReplayPhaseInvalidationReason::DmaStreamComputeLanePlacementMismatch;
		return false;
	}
}
