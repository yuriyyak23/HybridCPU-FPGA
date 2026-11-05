#pragma once

#include "Lane6QueueNamespace.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane6QueueNamespace final : public std::enable_shared_from_this<Lane6QueueNamespace>
	{
	private:
		const Lane6QueueNamespaceAuthority Authority;
		const unsigned long long NamespaceId;
		const Lane6QueueVisibility Visibility;
		const bool AllowsCompatibilityProjection;

	public:
		Lane6QueueNamespace();

		Lane6QueueNamespace(Lane6QueueNamespaceAuthority authority, unsigned long long namespaceId, Lane6QueueVisibility visibility, bool allowsCompatibilityProjection);

		Lane6QueueNamespaceAuthority getAuthority() const;

		unsigned long long getNamespaceId() const;

		Lane6QueueVisibility getVisibility() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasNamespaceBinding() const;

		bool getExposesNativeQueueHandles() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<Lane6QueueNamespace> WithNamespaceBinding(unsigned long long namespaceId);
	};
}
