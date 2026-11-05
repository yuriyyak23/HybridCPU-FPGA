#include "YAKSys_Hybrid_CPU.Core.SecureEvidencePolicy.h"
#include "../../../Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureEvidencePolicy::SecureEvidencePolicy() : SecureEvidencePolicy(false, false, false, false)
	{
	}

	SecureEvidencePolicy::SecureEvidencePolicy(bool allowGuestVisibleEvidence, bool allowMigrationSerializableEvidence, bool allowCompatibilityAliasEvidence, bool allowDebugEvidence)
		: AllowGuestVisibleEvidence(allowGuestVisibleEvidence),
		  AllowMigrationSerializableEvidence(allowMigrationSerializableEvidence),
		  AllowCompatibilityAliasEvidence(allowCompatibilityAliasEvidence),
		  AllowDebugEvidence(allowDebugEvidence)
	{
	}

	std::shared_ptr<SecureEvidencePolicy> SecureEvidencePolicy::getFailClosed()
	{
		return FailClosed;
	}

	bool SecureEvidencePolicy::getAllowGuestVisibleEvidence() const
	{
		return AllowGuestVisibleEvidence;
	}

	bool SecureEvidencePolicy::getAllowMigrationSerializableEvidence() const
	{
		return AllowMigrationSerializableEvidence;
	}

	bool SecureEvidencePolicy::getAllowCompatibilityAliasEvidence() const
	{
		return AllowCompatibilityAliasEvidence;
	}

	bool SecureEvidencePolicy::getAllowDebugEvidence() const
	{
		return AllowDebugEvidence;
	}

	bool SecureEvidencePolicy::CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> basePolicy, SecureEvidenceVisibilityClass evidenceClass)
	{
		if (basePolicy == nullptr)
		{
			return false;
		}

		switch (evidenceClass)
		{
		case SecureEvidenceVisibilityClass::GuestVisible:
			return getAllowGuestVisibleEvidence() && basePolicy->CanExposeToGuest(EvidenceVisibilityClass::GuestArchitecturalState);
		case SecureEvidenceVisibilityClass::CompatibilityAlias:
			return getAllowCompatibilityAliasEvidence() && basePolicy->CanExposeToGuest(EvidenceVisibilityClass::CompatibilityAlias);
		case SecureEvidenceVisibilityClass::DebugOnly:
			return getAllowDebugEvidence() && basePolicy->CanExposeToGuest(EvidenceVisibilityClass::GuestArchitecturalState);
		default:
			return false;
		}
	}

	bool SecureEvidencePolicy::CanSerializeAcrossMigration(std::shared_ptr<EvidencePolicyDescriptor> basePolicy, SecureEvidenceVisibilityClass evidenceClass)
	{
		return basePolicy != nullptr && evidenceClass == SecureEvidenceVisibilityClass::MigrationSerializable && getAllowMigrationSerializableEvidence() && basePolicy->CanSerializeAcrossMigration(EvidenceVisibilityClass::GuestArchitecturalState, EvidenceRestorePolicy::PreserveGuestArchitecturalState);
	}

	bool SecureEvidencePolicy::MustQuarantine(SecureEvidenceVisibilityClass evidenceClass)
	{
		return (evidenceClass == SecureEvidenceVisibilityClass::HostOwnedQuarantined || evidenceClass == SecureEvidenceVisibilityClass::Denied);
	}
}
