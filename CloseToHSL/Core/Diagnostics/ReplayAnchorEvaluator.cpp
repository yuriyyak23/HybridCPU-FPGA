#include "ReplayAnchorEvaluator.h"
#include "../Pipeline/Metadata/BundleMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{

	void ReplayAnchorEvaluator::NotifyFsmTransition(unsigned long long bundleSerial)
	{
		_lastFsmTransitionBundleSerial = bundleSerial;
	}

	std::optional<SnapshotTrigger> ReplayAnchorEvaluator::ShouldCaptureSnapshot(std::shared_ptr<BundleMetadata> meta, unsigned long long bundleSerial)
	{
		// Highest priority: explicit compiler anchor hint
		if (meta->IsReplayAnchor)
		{
			return SnapshotTrigger::AnchorHint;
		}

		// FSM transition at this bundle boundary
		if (_lastFsmTransitionBundleSerial == bundleSerial)
		{
			return SnapshotTrigger::FsmTransition;
		}

		// Periodic fallback — always capture regardless of compiler hints
		if (bundleSerial % PeriodicSnapshotInterval == 0)
		{
			return SnapshotTrigger::Periodic;
		}

		return std::nullopt;
	}

	void ReplayAnchorEvaluator::Reset()
	{
		_lastFsmTransitionBundleSerial = std::numeric_limits<unsigned long long>::max();
	}
}
