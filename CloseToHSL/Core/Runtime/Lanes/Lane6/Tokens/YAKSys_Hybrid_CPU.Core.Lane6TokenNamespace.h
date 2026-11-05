#pragma once

#include "Lane6TokenNamespace.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane6TokenNamespace final : public std::enable_shared_from_this<Lane6TokenNamespace>
	{
	private:
		const Lane6TokenNamespaceAuthority Authority;
		const unsigned long long NamespaceId;
		const Lane6TokenVisibility Visibility;
		const bool AllowsCompatibilityProjection;

	public:
		Lane6TokenNamespace();

		Lane6TokenNamespace(Lane6TokenNamespaceAuthority authority, unsigned long long namespaceId, Lane6TokenVisibility visibility, bool allowsCompatibilityProjection);

		Lane6TokenNamespaceAuthority getAuthority() const;

		unsigned long long getNamespaceId() const;

		Lane6TokenVisibility getVisibility() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasNamespaceBinding() const;

		bool getExposesNativeTokens() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<Lane6TokenNamespace> WithNamespaceBinding(unsigned long long namespaceId);
	};
}
