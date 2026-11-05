#pragma once
#include "../../Authority/SecureRevocationEpoch.h"

#include "SecureMigrationDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureMigrationDescriptor final : public std::enable_shared_from_this<SecureMigrationDescriptor>
	{
	private:
		static inline const std::shared_ptr<SecureMigrationDescriptor> Disabled = std::make_shared<SecureMigrationDescriptor>();
		const SecureMigrationMode Mode;
		const SecurePrivateMemoryMigrationPolicy PrivateMemoryPolicy;
		const std::shared_ptr<SecureRevocationEpoch> PolicyEpoch;
		const bool AllowGuestVisibleEvidence;
		const bool AllowCompatibilityProjectionMetadata;
		const SecureMeasurementRestorePolicy MeasurementRestorePolicy;
		const SecureGrantRestorePolicy GrantRestorePolicy;

	public:
		SecureMigrationDescriptor();

		SecureMigrationDescriptor(SecureMigrationMode mode, SecurePrivateMemoryMigrationPolicy privateMemoryPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, bool allowGuestVisibleEvidence, bool allowCompatibilityProjectionMetadata);

		SecureMigrationDescriptor(SecureMigrationMode mode, SecurePrivateMemoryMigrationPolicy privateMemoryPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, bool allowGuestVisibleEvidence, bool allowCompatibilityProjectionMetadata, SecureMeasurementRestorePolicy measurementRestorePolicy, SecureGrantRestorePolicy grantRestorePolicy);

		static std::shared_ptr<SecureMigrationDescriptor> getDisabled();

		SecureMigrationMode getMode() const;

		SecurePrivateMemoryMigrationPolicy getPrivateMemoryPolicy() const;

		std::shared_ptr<SecureRevocationEpoch> getPolicyEpoch() const;

		bool getAllowGuestVisibleEvidence() const;

		bool getAllowCompatibilityProjectionMetadata() const;

		SecureMeasurementRestorePolicy getMeasurementRestorePolicy() const;

		SecureGrantRestorePolicy getGrantRestorePolicy() const;

		bool getAllowsMigration() const;

		bool getAllowsPrivateMemoryPayload() const;

		bool getRequiresReattestation() const;

		bool getRequiresMeasurementRevalidation() const;
	};
}
