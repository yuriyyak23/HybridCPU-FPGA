#pragma once

#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"

#include "NestedProofAuthoritySourceContract.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedProofAuthoritySourceContract final : public std::enable_shared_from_this<NestedProofAuthoritySourceContract>
	{
	public:
		NestedProofAuthoritySourceViolation Evaluate(std::shared_ptr<NestedProofAuthoritySource> source);

		bool IsSatisfied(std::shared_ptr<NestedProofAuthoritySource> source);
	};
}

