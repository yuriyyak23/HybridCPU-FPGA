#include "CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../Governance/Virtualization/Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Governance/Virtualization/CompletionReasonQualificationVmReadDecisionValidatorV2.h"
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

	bool IsCompletionReasonQualificationPolicyAccepted()
	{
		const auto result = YAKSys_Hybrid_CPU::Core::Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::ValidateRepositoryArtifact(
			L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		return result && result->IsAcceptedPolicyObject;
	}
}

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition(ArchitecturalCompletionCommitOwner completionOwner, ArchitecturalCompletionCommitOwner.ProducerRegistration exactProducer, bool enabled)
	CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition(std::shared_ptr<ArchitecturalCompletionCommitOwner> completionOwner, std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> exactProducer, bool enabled)
		: PolicyAccepted(IsCompletionReasonQualificationPolicyAccepted()),
		  _completionOwner(RequireCompletionOwner(std::move(completionOwner))),
		  _projection(std::make_shared<NeutralCompletionReasonQualificationProjectionOwner>(RequireCompletionProducer(std::move(exactProducer)))),
		  _enabled(enabled ? 1 : 0)
	{
	}

	bool CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::getPolicyAccepted() const
	{
		return PolicyAccepted;
	}

	bool CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::isEnabled() const
	{
		return _enabled.load(std::memory_order_acquire) != 0 && getPolicyAccepted();
	}

	bool CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::Disable()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (_enabled.load(std::memory_order_relaxed) == 0)
			{
				return true;
			}

			_enabled.store(0, std::memory_order_release);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _generation = checked(_generation + 1);
			_generation = _generation + 1;
			return true;
		}
	}

	std::shared_ptr<VmReadScalarDeliveryResult> CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (!isEnabled())
			{
				return Deny(VmReadScalarDeliveryDecision::Disabled, L"Exact completion projection is default-disabled.");
			}
			if (!replayPhase.isActive() || attempt->getOperation() != VmxOperationKind::VmRead || attempt->getAttemptId() == 0 || attempt->getReplayEpoch() != replayPhase.getEpochId() || carrier->getVirtualizationAdmission() != attempt->getCertificate())
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
			if ((field == VmcsField::GuestPhysicalAddress || field == VmcsField::EptViolationQualification))
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"GPA and EPT violation qualification lack neutral semantic coverage.");
			}
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if ((field != VmcsField::ExitReason && field != VmcsField::ExitQualification))
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"Only exact ExitReason and ExitQualification are admitted.");
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
			std::shared_ptr<NeutralCompletionReasonQualificationProjection> mapped = _projection->Project(snapshot);
			if (!mapped)
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, L"Completion projection returned no decision.");
			}
			if (!mapped->isProjected())
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, mapped->Reason);
			}
			unsigned long long value = field == VmcsField::ExitReason ? static_cast<unsigned long long>(mapped->ExitReason) : mapped->ExitQualification;
			std::shared_ptr<VmReadScalarResultReceipt> receipt = std::make_shared<VmReadScalarResultReceipt>(shared_from_this(), attempt, CompletionReasonQualificationVmReadDecisionValidatorV2::ExpectedDecisionId, _completionOwner->getObservationOwner(), snapshot->CompletionGeneration, snapshot, _generation, attempt->getAttemptId(), attempt->getIssuerGeneration(), attempt->getBundleIdentity(), attempt->getReplayEpoch(), restoreGeneration, attempt->getDomainTag(), field, carrier->getRd(), value);
			return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact committed neutral completion tuple captured for compatibility-only scalar delivery.");
		}
	}

	bool CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return Validate(receipt, currentRestoreGeneration) && !receipt->isConsumed();
		}
	}

	bool CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return Validate(receipt, currentRestoreGeneration);
		}
	}

	bool CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::Validate(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		return receipt && isEnabled() && receipt->getProfileGeneration() == _generation && receipt->getDecisionId() == CompletionReasonQualificationVmReadDecisionValidatorV2::ExpectedDecisionId && receipt->MatchesCurrentCompletionObservation(_completionOwner->getObservationOwner(), currentRestoreGeneration);
	}

	std::shared_ptr<VmReadScalarDeliveryResult> CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition::Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryResult>(decision, nullptr, reason);
	}
}
