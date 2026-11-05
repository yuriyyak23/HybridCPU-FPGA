#include "YAKSys_Hybrid_CPU.Core.SecureDomainAdmissionPolicy.h"
#include "../../../../../Decoder/CanonicalSecurityOperationIdentity.h"
#include "../Measurement/YAKSys_Hybrid_CPU.Core.SecureMeasurementAdmissionPolicy.h"
#include "../Evidence/YAKSys_Hybrid_CPU.Core.SecureEvidencePolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureDomainAdmissionResult Admit(SecureComputeDomainDescriptor? descriptor, SecureDomainOperationClass operationClass, System.Nullable<DomainMeasurementDescriptor> measurement, System.Nullable<SecureMemoryDomainDescriptor> memory)
	std::shared_ptr<SecureDomainAdmissionResult> SecureDomainAdmissionPolicy::Admit(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, SecureDomainOperationClass operationClass, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory)
	{
		if (Decoder::CanonicalSecurityOperationIdentity::FromLegacyPolicyClass(operationClass) == Decoder::CanonicalSecurityOperationKind::Ordinary)
		{
			return SecureDomainAdmissionResult::AllowedNoEffect;
		}

		if (static_cast<unsigned char>(operationClass) > static_cast<unsigned char>(SecureDomainOperationClass::CompatibilityProjection))
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedUnsupportedOperationClass, L"Unknown secure-domain operation class is denied.");
		}

		if (descriptor == nullptr)
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingDescriptor, L"Secure descriptor is required only for secure-domain operation classes.");
		}

		if (!descriptor->isEnabled())
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedDisabledDescriptor, L"Secure descriptor is disabled.");
		}

		if (!descriptor->isMaterialized())
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedUnmaterializedDescriptor, L"Secure descriptor must be materialized before secure-domain enforcement is active.");
		}

		if (descriptor->getRequiresMeasurement())
		{
			std::shared_ptr<SecureMeasurementAdmissionResult> measurementAdmission = (std::make_shared<SecureMeasurementAdmissionPolicy>())->AdmitMeasurement(descriptor, measurement, memory);

			if (!measurementAdmission->IsAllowed)
			{
				return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingMeasurement, measurementAdmission->Reason);
			}
		}

		if (descriptor->getRequiresSecureMemoryPolicy() && (memory == nullptr || !memory->isMaterialized() || !memory->getHasPrivateMemory()))
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingMemoryPolicy, L"Secure domain requires a materialized secure memory descriptor with private memory policy.");
		}

		if (memory != nullptr && memory->isMaterialized() && memory->getDomainTag() != descriptor->getDomainTag())
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMemoryDomainBindingMismatch, L"Secure memory descriptor domain tag must match the secure domain descriptor.");
		}

		if ((operationClass == SecureDomainOperationClass::CreateEvidence || operationClass == SecureDomainOperationClass::PublishCompletion || operationClass == SecureDomainOperationClass::PublishRetireSideEffect))
		{
			std::shared_ptr<SecureEvidencePolicy> evidence = descriptor->getEvidenceVisibilityPolicy();
			if (!evidence->getAllowGuestVisibleEvidence() && !evidence->getAllowMigrationSerializableEvidence() && !evidence->getAllowCompatibilityAliasEvidence() && !evidence->getAllowDebugEvidence())
			{
				return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingEvidencePolicy, L"Secure evidence operation requires an explicit evidence visibility policy.");
			}
		}

		if (operationClass == SecureDomainOperationClass::SecureMigration && !descriptor->getMigrationPolicy()->getAllowsMigration())
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingMigrationPolicy, L"Secure migration requires an explicit migration policy.");
		}

		if (operationClass == SecureDomainOperationClass::NestedSecureDomain)
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedUnsupportedOperationClass, L"Nested secure-domain execution has no admitted policy in this runtime.");
		}

		if (operationClass == SecureDomainOperationClass::CompatibilityProjection && (!descriptor->getCompatibilityProjectionPolicy()->getAllowReadOnlyAliases() || !descriptor->getEvidenceVisibilityPolicy()->getAllowCompatibilityAliasEvidence() || !descriptor->getMigrationPolicy()->getAllowCompatibilityProjectionMetadata()))
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingEvidencePolicy, L"Compatibility projection requires explicit compatibility, evidence, and migration metadata policies.");
		}

		if (operationClass == SecureDomainOperationClass::SecureIo && !descriptor->getIoPolicy()->getNeutralIoOwnerMaterialized())
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingIoPolicy, L"Secure I/O operation requires a neutral secure I/O owner policy.");
		}

		if (operationClass == SecureDomainOperationClass::SecureHypercall && !descriptor->getHypercallPolicy()->getHasPolicy())
		{
			return SecureDomainAdmissionResult::Denied(SecureDomainAdmissionDecision::DeniedMissingHypercallPolicy, L"Secure hypercall operation requires a secure hypercall policy.");
		}

		// These are legacy descriptor-policy results only, never machine authority.
		// Adding a classification must not silently inherit a positive policy result.
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return operationClass switch
	//			{
	//				SecureDomainOperationClass.EnterSecureDomain => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.TouchSecureMemory => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.CreateEvidence => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.PublishCompletion => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.PublishRetireSideEffect => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.SecureIo => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.SecureHypercall => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.SecureMigration => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				SecureDomainOperationClass.CompatibilityProjection => SecureDomainAdmissionResult.AllowedSecureOperation,
	//				_ => SecureDomainAdmissionResult.Denied(
	//					SecureDomainAdmissionDecision.DeniedUnsupportedOperationClass, "Secure-domain operation class has no explicit policy handler.")
	//			};
	}
}
