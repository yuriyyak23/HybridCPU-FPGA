#include "GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../Governance/Virtualization/GuestPcSpFlagsVmReadScalarDeliveryAcceptedPolicyResolver.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Governance/Virtualization/GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition(ExecutionDomainRuntime executionRuntime, DomainRuntimeContext context, RootAuthorityDescriptor root, EvidencePolicyDescriptor evidence, ulong restoreGeneration, in GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup lookup)
	GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition(std::shared_ptr<ExecutionDomainRuntime> executionRuntime, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, unsigned long long restoreGeneration, std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup> lookup)
		: PolicyResolution(GuestPcSpFlagsVmReadScalarDeliveryAcceptedPolicyResolver::Resolve(std::move(lookup))),
		  _executionRuntime(executionRuntime ? std::move(executionRuntime) : throw std::invalid_argument("executionRuntime")),
		  _root(root ? std::move(root) : throw std::invalid_argument("root")),
		  _evidence(evidence ? std::move(evidence) : throw std::invalid_argument("evidence")),
		  _context(context ? std::move(context) : throw std::invalid_argument("context")),
		  _restoreGeneration(restoreGeneration)
	{
	}

	std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution> GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::getPolicyResolution() const
	{
		return PolicyResolution;
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::isEnabled() const
	{
		{
		std::lock_guard<std::mutex> lock(_sync);
			return _enabled;
		}
	}

	unsigned long long GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::getGeneration() const
	{
		{
		std::lock_guard<std::mutex> lock(_sync);
			return _generation;
		}
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::EnableExact()
	{
		{
			std::lock_guard<std::mutex> lock(_sync);
			if (!getPolicyResolution()->isResolved() || _enabled || !HasCurrentSourceBinding(_context))
			{
				return false;
			}
			_enabled = true;
			return true;
		}
	}

	void GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::Disable()
	{
		{
			std::lock_guard<std::mutex> lock(_sync);
			_enabled = false;
			AdvanceGeneration();
		}
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::RefreshSourceContext(std::shared_ptr<DomainRuntimeContext> context)
	{
		ArgumentNullException::ThrowIfNull(context);
		{
			std::lock_guard<std::mutex> lock(_sync);
			if (context->getDomainTag() != _context->getDomainTag() || context->getAddressSpaceTag() != _context->getAddressSpaceTag() || !HasCurrentSourceBinding(context))
			{
				return false;
			}
			_context = context;
			return true;
		}
	}

	void GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::ReplaceAfterRestore(std::shared_ptr<DomainRuntimeContext> context, unsigned long long restoreGeneration)
	{
		ArgumentNullException::ThrowIfNull(context);
		{
			std::lock_guard<std::mutex> lock(_sync);
			_context = context;
			_restoreGeneration = restoreGeneration;
			_enabled = false;
			AdvanceGeneration();
		}
	}

	void GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::ObserveReplayPhase(ReplayPhaseContext const phase)
	{
		{
			std::lock_guard<std::mutex> lock(_sync);
			bool changed = _hasReplayObservation && (_replayActive != phase.isActive() || _replayEpoch != phase.getEpochId() || _replayCachedPc != phase.getCachedPc() || _completedReplays != phase.getCompletedReplays());
			_hasReplayObservation = true;
			_replayActive = phase.isActive();
			_replayEpoch = phase.getEpochId();
			_replayCachedPc = phase.getCachedPc();
			_completedReplays = phase.getCompletedReplays();
			if (changed)
			{
				AdvanceGeneration();
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VmReadScalarDeliveryResult Prepare(ReplayPhaseContext replayPhase, VmxMicroOp carrier, VmReadScalarAttemptBinding? attempt, ulong fieldSelector, ulong restoreGeneration)
	std::shared_ptr<VmReadScalarDeliveryResult> GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		{
			std::lock_guard<std::mutex> lock(_sync);
			if (!_enabled)
			{
				return Deny(VmReadScalarDeliveryDecision::Disabled, L"Exact GuestPc/GuestSp/GuestFlags scalar-delivery profile is disabled.");
			}
			if (!getPolicyResolution()->isResolved())
			{
				return Deny(VmReadScalarDeliveryDecision::PolicyDenied, getPolicyResolution()->Reason);
			}
			if (restoreGeneration != _restoreGeneration)
			{
				_enabled = false;
				AdvanceGeneration();
				return Deny(VmReadScalarDeliveryDecision::StaleReceipt, L"Restore generation changed; exact profile was disabled pending source rebind.");
			}
			if (!_hasReplayObservation || !_replayActive || _replayEpoch != replayPhase.getEpochId() || attempt == nullptr || attempt->getOperation() != VmxOperationKind::VmRead || attempt->getAttemptId() == 0 || attempt->getBundleIdentity() == 0 || attempt->getReplayEpoch() != replayPhase.getEpochId() || attempt->getDomainTag() != _context->getDomainTag() || carrier->getVirtualizationAdmission() != attempt->getCertificate())
			{
				return Deny(VmReadScalarDeliveryDecision::AdmissionDenied, L"Live canonical VMREAD E1 identity is required.");
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (fieldSelector > ushort.MaxValue || unchecked((VmcsField)(ushort)fieldSelector) is not(VmcsField.GuestPc or VmcsField.GuestSp or VmcsField.GuestFlags))
			const VmcsField selectedField = static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector));
			if (fieldSelector > std::numeric_limits<unsigned short>::max() ||
				(selectedField != VmcsField::GuestPc && selectedField != VmcsField::GuestSp && selectedField != VmcsField::GuestFlags))
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"Only exact GuestPc, GuestSp, or GuestFlags is admitted.");
			}
			if (carrier->getRd() == 0 || carrier->getRd() > 31 || carrier->getRs2() != 0)
			{
				return Deny(VmReadScalarDeliveryDecision::DestinationDenied, L"VMREAD scalar delivery requires x1-x31 and reserved x0 Rs2.");
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: VmcsField field = unchecked((VmcsField)(ushort)fieldSelector);
			VmcsField field = static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector));
			std::shared_ptr<ExecutionDomainSourceCaptureResult> source = _executionRuntime->CaptureVmReadScalarSource(_context->getExecution(), _context->getDomainTag(), _context->getAddressSpaceTag(), field);
			if (!source || !source->isCaptured() || source->Capture == nullptr || !_executionRuntime->IsAuthenticCapture(source->Capture))
			{
				return Deny(VmReadScalarDeliveryDecision::SourceDenied, source ? source->Reason : L"Execution-domain source capture returned no result.");
			}

			std::shared_ptr<VmxCompatibilityVmReadAdmissionResult> projection = _admission->AdmitVmReadProjection(std::make_shared<VmxCompatibilityVmReadAdmissionRequest>(_context, _root, _evidence, nullptr, static_cast<unsigned short>(field), carrier->getRd(), carrier->getRs1(), carrier->getRs2(), true, true, true, true, true));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (!projection.IsReadOnlyValueProjected || unchecked((ulong)projection.Value) != source.Capture.Value)
			if (!projection || !projection->IsReadOnlyValueProjected || static_cast<unsigned long long>(projection->Value) != source->Capture->getValue())
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, projection && projection->IsReadOnlyValueProjected ? L"Canonical compatibility projection mismatched the atomic ExecutionDomain source capture." : (projection ? projection->Reason : L"Canonical compatibility projection returned no result."));
			}

			std::shared_ptr<VmReadScalarResultReceipt> receipt = std::make_shared<VmReadScalarResultReceipt>(shared_from_this(), attempt, GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId, source->Capture->getSourceOwner(), source->Capture->getSourceEpoch().getValue(), source->Capture, _generation, attempt->getAttemptId(), attempt->getIssuerGeneration(), attempt->getBundleIdentity(), attempt->getReplayEpoch(), restoreGeneration, _context->getDomainTag(), _context->getAddressSpaceTag(), nullptr, field, carrier->getRd(), source->Capture->getValue());
			return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact scalar value was captured from the authoritative ExecutionDomain read-only state and bound to canonical delivery.");
		}
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		{
			std::lock_guard<std::mutex> lock(_sync);
			return ValidateBinding(receipt, currentRestoreGeneration) && !receipt->isConsumed();
		}
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration)
	{
		{
			std::lock_guard<std::mutex> lock(_sync);
			return ValidateBinding(receipt, currentRestoreGeneration);
		}
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::ValidateBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: ExecutionDomainRuntime.SourceCapture? capture = receipt.ExecutionDomainCapture;
		std::shared_ptr<ExecutionDomainRuntime::SourceCapture> capture = receipt->getExecutionDomainCapture();
		std::any sourceOwner = receipt->getSourceOwner();
		const auto *typedSourceOwner = std::any_cast<std::shared_ptr<ExecutionDomainDescriptor>>(&sourceOwner);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		return _enabled && getPolicyResolution()->isResolved() && receipt->getDecisionId() == GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId && receipt->getProfileGeneration() == _generation && receipt->getAttemptId() != 0 && receipt->getDomainTag() == _context->getDomainTag() && receipt->getAddressSpaceTag() == _context->getAddressSpaceTag() && receipt->getRestoreGeneration() == _restoreGeneration && (!currentRestoreGeneration || receipt->getRestoreGeneration() == currentRestoreGeneration.value()) && capture != nullptr && _executionRuntime->IsAuthenticCapture(capture) && typedSourceOwner != nullptr && *typedSourceOwner == capture->getSourceOwner() && receipt->getSourceEpoch() == capture->getSourceEpoch().getValue() && receipt->getField() == capture->getField() && receipt->getValue() == capture->getValue() && receipt->getDomainTag() == capture->getDomainTag() && receipt->getAddressSpaceTag() == capture->getAddressSpaceTag();
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::HasCurrentSourceBinding(std::shared_ptr<DomainRuntimeContext> context)
	{
		return context->getDomainTag() != 0 && context->getAddressSpaceTag() != 0 && context->getExecution() != nullptr && context->getExecution() == _executionRuntime->getCurrentSourceDescriptor() && _executionRuntime->getCurrentSourceEpoch().isMaterialized() && context->getExecution()->getReadOnlyState()->StateEpoch == _executionRuntime->getCurrentSourceEpoch().getValue();
	}

	void GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::AdvanceGeneration()
	{
//		unchecked
			_generation++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		if (_generation == 0)
		{
			_generation = 1;
		}
	}

	std::shared_ptr<VmReadScalarDeliveryResult> GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition::Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryResult>(decision, nullptr, reason);
	}
}
