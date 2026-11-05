#include "YAKSys_Hybrid_CPU.Core.NestedEnablementProofAuthorityContract.h"


namespace YAKSys_Hybrid_CPU::Core
{

	NestedEnablementProofAuthorityViolation NestedEnablementProofAuthorityContract::Evaluate(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedEnablementProof> proof)
	{
		if (!proof->HasRequiredCapabilities)
		{
			return NestedEnablementProofAuthorityViolation::MissingRequiredCapabilityProof;
		}

		if (!proof->HasRequiredGates)
		{
			return NestedEnablementProofAuthorityViolation::MissingRequiredGateProof;
		}

		return proof->IsAuthoritative ? NestedEnablementProofAuthorityViolation::None : NestedEnablementProofAuthorityViolation::NonAuthoritativeProof;
	}

	bool NestedEnablementProofAuthorityContract::IsSatisfied(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedEnablementProof> proof)
	{
		return Evaluate(proof) == NestedEnablementProofAuthorityViolation::None;
	}
}

