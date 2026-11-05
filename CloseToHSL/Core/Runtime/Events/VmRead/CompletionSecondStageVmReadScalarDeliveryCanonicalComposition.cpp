#include "CompletionSecondStageVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../Governance/Virtualization/Phase57CompletionSecondStageVmReadDecisionAcceptanceV2.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Governance/Virtualization/CompletionSecondStageVmReadDecisionValidatorV2.h"
#include <utility>

namespace
{
	std::shared_ptr<YAKSys_Hybrid_CPU::Core::ArchitecturalCompletionCommitOwner> RequireCompletionOwner(
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::ArchitecturalCompletionCommitOwner> owner)
	{
		if (!owner) throw std::invalid_argument("completionOwner");
		return owner;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::ArchitecturalCompletionCommitOwner::ProducerRegistration> RequireCompletionProducer(
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::ArchitecturalCompletionCommitOwner::ProducerRegistration> producer)
	{
		if (!producer) throw std::invalid_argument("exactProducer");
		return producer;
	}

	bool IsCompletionSecondStagePolicyAccepted()
	{
		const auto result = YAKSys_Hybrid_CPU::Core::Phase57CompletionSecondStageVmReadDecisionAcceptanceV2::ValidateRepositoryArtifact(
			L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		return result && result->IsAcceptedPolicyObject;
	}
}

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal CompletionSecondStageVmReadScalarDeliveryCanonicalComposition(ArchitecturalCompletionCommitOwner completionOwner, ArchitecturalCompletionCommitOwner.ProducerRegistration exactProducer, bool enabled)
	CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::CompletionSecondStageVmReadScalarDeliveryCanonicalComposition(
		std::shared_ptr<ArchitecturalCompletionCommitOwner> completionOwner,
		std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> exactProducer, bool enabled)
		: PolicyAccepted(IsCompletionSecondStagePolicyAccepted()),
		  _completionOwner(RequireCompletionOwner(std::move(completionOwner))),
		  _projection(std::make_shared<NeutralCompletionSecondStageProjectionOwner>(RequireCompletionProducer(std::move(exactProducer)))),
		  _enabled(enabled)
	{
	}

	bool CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::getPolicyAccepted() const
	{
		return PolicyAccepted;
	}

	bool CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::isEnabled() const
	{
		return _enabled && getPolicyAccepted();
	}

	std::shared_ptr<VmReadScalarDeliveryResult> CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (!isEnabled())
			{
				return Deny(VmReadScalarDeliveryDecision::Disabled, L"Exact second-stage completion projection is default-disabled.");
			}
			if (!replayPhase.isActive() || !attempt || !carrier || attempt->getOperation() != VmxOperationKind::VmRead || attempt->getAttemptId() == 0 || attempt->getReplayEpoch() != replayPhase.getEpochId() || carrier->getVirtualizationAdmission() != attempt->getCertificate())
			{
				return Deny(VmReadScalarDeliveryDecision::AdmissionDenied, L"Live canonical VMREAD E1 attempt is required.");
			}
			if (fieldSelector > std::numeric_limits<unsigned short>::max())
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"Completion field selector is not exact.");
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: VmcsField field = unchecked((VmcsField)(ushort)fieldSelector);
			VmcsField field = static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if ((field != VmcsField::GuestPhysicalAddress && field != VmcsField::EptViolationQualification))
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"Only exact GPA and EPT violation qualification are admitted.");
			}
			if (carrier->getRd() == 0 || carrier->getRd() > 31 || carrier->getRs2() != 0)
			{
				return Deny(VmReadScalarDeliveryDecision::DestinationDenied, L"VMREAD scalar delivery requires x1-x31 and reserved x0 Rs2.");
			}

			std::shared_ptr<CompletionObservationScope> scope = std::make_shared<CompletionObservationScope>(attempt->getDomainTag(), carrier->getOwnerContextId(), carrier->getVirtualThreadId());
			std::shared_ptr<CompletionObservationResult> observation = _completionOwner->getObservationOwner()->Observe(scope);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (!observation || !observation->IsObserved || !observation->Snapshot || !*observation->Snapshot)
			{
				return Deny(VmReadScalarDeliveryDecision::SourceDenied, L"No committed neutral completion exists for the exact carrier scope.");
			}
			const auto snapshot = *observation->Snapshot;
			if (snapshot->RestoreGeneration != restoreGeneration || restoreGeneration != _completionOwner->getCurrentRestoreGeneration())
			{
				return Deny(VmReadScalarDeliveryDecision::StaleReceipt, L"Completion snapshot belongs to a stale restore generation.");
			}

			std::shared_ptr<NeutralCompletionSecondStageProjection> mapped = _projection->Project(snapshot);
			if (!mapped)
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, L"Completion projection returned no decision.");
			}
			if (!mapped->isProjected())
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, mapped->Reason);
			}
			unsigned long long value = field == VmcsField::GuestPhysicalAddress ? mapped->GuestPhysicalAddress : mapped->EptViolationQualification;
			std::shared_ptr<VmReadScalarResultReceipt> receipt = std::make_shared<VmReadScalarResultReceipt>(shared_from_this(), attempt, CompletionSecondStageVmReadDecisionValidatorV2::ExpectedDecisionId, _completionOwner->getObservationOwner(), snapshot->CompletionGeneration, snapshot, _generation, attempt->getAttemptId(), attempt->getIssuerGeneration(), attempt->getBundleIdentity(), attempt->getReplayEpoch(), restoreGeneration, attempt->getDomainTag(), field, carrier->getRd(), value);
			return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact committed neutral CPU second-stage tuple captured for compatibility-only scalar delivery.");
		}
	}

	bool CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return Validate(receipt, currentRestoreGeneration) && !receipt->isConsumed();
		}
	}

	bool CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return Validate(receipt, currentRestoreGeneration);
		}
	}

	bool CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::Validate(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (!receipt || !isEnabled() || receipt->getProfileGeneration() != _generation ||
			receipt->getDecisionId() != CompletionSecondStageVmReadDecisionValidatorV2::ExpectedDecisionId)
			return false;
		const auto capture = receipt->getCompletionObservationCapture();
		return capture && *capture && (*capture)->TranslationProvenance.hasSecondStageIdentity() &&
			receipt->MatchesCurrentCompletionObservation(_completionOwner->getObservationOwner(), currentRestoreGeneration);
	}

	std::shared_ptr<VmReadScalarDeliveryResult> CompletionSecondStageVmReadScalarDeliveryCanonicalComposition::Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryResult>(decision, nullptr, reason);
	}
}
