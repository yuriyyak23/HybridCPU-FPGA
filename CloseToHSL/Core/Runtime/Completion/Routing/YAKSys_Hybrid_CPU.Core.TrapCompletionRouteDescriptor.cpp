#include "YAKSys_Hybrid_CPU.Core.TrapCompletionRouteDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	TrapCompletionRouteDescriptor::TrapCompletionRouteDescriptor() : TrapCompletionRouteDescriptor(CompletionRouteAuthority::Runtime, false, false, true)
	{
	}

	TrapCompletionRouteDescriptor::TrapCompletionRouteDescriptor(CompletionRouteAuthority authority, bool allowsCompletionPublication, bool allowsRetirePublication, bool requiresValidatedDomain) : Authority(authority), AllowsCompletionPublication(allowsCompletionPublication), AllowsRetirePublication(allowsRetirePublication), RequiresValidatedDomain(requiresValidatedDomain) {}

	std::shared_ptr<TrapCompletionRouteDescriptor> TrapCompletionRouteDescriptor::getProjectionOnlyDenied()
	{
		return ProjectionOnlyDenied;
	}

	std::shared_ptr<TrapCompletionRouteDescriptor> TrapCompletionRouteDescriptor::getRuntimeOwnedCompletionPublication()
	{
		return RuntimeOwnedCompletionPublication;
	}

	std::shared_ptr<TrapCompletionRouteDescriptor> TrapCompletionRouteDescriptor::getRuntimeOwnedPublication()
	{
		return RuntimeOwnedPublication;
	}

	CompletionRouteAuthority TrapCompletionRouteDescriptor::getAuthority() const
	{
		return Authority;
	}

	bool TrapCompletionRouteDescriptor::getAllowsCompletionPublication() const
	{
		return AllowsCompletionPublication;
	}

	bool TrapCompletionRouteDescriptor::getAllowsRetirePublication() const
	{
		return AllowsRetirePublication;
	}

	bool TrapCompletionRouteDescriptor::getRequiresValidatedDomain() const
	{
		return RequiresValidatedDomain;
	}

	bool TrapCompletionRouteDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == CompletionRouteAuthority::Runtime;
	}
}
