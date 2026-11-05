#include "DecodeState.h"

namespace YAKSys_Hybrid_CPU::Core
{
	DecodeState::DecodeState()
		: Decode{}, PipelineBundleSlot{}, BundleRuntime{},
		  BundleProgress(BundleProgressState::CreateEmpty()),
		  DerivedIssuePlan(DecodedBundleDerivedIssuePlanState::CreateEmpty())
	{
	}
}
