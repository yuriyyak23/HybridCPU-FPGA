#include "AcceleratorOwnerDomainGuard.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth
{

	std::shared_ptr<AcceleratorOwnerDomainGuard> AcceleratorOwnerDomainGuard::getDefault()
	{
		return Default;
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EnsureBeforeDescriptorAcceptance(std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence)
	{
		return EvaluateGuard(AcceleratorGuardSurface::DescriptorAcceptance, descriptorOwnerBinding, guardEvidence, std::nullopt, false);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EnsureBeforeCapabilityAcceptance(std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence)
	{
		return EvaluateGuard(AcceleratorGuardSurface::CapabilityAcceptance, descriptorOwnerBinding, guardEvidence, std::nullopt, false);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EnsureBeforeCapabilityAcceptance(std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::shared_ptr<AcceleratorGuardDecision> descriptorGuardDecision)
	{
		ArgumentNullException::ThrowIfNull(descriptorGuardDecision);
		return EvaluateGuard(AcceleratorGuardSurface::CapabilityAcceptance, descriptorOwnerBinding, descriptorGuardDecision->Evidence, descriptorGuardDecision, false);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EnsureBeforeSubmit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		return EvaluateGuard(AcceleratorGuardSurface::SubmitAdmission, descriptor->OwnerBinding, guardEvidence, descriptor->OwnerGuardDecision, true);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EnsureBeforeDeviceExecution(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		return EvaluateGuard(AcceleratorGuardSurface::DeviceExecution, descriptor->OwnerBinding, guardEvidence, descriptor->OwnerGuardDecision, true);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EnsureBeforeCommit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		return EvaluateGuard(AcceleratorGuardSurface::Commit, descriptor->OwnerBinding, guardEvidence, descriptor->OwnerGuardDecision, true);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EnsureBeforeExceptionPublication(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		return EvaluateGuard(AcceleratorGuardSurface::ExceptionPublication, descriptor->OwnerBinding, guardEvidence, descriptor->OwnerGuardDecision, true);
	}

	std::shared_ptr<AcceleratorMappingEpoch> AcceleratorOwnerDomainGuard::CaptureMappingEpoch(std::shared_ptr<AcceleratorGuardEvidence> guardEvidence)
	{
		ArgumentNullException::ThrowIfNull(guardEvidence);
		if (guardEvidence->Source != AcceleratorGuardEvidenceSource::GuardPlane)
		{
			throw InvalidOperationException(L"L7-SDC mapping epoch evidence must come from the guard plane.");
		}

		return std::make_shared<AcceleratorMappingEpoch>(guardEvidence->MappingEpoch);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::ValidateMappingEpoch(std::shared_ptr<AcceleratorGuardDecision> acceptedGuardDecision, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (!acceptedGuardDecision || acceptedGuardDecision->DescriptorOwnerBinding == nullptr)
		{
			return AcceleratorGuardDecision::Reject(AcceleratorGuardSurface::MappingEpochValidation, AcceleratorGuardFault::MissingGuardEvidence, nullptr, currentGuardEvidence.value_or(nullptr), RejectKind::OwnerMismatch, L"L7-SDC mapping epoch validation requires a prior guard-backed descriptor decision.");
		}

		return EvaluateGuard(AcceleratorGuardSurface::MappingEpochValidation, acceptedGuardDecision->DescriptorOwnerBinding, currentGuardEvidence, acceptedGuardDecision, true);
	}

	std::shared_ptr<AcceleratorInvalidOwnerCompletionResult> AcceleratorOwnerDomainGuard::MarkAbandonedOnInvalidOwner(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &completionEvidence, bool recordPrivilegedDiagnostic)
	{
		std::shared_ptr<AcceleratorGuardDecision> completionGuard = EnsureBeforeCommit(descriptor, completionEvidence);
		return completionGuard->IsAllowed ? AcceleratorInvalidOwnerCompletionResult::GuardStillValid(completionGuard) : AcceleratorInvalidOwnerCompletionResult::InvalidOwner(completionGuard, recordPrivilegedDiagnostic);
	}

	bool AcceleratorOwnerDomainGuard::IsDescriptorGuardBacked(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::wstring &message)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		std::shared_ptr<AcceleratorGuardDecision> decision = descriptor->OwnerGuardDecision;
		if (!decision || !decision->IsAllowed)
		{
			message = !decision || StringHelper::isEmptyOrWhiteSpace(decision->Message) ? L"L7-SDC descriptor sideband lacks an accepted owner/domain guard decision." : decision->Message;
			return false;
		}

		if (decision->DescriptorOwnerBinding == nullptr || !decision->DescriptorOwnerBinding->Equals(descriptor->OwnerBinding))
		{
			message = L"L7-SDC descriptor guard decision does not match descriptor owner binding.";
			return false;
		}

		if ((!decision->Evidence || decision->Evidence->Source != AcceleratorGuardEvidenceSource::GuardPlane) || decision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || decision->LegalityDecision.getAttemptedReplayCertificateReuse())
		{
			message = L"L7-SDC descriptor acceptance requires guard-plane authority; evidence-plane identity cannot authorize sideband admission.";
			return false;
		}

		message = L"L7-SDC descriptor sideband is guard-backed.";
		return true;
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::EvaluateGuard(AcceleratorGuardSurface surface, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, const std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence, const std::optional<std::shared_ptr<AcceleratorGuardDecision>> &priorAcceptedDecision, bool validateEpochs)
	{
		ArgumentNullException::ThrowIfNull(descriptorOwnerBinding);

		if (!guardEvidence || !*guardEvidence)
		{
			return Reject(surface, AcceleratorGuardFault::MissingGuardEvidence, descriptorOwnerBinding, guardEvidence, RejectKind::OwnerMismatch, std::wstring(L"L7-SDC owner/domain guard evidence is required before authority acceptance."));
		}

		if ((*guardEvidence)->Source != AcceleratorGuardEvidenceSource::GuardPlane)
		{
			return Reject(surface, AcceleratorGuardFault::EvidenceSourceNotAuthority, descriptorOwnerBinding, guardEvidence, RejectKind::OwnerMismatch, std::wstring(L"The evidence source is evidence only and cannot authorize L7-SDC owner/domain acceptance."));
		}

		if (priorAcceptedDecision && *priorAcceptedDecision)
		{
			std::shared_ptr<AcceleratorGuardDecision> prior = priorAcceptedDecision.value();
			if (!prior->IsAllowed)
			{
				return Reject(surface, AcceleratorGuardFault::RejectedGuard, descriptorOwnerBinding, guardEvidence, prior->LegalityDecision.getRejectKind() == RejectKind::None ? RejectKind::OwnerMismatch : prior->LegalityDecision.getRejectKind(), std::wstring(L"prior L7-SDC guard decision was not accepted."));
			}

			if (prior->DescriptorOwnerBinding == nullptr || !prior->DescriptorOwnerBinding->Equals(descriptorOwnerBinding))
			{
				return Reject(surface, AcceleratorGuardFault::DescriptorOwnerBindingMismatch, descriptorOwnerBinding, guardEvidence, RejectKind::OwnerMismatch, std::wstring(L"prior L7-SDC guard decision does not match descriptor owner binding."));
			}

			if ((!prior->Evidence || prior->Evidence->Source != AcceleratorGuardEvidenceSource::GuardPlane) || prior->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || prior->LegalityDecision.getAttemptedReplayCertificateReuse())
			{
				return Reject(surface, AcceleratorGuardFault::EvidenceSourceNotAuthority, descriptorOwnerBinding, guardEvidence, RejectKind::OwnerMismatch, std::wstring(L"prior L7-SDC guard decision was not produced by the guard plane."));
			}

			if (validateEpochs)
			{
				if (prior->getMappingEpoch() != (*guardEvidence)->MappingEpoch)
				{
					return Reject(surface, AcceleratorGuardFault::MappingEpochDrift, descriptorOwnerBinding, guardEvidence, RejectKind::EpochMismatch, std::wstring(L"L7-SDC mapping epoch drift prevents authority reuse."));
				}

				if (prior->getIommuDomainEpoch() != (*guardEvidence)->IommuDomainEpoch)
				{
					return Reject(surface, AcceleratorGuardFault::IommuDomainEpochDrift, descriptorOwnerBinding, guardEvidence, RejectKind::EpochMismatch, std::wstring(L"L7-SDC IOMMU-domain epoch drift prevents authority reuse."));
				}
			}
		}

		if (descriptorOwnerBinding->OwnerVirtualThreadId != (*guardEvidence)->OwnerVirtualThreadId || descriptorOwnerBinding->OwnerContextId != (*guardEvidence)->OwnerContextId || descriptorOwnerBinding->OwnerCoreId != (*guardEvidence)->OwnerCoreId || descriptorOwnerBinding->OwnerPodId != (*guardEvidence)->OwnerPodId)
		{
			return Reject(surface, AcceleratorGuardFault::OwnerMismatch, descriptorOwnerBinding, guardEvidence, RejectKind::OwnerMismatch, std::wstring(L"L7-SDC descriptor owner binding does not match guard-plane owner evidence."));
		}

		if (descriptorOwnerBinding->DomainTag != (*guardEvidence)->DomainTag.Value)
		{
			return Reject(surface, AcceleratorGuardFault::DomainMismatch, descriptorOwnerBinding, guardEvidence, RejectKind::DomainMismatch, std::wstring(L"L7-SDC descriptor domain tag does not match guard-plane domain evidence."));
		}

		if (!IsDomainCoveredByCertificate((*guardEvidence)->DomainTag.Value, (*guardEvidence)->ActiveDomainCertificate))
		{
			return Reject(surface, AcceleratorGuardFault::DomainMismatch, descriptorOwnerBinding, guardEvidence, RejectKind::DomainMismatch, std::wstring(L"L7-SDC descriptor domain is not covered by the active guard-plane domain certificate."));
		}

		return AcceleratorGuardDecision::Allow(surface, descriptorOwnerBinding, *guardEvidence, std::wstring(L"L7-SDC owner/domain guard succeeded."));
	}

	bool AcceleratorOwnerDomainGuard::IsDomainCoveredByCertificate(unsigned long long ownerDomainTag, unsigned long long activeDomainCertificate)
	{
		if (ownerDomainTag == 0)
		{
			return activeDomainCertificate == 0;
		}

		return activeDomainCertificate == 0 || (ownerDomainTag & activeDomainCertificate) != 0;
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorOwnerDomainGuard::Reject(AcceleratorGuardSurface surface, AcceleratorGuardFault fault, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, const std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence, RejectKind rejectKind, const std::wstring &message)
	{
		return AcceleratorGuardDecision::Reject(surface, fault, descriptorOwnerBinding, guardEvidence.value_or(nullptr), rejectKind, message);
	}
}
