#pragma once

#include "../../Domains/Descriptors/CompletionRoute/CompletionRouteDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class TrapCompletionRouteDescriptor final : public std::enable_shared_from_this<TrapCompletionRouteDescriptor>
	{
	private:
		static inline const std::shared_ptr<TrapCompletionRouteDescriptor> ProjectionOnlyDenied = std::make_shared<TrapCompletionRouteDescriptor>(CompletionRouteAuthority::Runtime, false, false, false);
		static inline const std::shared_ptr<TrapCompletionRouteDescriptor> RuntimeOwnedCompletionPublication = std::make_shared<TrapCompletionRouteDescriptor>(CompletionRouteAuthority::Runtime, true, false, true);
		static inline const std::shared_ptr<TrapCompletionRouteDescriptor> RuntimeOwnedPublication = std::make_shared<TrapCompletionRouteDescriptor>(CompletionRouteAuthority::Runtime, true, true, true);
		const CompletionRouteAuthority Authority;
		const bool AllowsCompletionPublication;
		const bool AllowsRetirePublication;
		const bool RequiresValidatedDomain;

	public:
		TrapCompletionRouteDescriptor();

		TrapCompletionRouteDescriptor(CompletionRouteAuthority authority, bool allowsCompletionPublication, bool allowsRetirePublication, bool requiresValidatedDomain);

		static std::shared_ptr<TrapCompletionRouteDescriptor> getProjectionOnlyDenied();

		static std::shared_ptr<TrapCompletionRouteDescriptor> getRuntimeOwnedCompletionPublication();

		static std::shared_ptr<TrapCompletionRouteDescriptor> getRuntimeOwnedPublication();

		CompletionRouteAuthority getAuthority() const;

		bool getAllowsCompletionPublication() const;

		bool getAllowsRetirePublication() const;

		bool getRequiresValidatedDomain() const;

		bool isRuntimeAuthoritative() const;
	};
}
