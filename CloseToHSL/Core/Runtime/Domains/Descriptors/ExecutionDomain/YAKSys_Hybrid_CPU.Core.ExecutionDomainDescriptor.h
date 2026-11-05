#pragma once
#include "ExecutionDomainReadOnlyStateView.h"

#include "YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"
#include "YAKSys_Hybrid_CPU.Core.SchedulingBudgetDescriptor.h"
#include "YAKSys_Hybrid_CPU.Core.ExecutionExtensionDescriptor.h"
#include <string>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class ExecutionDomainDescriptor final : public std::enable_shared_from_this<ExecutionDomainDescriptor>
	{
	private:
		const unsigned long long DomainTag;
		const std::shared_ptr<BundleLegalityDescriptor> BundleLegality;
		const std::shared_ptr<SchedulingBudgetDescriptor> SchedulingBudget;
		const std::shared_ptr<ExecutionExtensionDescriptor> Extension;
		const bool CompatibilityProjectionEnabled;
		const std::shared_ptr<ExecutionDomainReadOnlyStateView> ReadOnlyState;

	public:
		ExecutionDomainDescriptor();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionDomainDescriptor(ulong domainTag, BundleLegalityDescriptor? bundleLegality, System.Nullable<SchedulingBudgetDescriptor> schedulingBudget, System.Nullable<ExecutionExtensionDescriptor> extension, bool compatibilityProjectionEnabled, System.Nullable<ExecutionDomainReadOnlyStateView> readOnlyState = null)
		ExecutionDomainDescriptor(unsigned long long domainTag, std::shared_ptr<BundleLegalityDescriptor> bundleLegality, std::shared_ptr<SchedulingBudgetDescriptor> schedulingBudget, std::shared_ptr<ExecutionExtensionDescriptor> extension, bool compatibilityProjectionEnabled, std::shared_ptr<ExecutionDomainReadOnlyStateView> readOnlyState = nullptr);

		unsigned long long getDomainTag() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public BundleLegalityDescriptor? BundleLegality {get;}
		std::shared_ptr<BundleLegalityDescriptor> getBundleLegality() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SchedulingBudgetDescriptor? SchedulingBudget {get;}
		std::shared_ptr<SchedulingBudgetDescriptor> getSchedulingBudget() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionExtensionDescriptor? Extension {get;}
		std::shared_ptr<ExecutionExtensionDescriptor> getExtension() const;

		bool getCompatibilityProjectionEnabled() const;

		std::shared_ptr<ExecutionDomainReadOnlyStateView> getReadOnlyState() const;

		bool isAuthoritativeExecutionStateOwner() const;

		bool getHasBundleLegality() const;

		bool getHasSchedulingBudget() const;

		bool getHasExecutionExtension() const;

		bool getHasMaterializedGuestArchitecturalState() const;

		bool TryCreateReadOnlyStateView(std::shared_ptr<ExecutionDomainReadOnlyStateView> &view, std::wstring &reason);

		std::shared_ptr<ExecutionDomainDescriptor> WithReadOnlyState(std::shared_ptr<ExecutionDomainReadOnlyStateView> readOnlyState);

		std::shared_ptr<ExecutionDomainDescriptor> WithCompatibilityProjection(bool enabled);
	};
}
