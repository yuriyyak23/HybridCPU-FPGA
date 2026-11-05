#include "YAKSys_Hybrid_CPU.Core.Lane6QueueNamespace.h"

namespace YAKSys_Hybrid_CPU::Core
{

	Lane6QueueNamespace::Lane6QueueNamespace() : Lane6QueueNamespace(Lane6QueueNamespaceAuthority::Runtime, 0, Lane6QueueVisibility::VirtualQueueIds, true)
	{
	}

	Lane6QueueNamespace::Lane6QueueNamespace(Lane6QueueNamespaceAuthority authority, unsigned long long namespaceId, Lane6QueueVisibility visibility, bool allowsCompatibilityProjection) : Authority(authority), NamespaceId(namespaceId), Visibility(visibility), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	Lane6QueueNamespaceAuthority Lane6QueueNamespace::getAuthority() const
	{
		return Authority;
	}

	unsigned long long Lane6QueueNamespace::getNamespaceId() const
	{
		return NamespaceId;
	}

	Lane6QueueVisibility Lane6QueueNamespace::getVisibility() const
	{
		return Visibility;
	}

	bool Lane6QueueNamespace::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool Lane6QueueNamespace::isRuntimeAuthoritative() const
	{
		return getAuthority() == Lane6QueueNamespaceAuthority::Runtime;
	}

	bool Lane6QueueNamespace::getHasNamespaceBinding() const
	{
		return getNamespaceId() != 0;
	}

	bool Lane6QueueNamespace::getExposesNativeQueueHandles() const
	{
		return getVisibility() == Lane6QueueVisibility::NativeQueueHandles;
	}

	bool Lane6QueueNamespace::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative() && !getExposesNativeQueueHandles();
	}

	std::shared_ptr<Lane6QueueNamespace> Lane6QueueNamespace::WithNamespaceBinding(unsigned long long namespaceId)
	{
		return std::make_shared<Lane6QueueNamespace>(getAuthority(), namespaceId, getVisibility(), getAllowsCompatibilityProjection());
	}
}
