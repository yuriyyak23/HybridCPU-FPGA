#include "YAKSys_Hybrid_CPU.Core.SecureMigrationDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureMigrationDescriptor::SecureMigrationDescriptor() : SecureMigrationDescriptor(SecureMigrationMode::Disabled, SecurePrivateMemoryMigrationPolicy::Denied, SecureRevocationEpoch::Unmaterialized, false, false)
	{
	}

	SecureMigrationDescriptor::SecureMigrationDescriptor(SecureMigrationMode mode, SecurePrivateMemoryMigrationPolicy privateMemoryPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, bool allowGuestVisibleEvidence, bool allowCompatibilityProjectionMetadata) : SecureMigrationDescriptor(mode, privateMemoryPolicy, policyEpoch, allowGuestVisibleEvidence, allowCompatibilityProjectionMetadata, mode == SecureMigrationMode::ReattestRequired ? SecureMeasurementRestorePolicy::Reattest : SecureMeasurementRestorePolicy::Revalidate, SecureGrantRestorePolicy::Rederive)
	{
	}

	SecureMigrationDescriptor::SecureMigrationDescriptor(SecureMigrationMode mode, SecurePrivateMemoryMigrationPolicy privateMemoryPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, bool allowGuestVisibleEvidence, bool allowCompatibilityProjectionMetadata, SecureMeasurementRestorePolicy measurementRestorePolicy, SecureGrantRestorePolicy grantRestorePolicy)
		: Mode(mode), PrivateMemoryPolicy(privateMemoryPolicy), PolicyEpoch(policyEpoch), AllowGuestVisibleEvidence(allowGuestVisibleEvidence), AllowCompatibilityProjectionMetadata(allowCompatibilityProjectionMetadata), MeasurementRestorePolicy(measurementRestorePolicy), GrantRestorePolicy(grantRestorePolicy)
	{
	}

	std::shared_ptr<SecureMigrationDescriptor> SecureMigrationDescriptor::getDisabled()
	{
		return Disabled;
	}

	SecureMigrationMode SecureMigrationDescriptor::getMode() const
	{
		return Mode;
	}

	SecurePrivateMemoryMigrationPolicy SecureMigrationDescriptor::getPrivateMemoryPolicy() const
	{
		return PrivateMemoryPolicy;
	}

	std::shared_ptr<SecureRevocationEpoch> SecureMigrationDescriptor::getPolicyEpoch() const
	{
		return PolicyEpoch;
	}

	bool SecureMigrationDescriptor::getAllowGuestVisibleEvidence() const
	{
		return AllowGuestVisibleEvidence;
	}

	bool SecureMigrationDescriptor::getAllowCompatibilityProjectionMetadata() const
	{
		return AllowCompatibilityProjectionMetadata;
	}

	SecureMeasurementRestorePolicy SecureMigrationDescriptor::getMeasurementRestorePolicy() const
	{
		return MeasurementRestorePolicy;
	}

	SecureGrantRestorePolicy SecureMigrationDescriptor::getGrantRestorePolicy() const
	{
		return GrantRestorePolicy;
	}

	bool SecureMigrationDescriptor::getAllowsMigration() const
	{
		return getMode() != SecureMigrationMode::Disabled;
	}

	bool SecureMigrationDescriptor::getAllowsPrivateMemoryPayload() const
	{
		return getAllowsMigration() && getPrivateMemoryPolicy() == SecurePrivateMemoryMigrationPolicy::SealedEncryptedPayloadRequired;
	}

	bool SecureMigrationDescriptor::getRequiresReattestation() const
	{
		return getMode() == SecureMigrationMode::ReattestRequired || getMeasurementRestorePolicy() == SecureMeasurementRestorePolicy::Reattest;
	}

	bool SecureMigrationDescriptor::getRequiresMeasurementRevalidation() const
	{
		return getRequiresReattestation() || ([&] { const auto codexPatternValue = getMeasurementRestorePolicy(); return codexPatternValue == SecureMeasurementRestorePolicy::Revalidate || codexPatternValue == SecureMeasurementRestorePolicy::Remeasure; }());
	}
}
