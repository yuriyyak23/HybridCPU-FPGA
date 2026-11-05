#include "YAKSys_Hybrid_CPU.Core.SecureMeasurementAdmissionPolicy.h"
#include "../Evidence/SecureEvidencePolicy.h"
#include "../../Descriptors/Measurement/DomainMeasurementDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMeasurementAdmissionResult AdmitMeasurement(SecureComputeDomainDescriptor descriptor, DomainMeasurementDescriptor? measurement, System.Nullable<SecureMemoryDomainDescriptor> memory)
	std::shared_ptr<SecureMeasurementAdmissionResult> SecureMeasurementAdmissionPolicy::AdmitMeasurement(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory)
	{
		return AdmitMeasurement(descriptor, measurement, memory, descriptor->getRequiresMeasurement());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMeasurementAdmissionResult AdmitAttestationPublication(SecureComputeDomainDescriptor descriptor, DomainMeasurementDescriptor? measurement, System.Nullable<SecureMemoryDomainDescriptor> memory, System.Nullable<EvidencePolicyDescriptor> baseEvidencePolicy, System.Nullable<SecureCompletionPublicationFence> publicationFence)
	std::shared_ptr<SecureMeasurementAdmissionResult> SecureMeasurementAdmissionPolicy::AdmitAttestationPublication(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory, std::shared_ptr<EvidencePolicyDescriptor> baseEvidencePolicy, std::shared_ptr<SecureCompletionPublicationFence> publicationFence)
	{
		std::shared_ptr<SecureMeasurementAdmissionResult> measurementAdmission = AdmitMeasurement(descriptor, measurement, memory, true);
		if (!measurementAdmission->IsAllowed)
		{
			return measurementAdmission;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: DomainMeasurementDescriptor materialized = measurement!;
		std::shared_ptr<DomainMeasurementDescriptor> materialized = measurement;
		SecureEvidenceVisibilityClass evidenceClass = materialized->getAttestationEvidenceClass();
		if (descriptor->getEvidenceVisibilityPolicy()->MustQuarantine(evidenceClass) || evidenceClass == SecureEvidenceVisibilityClass::RecomputedAfterRestore)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedEvidenceVisibility, L"Attestation evidence is host-owned, denied or recomputed-only and cannot be published to guest-visible state.");
		}

		if (publicationFence == nullptr || !publicationFence->getCanPublishCompletion())
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedPublicationFence, L"Attestation publication requires a completion publication fence.");
		}

		if (baseEvidencePolicy == nullptr)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedEvidenceVisibility, L"Attestation publication requires a neutral evidence policy.");
		}

		switch (evidenceClass)
		{
		case SecureEvidenceVisibilityClass::GuestVisible:
			if (descriptor->getEvidenceVisibilityPolicy()->CanExposeToGuest(baseEvidencePolicy, evidenceClass))
				return SecureMeasurementAdmissionResult::AllowedMeasurement;
			break;
		case SecureEvidenceVisibilityClass::CompatibilityAlias:
			if (descriptor->getCompatibilityProjectionPolicy()->getAllowReadOnlyAliases() && descriptor->getEvidenceVisibilityPolicy()->CanExposeToGuest(baseEvidencePolicy, evidenceClass))
				return SecureMeasurementAdmissionResult::AllowedMeasurement;
			break;
		case SecureEvidenceVisibilityClass::MigrationSerializable:
			if (descriptor->getEvidenceVisibilityPolicy()->CanSerializeAcrossMigration(baseEvidencePolicy, evidenceClass))
				return SecureMeasurementAdmissionResult::AllowedMeasurement;
			break;
		case SecureEvidenceVisibilityClass::DebugOnly:
			if (descriptor->getDebugPolicy()->getAllowsDebug() && materialized->getDebugClass() == SecureMeasurementDebugClass::MeasuredDebug && descriptor->getEvidenceVisibilityPolicy()->CanExposeToGuest(baseEvidencePolicy, evidenceClass))
				return SecureMeasurementAdmissionResult::AllowedMeasurement;
			break;
		default:
			break;
		}
		return Deny(SecureMeasurementAdmissionDecision::DeniedEvidenceVisibility, L"Attestation evidence visibility is not allowed by secure and neutral evidence policies.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureMeasurementAdmissionResult AdmitMeasurement(SecureComputeDomainDescriptor descriptor, DomainMeasurementDescriptor? measurement, System.Nullable<SecureMemoryDomainDescriptor> memory, bool requireMeasurement)
	std::shared_ptr<SecureMeasurementAdmissionResult> SecureMeasurementAdmissionPolicy::AdmitMeasurement(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory, bool requireMeasurement)
	{
		if (!requireMeasurement)
		{
			return SecureMeasurementAdmissionResult::AllowedMeasurement;
		}

		if (measurement == nullptr || measurement->getState() == SecureMeasurementState::Missing)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedMissingMeasurement, L"Secure domain requires a materialized measurement descriptor.");
		}

		if (measurement->getState() == SecureMeasurementState::Pending)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedPendingMeasurement, L"Secure domain measurement is still pending materialization.");
		}

		if (measurement->getState() == SecureMeasurementState::Stale)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedStaleMeasurement, L"Secure domain measurement epoch is stale.");
		}

		if (measurement->getState() == SecureMeasurementState::Revoked)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedRevokedMeasurement, L"Secure domain measurement has been revoked.");
		}

		if (!measurement->isMaterialized() || !measurement->BindsDomain(descriptor->getDomainTag()))
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedUnmaterializedMeasurement, L"Secure domain measurement must be materialized and bound to the secure domain.");
		}

		if (!measurement->BindsPolicyDigest(DomainMeasurementDescriptor::ComputePolicyDigest(descriptor)))
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedPolicyDigestMismatch, L"Secure domain measurement must bind to the secure domain policy digest.");
		}

		if (memory != nullptr && memory->isMaterialized())
		{
			if (!measurement->IsCurrentFor(memory->getPolicyEpoch()))
			{
				return Deny(SecureMeasurementAdmissionDecision::DeniedStaleMeasurement, L"Secure domain measurement epoch must match the secure memory policy epoch.");
			}

			if (memory->getHasMeasuredMemory() && !measurement->BindsMemoryDigest(DomainMeasurementDescriptor::ComputeMemoryDigest(memory)))
			{
				return Deny(SecureMeasurementAdmissionDecision::DeniedMemoryDigestMismatch, L"Secure domain measurement must bind to measured secure memory regions.");
			}
		}

		if (descriptor->getDebugPolicy()->getChangesMeasurementClass() && measurement->getDebugClass() != SecureMeasurementDebugClass::MeasuredDebug)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedDebugClassMismatch, L"Secure debug policy changes the measurement class and requires measured-debug evidence.");
		}

		if (!descriptor->getDebugPolicy()->getAllowsDebug() && measurement->getDebugClass() == SecureMeasurementDebugClass::MeasuredDebug)
		{
			return Deny(SecureMeasurementAdmissionDecision::DeniedDebugClassMismatch, L"Measured-debug evidence is denied unless secure debug policy explicitly allows debug.");
		}

		return SecureMeasurementAdmissionResult::AllowedMeasurement;
	}

	std::shared_ptr<SecureMeasurementAdmissionResult> SecureMeasurementAdmissionPolicy::Deny(SecureMeasurementAdmissionDecision decision, const std::wstring &reason)
	{
		return SecureMeasurementAdmissionResult::Denied(decision, reason);
	}
}
