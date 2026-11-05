#include "YAKSys_Hybrid_CPU.Core.Lane7AcceleratorDescriptor.h"


namespace YAKSys_Hybrid_CPU::Core
{

	Lane7AcceleratorDescriptor::Lane7AcceleratorDescriptor() : Lane7AcceleratorDescriptor(Lane7AcceleratorAuthority::Runtime, 7, 0, 0, 0, 0, true, true)
	{
	}

	Lane7AcceleratorDescriptor::Lane7AcceleratorDescriptor(Lane7AcceleratorAuthority authority, unsigned char laneId, unsigned long long backendBindingId, unsigned long long handleNamespaceId, unsigned long long tokenNamespaceId, unsigned long long completionRouteId, bool requiresRuntimeBackendBinding, bool allowsCompatibilityProjection) : Authority(authority), LaneId(laneId), BackendBindingId(backendBindingId), HandleNamespaceId(handleNamespaceId), TokenNamespaceId(tokenNamespaceId), CompletionRouteId(completionRouteId), RequiresRuntimeBackendBinding(requiresRuntimeBackendBinding), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	Lane7AcceleratorAuthority Lane7AcceleratorDescriptor::getAuthority() const
	{
		return Authority;
	}

	unsigned char Lane7AcceleratorDescriptor::getLaneId() const
	{
		return LaneId;
	}

	unsigned long long Lane7AcceleratorDescriptor::getBackendBindingId() const
	{
		return BackendBindingId;
	}

	unsigned long long Lane7AcceleratorDescriptor::getHandleNamespaceId() const
	{
		return HandleNamespaceId;
	}

	unsigned long long Lane7AcceleratorDescriptor::getTokenNamespaceId() const
	{
		return TokenNamespaceId;
	}

	unsigned long long Lane7AcceleratorDescriptor::getCompletionRouteId() const
	{
		return CompletionRouteId;
	}

	bool Lane7AcceleratorDescriptor::getRequiresRuntimeBackendBinding() const
	{
		return RequiresRuntimeBackendBinding;
	}

	bool Lane7AcceleratorDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool Lane7AcceleratorDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == Lane7AcceleratorAuthority::Runtime;
	}

	bool Lane7AcceleratorDescriptor::isPinnedToLane7() const
	{
		return getLaneId() == 7;
	}

	bool Lane7AcceleratorDescriptor::getHasBackendBinding() const
	{
		return getBackendBindingId() != 0;
	}

	bool Lane7AcceleratorDescriptor::getHasNamespaceBinding() const
	{
		return getHandleNamespaceId() != 0 || getTokenNamespaceId() != 0 || getCompletionRouteId() != 0;
	}

	bool Lane7AcceleratorDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	std::shared_ptr<Lane7AcceleratorDescriptor> Lane7AcceleratorDescriptor::WithBackendBinding(unsigned long long backendBindingId, unsigned long long handleNamespaceId, unsigned long long tokenNamespaceId, unsigned long long completionRouteId)
	{
		return std::make_shared<Lane7AcceleratorDescriptor>(getAuthority(), getLaneId(), backendBindingId, handleNamespaceId, tokenNamespaceId, completionRouteId, getRequiresRuntimeBackendBinding(), getAllowsCompatibilityProjection());
	}
}
