#include "MemoryOwnedVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../Governance/Virtualization/MemoryOwnedVmReadScalarDeliveryAcceptedPolicyResolver.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Governance/Virtualization/MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2.h"
#include <any>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal MemoryOwnedVmReadScalarDeliveryCanonicalComposition(MemoryDomainRuntime memoryRuntime, DomainRuntimeContext context, RootAuthorityDescriptor root, EvidencePolicyDescriptor evidence, ulong restoreGeneration, in MemoryOwnedVmReadScalarDeliveryPolicyLookup lookup)
	MemoryOwnedVmReadScalarDeliveryCanonicalComposition::MemoryOwnedVmReadScalarDeliveryCanonicalComposition(std::shared_ptr<MemoryDomainRuntime> memoryRuntime, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, unsigned long long restoreGeneration, std::shared_ptr<MemoryOwnedVmReadScalarDeliveryPolicyLookup> lookup)
		: PolicyResolution(MemoryOwnedVmReadScalarDeliveryAcceptedPolicyResolver::Resolve(lookup)),
		  _memoryRuntime((memoryRuntime != nullptr) ? memoryRuntime : throw std::invalid_argument("memoryRuntime")),
		  _root((root != nullptr) ? root : throw std::invalid_argument("root")),
		  _evidence((evidence != nullptr) ? evidence : throw std::invalid_argument("evidence"))
	{
	if (context != nullptr)
	{
		_context = context;
	}
	else
	{
		throw std::invalid_argument("context");
	}
		_restoreGeneration = restoreGeneration;
	}

	std::shared_ptr<MemoryOwnedVmReadScalarDeliveryPolicyResolution> MemoryOwnedVmReadScalarDeliveryCanonicalComposition::getPolicyResolution() const
	{
		return PolicyResolution;
	}

	bool MemoryOwnedVmReadScalarDeliveryCanonicalComposition::isEnabled() const
	{
		{
		std::scoped_lock<std::mutex> lock(_sync);
			return _enabled;
		}
	}

	unsigned long long MemoryOwnedVmReadScalarDeliveryCanonicalComposition::getGeneration() const
	{
		{
		std::scoped_lock<std::mutex> lock(_sync);
			return _generation;
		}
	}

	bool MemoryOwnedVmReadScalarDeliveryCanonicalComposition::EnableExact()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (!getPolicyResolution() || !getPolicyResolution()->isResolved() || _enabled || !HasCurrentSourceBinding(_context))
			{
				return false;
			}
			_enabled = true;
			return true;
		}
	}

	void MemoryOwnedVmReadScalarDeliveryCanonicalComposition::Disable()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			_enabled = false;
			AdvanceGeneration();
		}
	}

	bool MemoryOwnedVmReadScalarDeliveryCanonicalComposition::RefreshSourceContext(std::shared_ptr<DomainRuntimeContext> context)
	{
		ArgumentNullException::ThrowIfNull(context);
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (context->getDomainTag() != _context->getDomainTag() || context->getAddressSpaceTag() != _context->getAddressSpaceTag() || !HasCurrentSourceBinding(context))
			{
				return false;
			}
			_context = context;
			return true;
		}
	}

	void MemoryOwnedVmReadScalarDeliveryCanonicalComposition::ReplaceAfterRestore(std::shared_ptr<DomainRuntimeContext> context, unsigned long long restoreGeneration)
	{
		ArgumentNullException::ThrowIfNull(context);
		{
			std::scoped_lock<std::mutex> lock(_sync);
			_context = context;
			_restoreGeneration = restoreGeneration;
			_enabled = false;
			AdvanceGeneration();
		}
	}

	void MemoryOwnedVmReadScalarDeliveryCanonicalComposition::ObserveReplayPhase(ReplayPhaseContext const phase)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
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
	std::shared_ptr<VmReadScalarDeliveryResult> MemoryOwnedVmReadScalarDeliveryCanonicalComposition::Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (!_enabled)
			{
				return Deny(VmReadScalarDeliveryDecision::Disabled, L"Exact memory-owned scalar-delivery profile is disabled.");
			}
			if (!getPolicyResolution() || !getPolicyResolution()->isResolved())
			{
				return Deny(VmReadScalarDeliveryDecision::PolicyDenied, getPolicyResolution()->Reason);
			}
			if (restoreGeneration != _restoreGeneration)
			{
				_enabled = false;
				AdvanceGeneration();
				return Deny(VmReadScalarDeliveryDecision::StaleReceipt, L"Restore generation changed; exact memory-owned profile was disabled pending source rebind.");
			}
			if (!_hasReplayObservation || !_replayActive || _replayEpoch != replayPhase.getEpochId() || carrier == nullptr || attempt == nullptr || attempt->getOperation() != VmxOperationKind::VmRead || attempt->getAttemptId() == 0 || attempt->getBundleIdentity() == 0 || attempt->getReplayEpoch() != replayPhase.getEpochId() || attempt->getDomainTag() != _context->getDomainTag() || carrier->getVirtualizationAdmission() != attempt->getCertificate())
			{
				return Deny(VmReadScalarDeliveryDecision::AdmissionDenied, L"Live canonical VMREAD E1 identity is required.");
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (fieldSelector > ushort.MaxValue || unchecked((VmcsField)(ushort)fieldSelector) is not(VmcsField.GuestCr3 or VmcsField.EptPointer or VmcsField.Vpid or VmcsField.Cr3TargetCount))
			const auto fieldId = static_cast<unsigned short>(fieldSelector);
			const VmcsField requestedField = static_cast<VmcsField>(fieldId);
			if (fieldSelector > std::numeric_limits<unsigned short>::max() || (requestedField != VmcsField::GuestCr3 && requestedField != VmcsField::EptPointer && requestedField != VmcsField::Vpid && requestedField != VmcsField::Cr3TargetCount))
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"Only exact GuestCr3, EptPointer, Vpid, or Cr3TargetCount is admitted.");
			}
			if (carrier->getRd() == 0 || carrier->getRd() > 31 || carrier->getRs2() != 0)
			{
				return Deny(VmReadScalarDeliveryDecision::DestinationDenied, L"VMREAD scalar delivery requires x1-x31 and reserved x0 Rs2.");
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: VmcsField field = unchecked((VmcsField)(ushort)fieldSelector);
			VmcsField field = static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector));
			std::shared_ptr<MemoryDomainSourceCaptureResult> source = _memoryRuntime->CaptureVmReadScalarSource(_context->getMemory(), _context->getDomainTag(), _context->getAddressSpaceTag(), field);
			if (!source || !source->isCaptured() || !_memoryRuntime->IsAuthenticCapture(source->Capture))
			{
				return Deny(VmReadScalarDeliveryDecision::SourceDenied, source ? source->Reason : L"Memory-domain source capture returned no result.");
			}

			std::shared_ptr<VmxCompatibilityVmReadAdmissionRequest> projectionRequest = std::make_shared<VmxCompatibilityVmReadAdmissionRequest>(_context, _root, _evidence, nullptr, fieldId, carrier->getRd(), carrier->getRs1(), carrier->getRs2(), true, true, true, true, true);
			std::shared_ptr<VmxCompatibilityVmReadAdmissionResult> projection = _admission->AdmitVmReadProjection(projectionRequest);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (!projection.IsReadOnlyValueProjected || unchecked((ulong)projection.Value) != source.Capture.Value)
			if (!projection || !projection->IsReadOnlyValueProjected || static_cast<unsigned long long>(projection->Value) != source->Capture->getValue())
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, projection && projection->IsReadOnlyValueProjected ? L"Canonical compatibility projection mismatched the atomic MemoryDomain source capture." : (projection ? projection->Reason : L"Compatibility projection returned no result."));
			}

			std::shared_ptr<VmReadScalarResultReceipt> receipt = std::make_shared<VmReadScalarResultReceipt>(shared_from_this(), attempt, MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId, std::any(source->Capture->getSourceOwner()), source->Capture->getAddressSpaceGeneration(), source->Capture, _generation, attempt->getAttemptId(), attempt->getIssuerGeneration(), attempt->getBundleIdentity(), attempt->getReplayEpoch(), restoreGeneration, _context->getDomainTag(), _context->getAddressSpaceTag(), nullptr, field, carrier->getRd(), source->Capture->getValue());
			return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact scalar value was captured from authoritative MemoryDomain translation state and bound to canonical delivery.");
		}
	}

	bool MemoryOwnedVmReadScalarDeliveryCanonicalComposition::ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return ValidateBinding(receipt, currentRestoreGeneration) && !receipt->isConsumed();
		}
	}

	bool MemoryOwnedVmReadScalarDeliveryCanonicalComposition::ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return ValidateBinding(receipt, currentRestoreGeneration);
		}
	}

	bool MemoryOwnedVmReadScalarDeliveryCanonicalComposition::ValidateBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryDomainRuntime.SourceCapture? capture = receipt.MemoryDomainCapture;
		if (!receipt)
		{
			return false;
		}
		std::shared_ptr<MemoryDomainRuntime::SourceCapture> capture = receipt->getMemoryDomainCapture();
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		const std::any sourceOwner = receipt->getSourceOwner();
		const auto* capturedOwner = std::any_cast<std::shared_ptr<MemoryDomainDescriptor>>(&sourceOwner);
		return _enabled && getPolicyResolution() && getPolicyResolution()->isResolved() && receipt->getDecisionId() == MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId && receipt->getProfileGeneration() == _generation && receipt->getAttemptId() != 0 && receipt->getDomainTag() == _context->getDomainTag() && receipt->getAddressSpaceTag() == _context->getAddressSpaceTag() && receipt->getRestoreGeneration() == _restoreGeneration && (!currentRestoreGeneration || receipt->getRestoreGeneration() == currentRestoreGeneration.value()) && capture != nullptr && _memoryRuntime->IsAuthenticCapture(capture) && capturedOwner != nullptr && *capturedOwner == capture->getSourceOwner() && receipt->getSourceEpoch() == capture->getAddressSpaceGeneration() && receipt->getField() == capture->getField() && receipt->getValue() == capture->getValue() && receipt->getDomainTag() == capture->getDomainTag() && receipt->getAddressSpaceTag() == capture->getRuntimeAddressSpaceTag();
	}

	bool MemoryOwnedVmReadScalarDeliveryCanonicalComposition::HasCurrentSourceBinding(std::shared_ptr<DomainRuntimeContext> context)
	{
		return context->getDomainTag() != 0 && context->getAddressSpaceTag() != 0 && context->getMemory() != nullptr && context->getMemory() == _memoryRuntime->getCurrentTranslationSource() && _memoryRuntime->getCurrentAddressSpaceGeneration() != 0 && context->getMemory()->getTranslationControl()->AddressSpaceGeneration == _memoryRuntime->getCurrentAddressSpaceGeneration() && context->getMemory()->getTranslationControl()->DomainTag == context->getDomainTag();
	}

	void MemoryOwnedVmReadScalarDeliveryCanonicalComposition::AdvanceGeneration()
	{
//		unchecked
			_generation++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		if (_generation == 0)
		{
			_generation = 1;
		}
	}

	std::shared_ptr<VmReadScalarDeliveryResult> MemoryOwnedVmReadScalarDeliveryCanonicalComposition::Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryResult>(decision, nullptr, reason);
	}
}
