#include "YAKSys_Hybrid_CPU.Core.Lane6TokenNamespace.h"

namespace YAKSys_Hybrid_CPU::Core
{

	Lane6TokenNamespace::Lane6TokenNamespace() : Lane6TokenNamespace(Lane6TokenNamespaceAuthority::Runtime, 0, Lane6TokenVisibility::VirtualTokens, true)
	{
	}

	Lane6TokenNamespace::Lane6TokenNamespace(Lane6TokenNamespaceAuthority authority, unsigned long long namespaceId, Lane6TokenVisibility visibility, bool allowsCompatibilityProjection) : Authority(authority), NamespaceId(namespaceId), Visibility(visibility), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	Lane6TokenNamespaceAuthority Lane6TokenNamespace::getAuthority() const
	{
		return Authority;
	}

	unsigned long long Lane6TokenNamespace::getNamespaceId() const
	{
		return NamespaceId;
	}

	Lane6TokenVisibility Lane6TokenNamespace::getVisibility() const
	{
		return Visibility;
	}

	bool Lane6TokenNamespace::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool Lane6TokenNamespace::isRuntimeAuthoritative() const
	{
		return getAuthority() == Lane6TokenNamespaceAuthority::Runtime;
	}

	bool Lane6TokenNamespace::getHasNamespaceBinding() const
	{
		return getNamespaceId() != 0;
	}

	bool Lane6TokenNamespace::getExposesNativeTokens() const
	{
		return getVisibility() == Lane6TokenVisibility::NativeTokens;
	}

	bool Lane6TokenNamespace::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative() && !getExposesNativeTokens();
	}

	std::shared_ptr<Lane6TokenNamespace> Lane6TokenNamespace::WithNamespaceBinding(unsigned long long namespaceId)
	{
		return std::make_shared<Lane6TokenNamespace>(getAuthority(), namespaceId, getVisibility(), getAllowsCompatibilityProjection());
	}
}
