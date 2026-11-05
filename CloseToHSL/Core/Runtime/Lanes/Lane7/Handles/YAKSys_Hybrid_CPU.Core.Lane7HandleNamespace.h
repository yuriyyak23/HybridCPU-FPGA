#pragma once

#include "Lane7HandleNamespace.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7HandleNamespace final : public std::enable_shared_from_this<Lane7HandleNamespace>
	{
	private:
		const Lane7HandleNamespaceAuthority Authority;
		const unsigned long long NamespaceId;
		const Lane7HandleVisibility Visibility;
		const bool AllowsCompatibilityProjection;

	public:
		Lane7HandleNamespace();

		Lane7HandleNamespace(Lane7HandleNamespaceAuthority authority, unsigned long long namespaceId, Lane7HandleVisibility visibility, bool allowsCompatibilityProjection);

		Lane7HandleNamespaceAuthority getAuthority() const;

		unsigned long long getNamespaceId() const;

		Lane7HandleVisibility getVisibility() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasNamespaceBinding() const;

		bool getExposesNativeBackendHandles() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<Lane7HandleNamespace> WithNamespaceBinding(unsigned long long namespaceId);
	};
}
