#pragma once

#include "../../Generated/Schemas/SecureComputeCompatibilityProjectionSchema.h"
#include "../../../Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class SecureComputeCompatibilityFieldClass : unsigned char
	{
		OrdinaryReadOnlyProjection = 0,
		GuestPrivilegedControl = 1,
		HostAddressSpaceAlias = 2,
		HostExecutionAlias = 3,
		CompatibilityControl = 4,
		SecureMeasurementEvidenceDebugMigration = 5
	};

	enum class SecureComputeCompatibilityMatrixDecision : unsigned char
	{
		AllowedReadOnlyProjection = 0,
		DeniedNoNeutralOwner = 1,
		DeniedNoReadOnlySource = 2,
		DeniedNoSecureVisibility = 3,
		DeniedNoMigrationClass = 4,
		DeniedNoConformanceProof = 5,
		DeniedGuestPrivilegedControlOwnerMissing = 6,
		DeniedHostAddressSpaceOwnerMissing = 7,
		DeniedHostExecutionOwnerMissing = 8,
		DeniedCompatibilityControlContractMissing = 9,
		DeniedSecureSensitiveField = 10,
		DeniedSchemaOwnerMismatch = 11,
		DeniedWriteMutation = 12,
		DeniedVmxCapsAuthority = 13,
		DeniedVmcsCheckpointAuthority = 14,
		DeniedBackendSuccess = 15,
		DeniedUnsupportedGuestPrivilegedControlField = 16
	};

	struct SecureComputeCompatibilityReadMatrixRequest final
	{
		const unsigned long long FieldId;
		const SecureComputeCompatibilityFieldClass FieldClass;
		const SecureComputeProjectionOwnerKind SchemaOwner;
		const SecureComputeProjectionOwnerKind ExpectedOwner;
		const bool HasNeutralOwner;
		const bool HasReadOnlySource;
		const bool SecureVisibilityAllowed;
		const bool MigrationClassified;
		const bool ConformanceProven;

		SecureComputeCompatibilityReadMatrixRequest(unsigned long long fieldId,
			SecureComputeCompatibilityFieldClass fieldClass,
			SecureComputeProjectionOwnerKind schemaOwner,
			SecureComputeProjectionOwnerKind expectedOwner,
			bool hasNeutralOwner, bool hasReadOnlySource, bool secureVisibilityAllowed,
			bool migrationClassified, bool conformanceProven)
			: FieldId(fieldId), FieldClass(fieldClass), SchemaOwner(schemaOwner),
			  ExpectedOwner(expectedOwner), HasNeutralOwner(hasNeutralOwner),
			  HasReadOnlySource(hasReadOnlySource), SecureVisibilityAllowed(secureVisibilityAllowed),
			  MigrationClassified(migrationClassified), ConformanceProven(conformanceProven) {}
	};

	struct SecureComputeCompatibilityMatrixResult final
	{
		const SecureComputeCompatibilityMatrixDecision Decision;
		const bool ValueAvailable;
		const bool BackendSuccessAuthorized;
		const bool MutationAuthorized;
		const std::wstring Reason;
		const bool IsAllowed;

		SecureComputeCompatibilityMatrixResult(SecureComputeCompatibilityMatrixDecision decision,
			bool valueAvailable, bool backendSuccessAuthorized, bool mutationAuthorized,
			std::wstring reason)
			: Decision(decision), ValueAvailable(valueAvailable),
			  BackendSuccessAuthorized(backendSuccessAuthorized), MutationAuthorized(mutationAuthorized),
			  Reason(std::move(reason)),
			  IsAllowed(decision == SecureComputeCompatibilityMatrixDecision::AllowedReadOnlyProjection &&
				valueAvailable && !backendSuccessAuthorized && !mutationAuthorized) {}

		static const std::shared_ptr<SecureComputeCompatibilityMatrixResult> AllowedReadOnlyProjection;
		static std::shared_ptr<SecureComputeCompatibilityMatrixResult> Denied(
			SecureComputeCompatibilityMatrixDecision decision, const std::wstring &reason);
	};

	class SecureComputeCompatibilityBoundaryMatrixPolicy final : public std::enable_shared_from_this<SecureComputeCompatibilityBoundaryMatrixPolicy>
	{
	public:
		std::shared_ptr<SecureComputeCompatibilityMatrixResult> AdmitVmRead(std::shared_ptr<SecureComputeCompatibilityReadMatrixRequest> request);

		std::shared_ptr<SecureComputeCompatibilityMatrixResult> AdmitVmWrite(bool secureSensitiveField);

		std::shared_ptr<SecureComputeCompatibilityMatrixResult> AdmitVmxCapsDescriptorMaterialization(bool attemptsSecureDescriptorMaterialization);

		std::shared_ptr<SecureComputeCompatibilityMatrixResult> AdmitVmcsCheckpointAuthority(bool containsSecureMetadata);

		std::shared_ptr<SecureComputeCompatibilityMatrixResult> AdmitProjectionCompletion(bool attemptsBackendSuccess);

	private:
		static std::shared_ptr<SecureComputeCompatibilityMatrixResult> DenyByFieldClass(unsigned long long fieldId, SecureComputeCompatibilityFieldClass fieldClass);

		static std::shared_ptr<SecureComputeCompatibilityMatrixResult> Deny(SecureComputeCompatibilityMatrixDecision decision, const std::wstring &reason);
	};
}
