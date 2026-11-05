#include "YAKSys_Hybrid_CPU.Core.Lane7TokenNamespace.h"

namespace YAKSys_Hybrid_CPU::Core
{

	Lane7TokenNamespace::Lane7TokenNamespace() : Lane7TokenNamespace(Lane7TokenNamespaceAuthority::Runtime, 0, Lane7TokenVisibility::VirtualTokens, true)
	{
	}

	Lane7TokenNamespace::Lane7TokenNamespace(Lane7TokenNamespaceAuthority authority, unsigned long long namespaceId, Lane7TokenVisibility visibility, bool allowsCompatibilityProjection)
		: Authority(authority), NamespaceId(namespaceId), Visibility(visibility), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	Lane7TokenNamespaceAuthority Lane7TokenNamespace::getAuthority() const
	{
		return Authority;
	}

	unsigned long long Lane7TokenNamespace::getNamespaceId() const
	{
		return NamespaceId;
	}

	Lane7TokenVisibility Lane7TokenNamespace::getVisibility() const
	{
		return Visibility;
	}

	bool Lane7TokenNamespace::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool Lane7TokenNamespace::isRuntimeAuthoritative() const
	{
		return getAuthority() == Lane7TokenNamespaceAuthority::Runtime;
	}

	bool Lane7TokenNamespace::getHasNamespaceBinding() const
	{
		return getNamespaceId() != 0;
	}

	bool Lane7TokenNamespace::getExposesNativeTokens() const
	{
		return getVisibility() == Lane7TokenVisibility::NativeTokens;
	}

	bool Lane7TokenNamespace::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative() && !getExposesNativeTokens();
	}

	std::shared_ptr<Lane7TokenNamespace> Lane7TokenNamespace::WithNamespaceBinding(unsigned long long namespaceId)
	{
		return std::make_shared<Lane7TokenNamespace>(getAuthority(), namespaceId, getVisibility(), getAllowsCompatibilityProjection());
	}
}
