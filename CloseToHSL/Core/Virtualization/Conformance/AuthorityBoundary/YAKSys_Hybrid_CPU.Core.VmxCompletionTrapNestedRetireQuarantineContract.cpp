#include "YAKSys_Hybrid_CPU.Core.VmxCompletionTrapNestedRetireQuarantineContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<std::wstring> VmxCompletionTrapNestedRetireQuarantineContract::getRemovedCompatibilitySubstratePaths()
	{
		return RemovedCompatibilitySubstratePaths;
	}

	std::vector<std::wstring> VmxCompletionTrapNestedRetireQuarantineContract::getNeutralAuthorityPaths()
	{
		return NeutralAuthorityPaths;
	}

	std::vector<std::wstring> VmxCompletionTrapNestedRetireQuarantineContract::getCompatibilityQuarantinePaths()
	{
		return CompatibilityQuarantinePaths;
	}

	std::vector<std::wstring> VmxCompletionTrapNestedRetireQuarantineContract::getForbiddenNeutralAuthorityMarkers()
	{
		return ForbiddenNeutralAuthorityMarkers;
	}

	std::vector<std::wstring> VmxCompletionTrapNestedRetireQuarantineContract::getRequiredNeutralAuthorityMarkers()
	{
		return RequiredNeutralAuthorityMarkers;
	}

	std::vector<std::wstring> VmxCompletionTrapNestedRetireQuarantineContract::getForbiddenVmcsProjectionHelperMarkers()
	{
		return ForbiddenVmcsProjectionHelperMarkers;
	}

	std::vector<std::wstring> VmxCompletionTrapNestedRetireQuarantineContract::getRequiredVmcsProjectionReadOnlyMarkers()
	{
		return RequiredVmcsProjectionReadOnlyMarkers;
	}

	bool VmxCompletionTrapNestedRetireQuarantineContract::getRemovesSubstrateCompatibilityAuthority() const
	{
		return true;
	}

	bool VmxCompletionTrapNestedRetireQuarantineContract::getKeepsVmxNamesAsCompatibilityProjectionOnly() const
	{
		return true;
	}

	bool VmxCompletionTrapNestedRetireQuarantineContract::getRequiresNeutralRuntimeAuthority() const
	{
		return true;
	}

	bool VmxCompletionTrapNestedRetireQuarantineContract::getDoesNotCreateRenamedRuntimeOwner() const
	{
		return true;
	}

	bool VmxCompletionTrapNestedRetireQuarantineContract::IsSatisfied()
	{
		return getRemovesSubstrateCompatibilityAuthority() && getKeepsVmxNamesAsCompatibilityProjectionOnly() && getRequiresNeutralRuntimeAuthority() && getDoesNotCreateRenamedRuntimeOwner();
	}
}
