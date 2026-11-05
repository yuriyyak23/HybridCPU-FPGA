#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NestedProofAuthoritySourceViolation : unsigned char
	{
		None = 0,
		MissingDescriptorBackedProof = 1,
		MissingEvidencePolicyProof = 2,
		CompatibilityAliasUsedAsAuthority = 3,
		MissingRuntimeValidation = 4
	};


}
