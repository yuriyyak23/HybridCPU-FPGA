#pragma once

#include "NestedMemoryDomainComposer.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedMemoryDomainComposer final : public std::enable_shared_from_this<NestedMemoryDomainComposer>
	{
	public:
		std::shared_ptr<NestedMemoryDomainCompositionResult> Validate(std::shared_ptr<NestedMemoryDomainCompositionRequest> request);

		bool CanCompose(std::shared_ptr<NestedMemoryDomainCompositionRequest> request);
	};
}
