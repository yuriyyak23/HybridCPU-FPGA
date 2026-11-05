#pragma once

#include "../../Descriptors/Migration/YAKSys_Hybrid_CPU.Core.SecureMigrationDescriptor.h"
#include "../../Checkpoint/SecureCheckpointPayloadPolicy.h"
#include "../../Descriptors/Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.h"
#include "../../Descriptors/Measurement/YAKSys_Hybrid_CPU.Core.DomainMeasurementDescriptor.h"
#include "SecureMigrationAdmissionPolicy.h"
#include <string>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureMigrationAdmissionPolicy final : public std::enable_shared_from_this<SecureMigrationAdmissionPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureMigrationAdmissionPolicy> Default = std::make_shared<SecureMigrationAdmissionPolicy>();

	public:
		static std::shared_ptr<SecureMigrationAdmissionPolicy> getDefault();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMigrationAdmissionResult AdmitCheckpointPayload(SecureMigrationDescriptor? migration, SecureCheckpointPayloadClass payloadClass, System.Nullable<SecurePrivateMemorySealedPayloadContract> privateMemoryContract = null)
		std::shared_ptr<SecureMigrationAdmissionResult> AdmitCheckpointPayload(std::shared_ptr<SecureMigrationDescriptor> migration, SecureCheckpointPayloadClass payloadClass, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract = std::nullopt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMigrationAdmissionResult AdmitRestore(SecureMigrationDescriptor? migration, System.Nullable<DomainMeasurementDescriptor> measurement, System.Nullable<SecureMemoryDomainDescriptor> memory, SecureRevocationEpoch expectedPolicyEpoch, SecureGrantHandle restoredGrant, bool measurementRevalidated, bool reattestationCompleted, bool grantProvenanceValidated = false)
		std::shared_ptr<SecureMigrationAdmissionResult> AdmitRestore(std::shared_ptr<SecureMigrationDescriptor> migration, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory, std::shared_ptr<SecureRevocationEpoch> expectedPolicyEpoch, std::shared_ptr<SecureGrantHandle> restoredGrant, bool measurementRevalidated, bool reattestationCompleted, bool grantProvenanceValidated = false);

	private:
		static std::shared_ptr<SecureMigrationAdmissionResult> AdmitPrivateMemoryPayload(std::shared_ptr<SecureMigrationDescriptor> migration, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureMigrationAdmissionResult ValidateMigrationEnabled(SecureMigrationDescriptor? migration)
		static std::shared_ptr<SecureMigrationAdmissionResult> ValidateMigrationEnabled(std::shared_ptr<SecureMigrationDescriptor> migration);

		static std::shared_ptr<SecureMigrationAdmissionResult> Deny(SecureMigrationAdmissionDecision decision, const std::wstring &reason);
	};
}
