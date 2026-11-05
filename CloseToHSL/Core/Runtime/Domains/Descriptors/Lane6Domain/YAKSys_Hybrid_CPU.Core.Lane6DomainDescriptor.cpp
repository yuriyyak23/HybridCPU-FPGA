#include "YAKSys_Hybrid_CPU.Core.Lane6DomainDescriptor.h"

#include <utility>
#include "../../../../Virtualization/CompilerBoundary/LaneBinding/YAKSys_Hybrid_CPU.Core.VirtualizationLaneBindingPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	Lane6DomainDescriptor::Lane6DomainDescriptor() : Lane6DomainDescriptor(Lane6DomainAuthority::Runtime, VirtualizationLaneBindingPolicy::Lane6Id, 0, 0, 0, true)
	{
	}

	Lane6DomainDescriptor::Lane6DomainDescriptor(Lane6DomainAuthority authority, unsigned char laneId, unsigned long long tokenNamespaceId, unsigned long long queueNamespaceId, unsigned long long fenceDomainId, bool allowsCompatibilityProjection)
		: Authority(authority),
		  LaneId(laneId),
		  TokenNamespaceId(tokenNamespaceId),
		  QueueNamespaceId(queueNamespaceId),
		  FenceDomainId(fenceDomainId),
		  AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	Lane6DomainAuthority Lane6DomainDescriptor::getAuthority() const
	{
		return Authority;
	}

	unsigned char Lane6DomainDescriptor::getLaneId() const
	{
		return LaneId;
	}

	unsigned long long Lane6DomainDescriptor::getTokenNamespaceId() const
	{
		return TokenNamespaceId;
	}

	unsigned long long Lane6DomainDescriptor::getQueueNamespaceId() const
	{
		return QueueNamespaceId;
	}

	unsigned long long Lane6DomainDescriptor::getFenceDomainId() const
	{
		return FenceDomainId;
	}

	bool Lane6DomainDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool Lane6DomainDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == Lane6DomainAuthority::Runtime;
	}

	bool Lane6DomainDescriptor::isPinnedToLane6() const
	{
		return getLaneId() == VirtualizationLaneBindingPolicy::Lane6Id;
	}

	bool Lane6DomainDescriptor::getHasNamespaceBinding() const
	{
		return getTokenNamespaceId() != 0 || getQueueNamespaceId() != 0 || getFenceDomainId() != 0;
	}

	bool Lane6DomainDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	std::shared_ptr<Lane6DomainDescriptor> Lane6DomainDescriptor::WithNamespaceBinding(unsigned long long tokenNamespaceId, unsigned long long queueNamespaceId, unsigned long long fenceDomainId)
	{
		return std::make_shared<Lane6DomainDescriptor>(getAuthority(), getLaneId(), tokenNamespaceId, queueNamespaceId, fenceDomainId, getAllowsCompatibilityProjection());
	}
}
