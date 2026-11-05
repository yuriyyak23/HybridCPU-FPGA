#pragma once

#include "NestedExitMapper.h"
#include "../../Retire/VmxRetireModel.h"
#include <string>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Memory;


namespace YAKSys_Hybrid_CPU::Core::Nested
{
	class NestedExitMapper final
	{
	public:
		static std::shared_ptr<NestedExitMapping> FromIntercept(std::shared_ptr<NestedInterceptTranslationResult> intercept);

		static std::shared_ptr<NestedExitMapping> InvalidChildDomainIntent(unsigned short fieldId, const std::wstring &message);

		static std::shared_ptr<NestedExitMapping> InvalidVmcs12(unsigned short fieldId, const std::wstring &message);

		static std::shared_ptr<NestedExitMapping> UnsupportedOperation(VmxOperationKind operation);

		static std::shared_ptr<NestedExitMapping> VirtualInterrupt(std::shared_ptr<EventInjectionDescriptor> descriptor, bool reflectToL1);

		static std::shared_ptr<NestedExitMapping> PreemptionTimer(std::shared_ptr<TrapDecision> decision, bool reflectToL1);

		static std::shared_ptr<NestedExitMapping> LaneOperationBlocked(std::shared_ptr<TrapRequest> request);


		static std::shared_ptr<NestedExitMapping> FromNestedMemoryComposition(std::shared_ptr<NestedMemoryCompositionResult> result);
	};
}
