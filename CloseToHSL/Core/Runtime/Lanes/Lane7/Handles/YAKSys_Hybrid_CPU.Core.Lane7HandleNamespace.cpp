#include "YAKSys_Hybrid_CPU.Core.Lane7HandleNamespace.h"

namespace YAKSys_Hybrid_CPU::Core
{

	Lane7HandleNamespace::Lane7HandleNamespace() : Lane7HandleNamespace(Lane7HandleNamespaceAuthority::Runtime, 0, Lane7HandleVisibility::VirtualHandles, true)
	{
	}

	Lane7HandleNamespace::Lane7HandleNamespace(Lane7HandleNamespaceAuthority authority, unsigned long long namespaceId, Lane7HandleVisibility visibility, bool allowsCompatibilityProjection)
		: Authority(authority), NamespaceId(namespaceId), Visibility(visibility), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	Lane7HandleNamespaceAuthority Lane7HandleNamespace::getAuthority() const
	{
		return Authority;
	}

	unsigned long long Lane7HandleNamespace::getNamespaceId() const
	{
		return NamespaceId;
	}

	Lane7HandleVisibility Lane7HandleNamespace::getVisibility() const
	{
		return Visibility;
	}

	bool Lane7HandleNamespace::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool Lane7HandleNamespace::isRuntimeAuthoritative() const
	{
		return getAuthority() == Lane7HandleNamespaceAuthority::Runtime;
	}

	bool Lane7HandleNamespace::getHasNamespaceBinding() const
	{
		return getNamespaceId() != 0;
	}

	bool Lane7HandleNamespace::getExposesNativeBackendHandles() const
	{
		return getVisibility() == Lane7HandleVisibility::NativeBackendHandles;
	}

	bool Lane7HandleNamespace::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative() && !getExposesNativeBackendHandles();
	}

	std::shared_ptr<Lane7HandleNamespace> Lane7HandleNamespace::WithNamespaceBinding(unsigned long long namespaceId)
	{
		return std::make_shared<Lane7HandleNamespace>(getAuthority(), namespaceId, getVisibility(), getAllowsCompatibilityProjection());
	}
}
