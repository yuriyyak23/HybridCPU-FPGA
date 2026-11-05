#include "YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	EvidencePolicyDescriptor::EvidencePolicyDescriptor() : EvidencePolicyDescriptor(false, false, false)
	{
	}

	EvidencePolicyDescriptor::EvidencePolicyDescriptor(bool allowCompatibilityAliases, bool allowGuestArchitecturalState, bool allowMigrationSerializableState) : AllowCompatibilityAliases(allowCompatibilityAliases), AllowGuestArchitecturalState(allowGuestArchitecturalState), AllowMigrationSerializableState(allowMigrationSerializableState) {}

	std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicyDescriptor::getFailClosed()
	{
		return FailClosed;
	}

	bool EvidencePolicyDescriptor::getAllowCompatibilityAliases() const
	{
		return AllowCompatibilityAliases;
	}

	bool EvidencePolicyDescriptor::getAllowGuestArchitecturalState() const
	{
		return AllowGuestArchitecturalState;
	}

	bool EvidencePolicyDescriptor::getAllowMigrationSerializableState() const
	{
		return AllowMigrationSerializableState;
	}

	bool EvidencePolicyDescriptor::CanExposeToGuest(EvidenceVisibilityClass evidenceClass)
	{
		bool temp_var152;
		switch (evidenceClass)
		{
			case EvidenceVisibilityClass::GuestArchitecturalState:
				temp_var152 = getAllowGuestArchitecturalState();
				break;
			case EvidenceVisibilityClass::CompatibilityAlias:
				temp_var152 = getAllowCompatibilityAliases();
				break;
			default:
				temp_var152 = false;
				break;
		}
		return temp_var152;
	}

	bool EvidencePolicyDescriptor::CanSerializeAcrossMigration(EvidenceVisibilityClass evidenceClass, EvidenceRestorePolicy restorePolicy)
	{
		return getAllowMigrationSerializableState() && evidenceClass == EvidenceVisibilityClass::GuestArchitecturalState && restorePolicy == EvidenceRestorePolicy::PreserveGuestArchitecturalState;
	}

	bool EvidencePolicyDescriptor::MustRecomputeAfterRestore(EvidenceVisibilityClass evidenceClass)
	{
		return (evidenceClass == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || evidenceClass == EvidenceVisibilityClass::SchedulerEvidence || evidenceClass == EvidenceVisibilityClass::BackendBindingEvidence || evidenceClass == EvidenceVisibilityClass::NativeTokenEvidence);
	}
}
