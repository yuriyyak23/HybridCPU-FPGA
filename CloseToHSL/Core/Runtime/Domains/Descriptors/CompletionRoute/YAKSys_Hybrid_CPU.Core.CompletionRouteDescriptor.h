#pragma once

#include "CompletionRouteDescriptor.h"
#include "../../../Completion/Routing/LaneCompletionRouting.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CompletionRouteDescriptor final : public std::enable_shared_from_this<CompletionRouteDescriptor>
	{
	private:
		const CompletionRouteAuthority Authority;
		const unsigned long long EnabledSourceMask;
		const bool RequiresPostedEventQueue;
		const bool AllowsCompatibilityProjection;

	public:
		CompletionRouteDescriptor();

		CompletionRouteDescriptor(CompletionRouteAuthority authority, unsigned long long enabledSourceMask, bool requiresPostedEventQueue, bool allowsCompatibilityProjection);

		CompletionRouteAuthority getAuthority() const;

		unsigned long long getEnabledSourceMask() const;

		bool getRequiresPostedEventQueue() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasEnabledRoutes() const;

		bool getCanProjectToCompatibilityFrontend() const;

		bool AllowsSource(LaneCompletionSourceKind sourceKind);

		std::shared_ptr<CompletionRouteDescriptor> WithEnabledSourceMask(unsigned long long enabledSourceMask);
	};
}
