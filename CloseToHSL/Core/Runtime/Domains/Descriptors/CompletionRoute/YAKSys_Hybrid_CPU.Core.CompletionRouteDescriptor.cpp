#include "YAKSys_Hybrid_CPU.Core.CompletionRouteDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CompletionRouteDescriptor::CompletionRouteDescriptor() : CompletionRouteDescriptor(CompletionRouteAuthority::Runtime, 0, true, true)
	{
	}

	CompletionRouteDescriptor::CompletionRouteDescriptor(CompletionRouteAuthority authority, unsigned long long enabledSourceMask, bool requiresPostedEventQueue, bool allowsCompatibilityProjection) : Authority(authority), EnabledSourceMask(enabledSourceMask), RequiresPostedEventQueue(requiresPostedEventQueue), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	CompletionRouteAuthority CompletionRouteDescriptor::getAuthority() const
	{
		return Authority;
	}

	unsigned long long CompletionRouteDescriptor::getEnabledSourceMask() const
	{
		return EnabledSourceMask;
	}

	bool CompletionRouteDescriptor::getRequiresPostedEventQueue() const
	{
		return RequiresPostedEventQueue;
	}

	bool CompletionRouteDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool CompletionRouteDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == CompletionRouteAuthority::Runtime;
	}

	bool CompletionRouteDescriptor::getHasEnabledRoutes() const
	{
		return getEnabledSourceMask() != 0;
	}

	bool CompletionRouteDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	bool CompletionRouteDescriptor::AllowsSource(LaneCompletionSourceKind sourceKind)
	{
		unsigned char bit = static_cast<unsigned char>(sourceKind);
		return bit != 0 && bit < 64 && (getEnabledSourceMask() & (1ULL << bit)) != 0;
	}

	std::shared_ptr<CompletionRouteDescriptor> CompletionRouteDescriptor::WithEnabledSourceMask(unsigned long long enabledSourceMask)
	{
		return std::make_shared<CompletionRouteDescriptor>(getAuthority(), enabledSourceMask, getRequiresPostedEventQueue(), getAllowsCompatibilityProjection());
	}
}
