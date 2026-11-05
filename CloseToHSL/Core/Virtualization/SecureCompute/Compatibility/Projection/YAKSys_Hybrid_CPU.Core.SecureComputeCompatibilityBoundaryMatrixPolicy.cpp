#include "YAKSys_Hybrid_CPU.Core.SecureComputeCompatibilityBoundaryMatrixPolicy.h"
#include "../../../Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<SecureComputeCompatibilityMatrixResult>
		SecureComputeCompatibilityMatrixResult::AllowedReadOnlyProjection =
			std::make_shared<SecureComputeCompatibilityMatrixResult>(
				SecureComputeCompatibilityMatrixDecision::AllowedReadOnlyProjection,
				true, false, false, std::wstring{});

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityMatrixResult::Denied(
		SecureComputeCompatibilityMatrixDecision decision, const std::wstring &reason)
	{
		return std::make_shared<SecureComputeCompatibilityMatrixResult>(decision, false, false, false, reason);
	}

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityBoundaryMatrixPolicy::AdmitVmRead(
		std::shared_ptr<SecureComputeCompatibilityReadMatrixRequest> request)
	{
		if (!request)
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedNoNeutralOwner,
				L"SecureCompute VMREAD requires a valid request and neutral owner.");
		if (request->SchemaOwner != request->ExpectedOwner)
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedSchemaOwnerMismatch,
				L"SecureCompute compatibility projection schema owner mismatch is denied.");

		auto classDecision = SecureComputeCompatibilityBoundaryMatrixPolicy::DenyByFieldClass(request->FieldId, request->FieldClass);
		if (!classDecision->IsAllowed)
			return classDecision;
		if (!request->HasNeutralOwner)
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedNoNeutralOwner,
				L"SecureCompute VMREAD requires a neutral owner.");
		if (!request->HasReadOnlySource)
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedNoReadOnlySource,
				L"SecureCompute VMREAD requires a read-only neutral value source.");
		if (!request->SecureVisibilityAllowed)
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedNoSecureVisibility,
				L"SecureCompute VMREAD requires secure visibility policy.");
		if (!request->MigrationClassified)
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedNoMigrationClass,
				L"SecureCompute VMREAD requires migration classification.");
		if (!request->ConformanceProven)
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedNoConformanceProof,
				L"SecureCompute VMREAD requires conformance proof.");
		return SecureComputeCompatibilityMatrixResult::AllowedReadOnlyProjection;
	}

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityBoundaryMatrixPolicy::AdmitVmWrite(bool secureSensitiveField)
	{
		(void)secureSensitiveField;
		return Deny(SecureComputeCompatibilityMatrixDecision::DeniedWriteMutation,
			L"SecureCompute compatibility VMWRITE is denied and has no backend mutation.");
	}

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityBoundaryMatrixPolicy::AdmitVmxCapsDescriptorMaterialization(bool attemptsSecureDescriptorMaterialization)
	{
		return attemptsSecureDescriptorMaterialization
			? Deny(SecureComputeCompatibilityMatrixDecision::DeniedVmxCapsAuthority,
				L"VmxCaps projection cannot materialize a SecureCompute descriptor.")
			: SecureComputeCompatibilityMatrixResult::AllowedReadOnlyProjection;
	}

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityBoundaryMatrixPolicy::AdmitVmcsCheckpointAuthority(bool containsSecureMetadata)
	{
		return containsSecureMetadata
			? Deny(SecureComputeCompatibilityMatrixDecision::DeniedVmcsCheckpointAuthority,
				L"VMCS projection metadata cannot be SecureCompute checkpoint authority.")
			: SecureComputeCompatibilityMatrixResult::AllowedReadOnlyProjection;
	}

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityBoundaryMatrixPolicy::AdmitProjectionCompletion(bool attemptsBackendSuccess)
	{
		return attemptsBackendSuccess
			? Deny(SecureComputeCompatibilityMatrixDecision::DeniedBackendSuccess,
				L"Compatibility projection cannot become SecureCompute backend success.")
			: SecureComputeCompatibilityMatrixResult::AllowedReadOnlyProjection;
	}

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityBoundaryMatrixPolicy::DenyByFieldClass(
		unsigned long long fieldId, SecureComputeCompatibilityFieldClass fieldClass)
	{
		if (fieldClass == SecureComputeCompatibilityFieldClass::GuestPrivilegedControl)
		{
			const auto guestCr0 = static_cast<unsigned long long>(VmcsField::GuestCr0);
			const auto guestCr4 = static_cast<unsigned long long>(VmcsField::GuestCr4);
			if (fieldId != guestCr0 && fieldId != guestCr4)
				return Deny(SecureComputeCompatibilityMatrixDecision::DeniedUnsupportedGuestPrivilegedControlField,
					L"Only GuestCr0 and GuestCr4 have a Phase 10 privileged execution-state projection contract.");
		}

		switch (fieldClass)
		{
		case SecureComputeCompatibilityFieldClass::HostAddressSpaceAlias:
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedHostAddressSpaceOwnerMissing,
				L"Host address-space aliases require a neutral host-address-space owner.");
		case SecureComputeCompatibilityFieldClass::HostExecutionAlias:
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedHostExecutionOwnerMissing,
				L"Host execution aliases require a neutral host-execution owner.");
		case SecureComputeCompatibilityFieldClass::CompatibilityControl:
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedCompatibilityControlContractMissing,
				L"Compatibility-control fields require an explicit neutral value contract.");
		case SecureComputeCompatibilityFieldClass::SecureMeasurementEvidenceDebugMigration:
			return Deny(SecureComputeCompatibilityMatrixDecision::DeniedSecureSensitiveField,
				L"Secure measurement, evidence, debug and migration fields are denied by default.");
		default:
			return SecureComputeCompatibilityMatrixResult::AllowedReadOnlyProjection;
		}
	}

	std::shared_ptr<SecureComputeCompatibilityMatrixResult> SecureComputeCompatibilityBoundaryMatrixPolicy::Deny(
		SecureComputeCompatibilityMatrixDecision decision, const std::wstring &reason)
	{
		return SecureComputeCompatibilityMatrixResult::Denied(decision, reason);
	}
}
