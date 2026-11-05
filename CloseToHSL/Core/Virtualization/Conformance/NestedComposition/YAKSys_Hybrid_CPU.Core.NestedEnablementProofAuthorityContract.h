#pragma once

#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"

#include "NestedEnablementProofAuthorityContract.h"
#include <memory>


namespace YAKSys_Hybrid_CPU::Core
{
	class NestedEnablementProofAuthorityContract final : public std::enable_shared_from_this<NestedEnablementProofAuthorityContract>
	{
	public:
		NestedEnablementProofAuthorityViolation Evaluate(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedEnablementProof> proof);

		bool IsSatisfied(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedEnablementProof> proof);
	};
}


