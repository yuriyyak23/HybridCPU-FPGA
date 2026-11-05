#pragma once

#include "Lane7TokenNamespace.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7TokenNamespace final : public std::enable_shared_from_this<Lane7TokenNamespace>
	{
	private:
		const Lane7TokenNamespaceAuthority Authority;
		const unsigned long long NamespaceId;
		const Lane7TokenVisibility Visibility;
		const bool AllowsCompatibilityProjection;

	public:
		Lane7TokenNamespace();

		Lane7TokenNamespace(Lane7TokenNamespaceAuthority authority, unsigned long long namespaceId, Lane7TokenVisibility visibility, bool allowsCompatibilityProjection);

		Lane7TokenNamespaceAuthority getAuthority() const;

		unsigned long long getNamespaceId() const;

		Lane7TokenVisibility getVisibility() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasNamespaceBinding() const;

		bool getExposesNativeTokens() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<Lane7TokenNamespace> WithNamespaceBinding(unsigned long long namespaceId);
	};
}
