#include "YAKSys_Hybrid_CPU.Core.NestedProofAuthoritySourceContract.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	const NestedProofAuthoritySource NestedProofAuthoritySource::FailClosed(false, false,
		EvidenceVisibilityClass::HostOwnedRuntimeEvidence, false);
	const NestedProofAuthoritySource NestedProofAuthoritySource::RuntimePolicyValidated(true, true,
		EvidenceVisibilityClass::GuestArchitecturalState, true);
}
using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{

	NestedProofAuthoritySourceViolation NestedProofAuthoritySourceContract::Evaluate(std::shared_ptr<NestedProofAuthoritySource> source)
	{
		if (!source->DescriptorBacked)
		{
			return NestedProofAuthoritySourceViolation::MissingDescriptorBackedProof;
		}

		if (!source->EvidencePolicyBound)
		{
			return NestedProofAuthoritySourceViolation::MissingEvidencePolicyProof;
		}

		if (source->GateEvidenceClass == EvidenceVisibilityClass::CompatibilityAlias)
		{
			return NestedProofAuthoritySourceViolation::CompatibilityAliasUsedAsAuthority;
		}

		return source->RuntimeValidated ? NestedProofAuthoritySourceViolation::None : NestedProofAuthoritySourceViolation::MissingRuntimeValidation;
	}

	bool NestedProofAuthoritySourceContract::IsSatisfied(std::shared_ptr<NestedProofAuthoritySource> source)
	{
		return Evaluate(source) == NestedProofAuthoritySourceViolation::None;
	}
}

