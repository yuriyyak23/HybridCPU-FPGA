#include "YAKSys_Hybrid_CPU.Core.EventTrapDomainIdentityAuthorityRemovalContract.h"
#include "../../../Runtime/Events/Injection/EventInjectionDescriptor.h"
#include "../../../Runtime/Events/Traps/YAKSys_Hybrid_CPU.Core.TrapRequest.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> EventTrapDomainIdentityAuthorityRemovalContract::getExecutableEventTrapPaths()
	{
		return ExecutableEventTrapPaths;
	}

	std::vector<std::wstring> EventTrapDomainIdentityAuthorityRemovalContract::getForbiddenExecutableIdentityMarkers()
	{
		return ForbiddenExecutableIdentityMarkers;
	}

	std::vector<std::wstring> EventTrapDomainIdentityAuthorityRemovalContract::getForbiddenCompatibilityEventHelperMarkers()
	{
		return ForbiddenCompatibilityEventHelperMarkers;
	}

	bool EventTrapDomainIdentityAuthorityRemovalContract::IsNeutralIdentity(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<TrapRequest> request, unsigned short executionDomainTag, unsigned short addressSpaceTag)
	{
		return descriptor->ExecutionDomainTag == executionDomainTag && descriptor->AddressSpaceTag == addressSpaceTag && request->ExecutionDomainTag == executionDomainTag && request->AddressSpaceTag == addressSpaceTag;
	}
}
