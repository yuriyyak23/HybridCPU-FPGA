#pragma once

#include "NestedInterceptTranslator.h"
#include "../../../../../Runtime/Events/Traps/TrapPolicyBitmap.h"
#include "../../../../../Runtime/Events/Traps/TrapRequest.h"
#include "../../../../../Runtime/Events/Traps/YAKSys_Hybrid_CPU.Core.TrapPolicyBitmap.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	class NestedInterceptTranslator final
	{
	private:
		static TrapPolicyClass ToClass(TrapTargetKind targetKind);


	public:
		static std::shared_ptr<NestedInterceptTranslationResult> Translate(std::shared_ptr<TrapRequest> request, std::shared_ptr<TrapPolicyBitmap> l1RequestedIntercepts, std::shared_ptr<TrapPolicyBitmap> l0MandatoryIntercepts, std::shared_ptr<NestedInterceptPolicy> policy);
	};
}
