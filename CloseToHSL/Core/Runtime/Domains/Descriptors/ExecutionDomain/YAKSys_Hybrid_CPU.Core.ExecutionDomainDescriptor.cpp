#include "YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ExecutionDomainDescriptor::ExecutionDomainDescriptor() : ExecutionDomainDescriptor(0, nullptr, nullptr, nullptr, true, nullptr)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionDomainDescriptor(ulong domainTag, BundleLegalityDescriptor? bundleLegality, System.Nullable<SchedulingBudgetDescriptor> schedulingBudget, System.Nullable<ExecutionExtensionDescriptor> extension, bool compatibilityProjectionEnabled, System.Nullable<ExecutionDomainReadOnlyStateView> readOnlyState = null)
	ExecutionDomainDescriptor::ExecutionDomainDescriptor(unsigned long long domainTag, std::shared_ptr<BundleLegalityDescriptor> bundleLegality, std::shared_ptr<SchedulingBudgetDescriptor> schedulingBudget, std::shared_ptr<ExecutionExtensionDescriptor> extension, bool compatibilityProjectionEnabled, std::shared_ptr<ExecutionDomainReadOnlyStateView> readOnlyState) : DomainTag(domainTag), BundleLegality(bundleLegality), SchedulingBudget(schedulingBudget), Extension(extension), CompatibilityProjectionEnabled(compatibilityProjectionEnabled), ReadOnlyState(readOnlyState ? readOnlyState : ExecutionDomainReadOnlyStateView::Unmaterialized) {}

	unsigned long long ExecutionDomainDescriptor::getDomainTag() const
	{
		return DomainTag;
	}

	std::shared_ptr<BundleLegalityDescriptor> ExecutionDomainDescriptor::getBundleLegality() const
	{
		return BundleLegality;
	}

	std::shared_ptr<SchedulingBudgetDescriptor> ExecutionDomainDescriptor::getSchedulingBudget() const
	{
		return SchedulingBudget;
	}

	std::shared_ptr<ExecutionExtensionDescriptor> ExecutionDomainDescriptor::getExtension() const
	{
		return Extension;
	}

	bool ExecutionDomainDescriptor::getCompatibilityProjectionEnabled() const
	{
		return CompatibilityProjectionEnabled;
	}

	std::shared_ptr<ExecutionDomainReadOnlyStateView> ExecutionDomainDescriptor::getReadOnlyState() const
	{
		return ReadOnlyState;
	}

	bool ExecutionDomainDescriptor::isAuthoritativeExecutionStateOwner() const
	{
		return true;
	}

	bool ExecutionDomainDescriptor::getHasBundleLegality() const
	{
		return getBundleLegality() != nullptr;
	}

	bool ExecutionDomainDescriptor::getHasSchedulingBudget() const
	{
		return getSchedulingBudget() != nullptr;
	}

	bool ExecutionDomainDescriptor::getHasExecutionExtension() const
	{
		return getExtension() != nullptr;
	}

	bool ExecutionDomainDescriptor::getHasMaterializedGuestArchitecturalState() const
	{
		return getReadOnlyState()->getHasAnyMaterializedGuestArchitecturalState();
	}

	bool ExecutionDomainDescriptor::TryCreateReadOnlyStateView(std::shared_ptr<ExecutionDomainReadOnlyStateView> &view, std::wstring &reason)
	{
		if (!getHasMaterializedGuestArchitecturalState())
		{
			view = nullptr;
			reason = L"Execution domain descriptor has no materialized read-only guest architectural state view.";
			return false;
		}

		view = getReadOnlyState();
		reason = L"Execution domain descriptor exposed a read-only guest architectural state view.";
		return true;
	}

	std::shared_ptr<ExecutionDomainDescriptor> ExecutionDomainDescriptor::WithReadOnlyState(std::shared_ptr<ExecutionDomainReadOnlyStateView> readOnlyState)
	{
		return std::make_shared<ExecutionDomainDescriptor>(getDomainTag(), getBundleLegality(), getSchedulingBudget(), getExtension(), getCompatibilityProjectionEnabled(), readOnlyState);
	}

	std::shared_ptr<ExecutionDomainDescriptor> ExecutionDomainDescriptor::WithCompatibilityProjection(bool enabled)
	{
		return std::make_shared<ExecutionDomainDescriptor>(getDomainTag(), getBundleLegality(), getSchedulingBudget(), getExtension(), enabled, getReadOnlyState());
	}
}
