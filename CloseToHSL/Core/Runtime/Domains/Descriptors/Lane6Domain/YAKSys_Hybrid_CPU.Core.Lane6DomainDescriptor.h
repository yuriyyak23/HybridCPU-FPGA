#pragma once

#include "Lane6DomainDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane6DomainDescriptor final : public std::enable_shared_from_this<Lane6DomainDescriptor>
	{
	private:
		const Lane6DomainAuthority Authority;
		const unsigned char LaneId;
		const unsigned long long TokenNamespaceId;
		const unsigned long long QueueNamespaceId;
		const unsigned long long FenceDomainId;
		const bool AllowsCompatibilityProjection;

	public:
		Lane6DomainDescriptor();

		Lane6DomainDescriptor(Lane6DomainAuthority authority, unsigned char laneId, unsigned long long tokenNamespaceId, unsigned long long queueNamespaceId, unsigned long long fenceDomainId, bool allowsCompatibilityProjection);

		Lane6DomainAuthority getAuthority() const;

		unsigned char getLaneId() const;

		unsigned long long getTokenNamespaceId() const;

		unsigned long long getQueueNamespaceId() const;

		unsigned long long getFenceDomainId() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool isPinnedToLane6() const;

		bool getHasNamespaceBinding() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<Lane6DomainDescriptor> WithNamespaceBinding(unsigned long long tokenNamespaceId, unsigned long long queueNamespaceId, unsigned long long fenceDomainId);
	};
}
