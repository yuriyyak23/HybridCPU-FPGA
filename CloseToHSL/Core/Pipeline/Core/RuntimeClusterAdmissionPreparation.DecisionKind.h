#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	enum class RuntimeClusterAdmissionDecisionKind : unsigned char
	{
		Empty = 0,
		AdvisoryReferenceSequential = 1,
		AdvisoryClusterCandidate = 2,
		AdvisoryAuxiliaryOnly = 3
	};
}
