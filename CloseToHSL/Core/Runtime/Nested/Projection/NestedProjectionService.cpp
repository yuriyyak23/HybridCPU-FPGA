#include "NestedProjectionService.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<NestedProjectionResult> NestedProjectionResult::Allowed =
		std::make_shared<NestedProjectionResult>(NestedProjectionDecision::Allowed, L"Nested compatibility projection allowed.");
}
