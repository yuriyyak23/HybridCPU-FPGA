#pragma once

#include "MemoryTranslationAuthorityContract.h"
#include "../../Compatibility/Frontend/Projection/Memory/MemoryTranslationControl.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class MemoryTranslationAuthorityContract final : public std::enable_shared_from_this<MemoryTranslationAuthorityContract>
	{
	public:
		MemoryTranslationAuthorityViolation Evaluate(MemoryTranslationAuthorityRequest request);

		bool IsSatisfied(MemoryTranslationAuthorityRequest request);

		bool IsSatisfied(MemoryTranslationControl control);

		MemoryTranslationAuthorityViolation EvaluateDomainControl(MemoryTranslationControl control);
	};
}
