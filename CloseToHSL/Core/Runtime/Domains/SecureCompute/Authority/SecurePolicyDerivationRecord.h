#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecurePolicyDerivationDecision : unsigned char
	{
		NotEvaluated = 0,
		AllowedSubset = 1,
		ParentMissing = 2,
		ChildExpandsAuthority = 3,
		EpochMismatch = 4,
		ProvenanceMissing = 5
	};


}
