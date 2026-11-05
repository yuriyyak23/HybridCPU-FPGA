#pragma once

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class Lane7AcceleratorAuthority : unsigned char
	{
		Runtime = 0,
		DomainDescriptor = 1,
		CompatibilityProjection = 2
	};

	class Lane7AcceleratorDescriptor final : public std::enable_shared_from_this<Lane7AcceleratorDescriptor>
	{
	private:
		const Lane7AcceleratorAuthority Authority;
		const unsigned char LaneId;
		const unsigned long long BackendBindingId;
		const unsigned long long HandleNamespaceId;
		const unsigned long long TokenNamespaceId;
		const unsigned long long CompletionRouteId;
		const bool RequiresRuntimeBackendBinding;
		const bool AllowsCompatibilityProjection;

	public:
		Lane7AcceleratorDescriptor();
		Lane7AcceleratorDescriptor(Lane7AcceleratorAuthority authority, unsigned char laneId, unsigned long long backendBindingId, unsigned long long handleNamespaceId, unsigned long long tokenNamespaceId, unsigned long long completionRouteId, bool requiresRuntimeBackendBinding, bool allowsCompatibilityProjection);

		Lane7AcceleratorAuthority getAuthority() const;
		unsigned char getLaneId() const;
		unsigned long long getBackendBindingId() const;
		unsigned long long getHandleNamespaceId() const;
		unsigned long long getTokenNamespaceId() const;
		unsigned long long getCompletionRouteId() const;
		bool getRequiresRuntimeBackendBinding() const;
		bool getAllowsCompatibilityProjection() const;
		bool isRuntimeAuthoritative() const;
		bool isPinnedToLane7() const;
		bool getHasBackendBinding() const;
		bool getHasNamespaceBinding() const;
		bool getCanProjectToCompatibilityFrontend() const;
		std::shared_ptr<Lane7AcceleratorDescriptor> WithBackendBinding(unsigned long long backendBindingId, unsigned long long handleNamespaceId, unsigned long long tokenNamespaceId, unsigned long long completionRouteId);
	};
}
