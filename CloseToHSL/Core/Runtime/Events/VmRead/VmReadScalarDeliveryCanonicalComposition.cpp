#include "VmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../Governance/Virtualization/VmReadScalarDeliveryAcceptedPolicyResolver.h"
#include "../../Governance/Virtualization/VmReadScalarDeliveryDecisionValidatorV2.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	VmReadScalarAttemptBinding::VmReadScalarAttemptBinding(std::shared_ptr<VirtualizationAdmissionCertificate> certificate)
		: Certificate(certificate ? std::move(certificate) : throw std::invalid_argument("certificate"))
	{
	}

	std::shared_ptr<VirtualizationAdmissionCertificate> VmReadScalarAttemptBinding::getCertificate() const
	{
		return Certificate;
	}

	unsigned long long VmReadScalarAttemptBinding::getAttemptId() const
	{
		return getCertificate()->getAttemptId();
	}

	unsigned long long VmReadScalarAttemptBinding::getIssuerGeneration() const
	{
		return getCertificate()->getIssuerGeneration();
	}

	unsigned long long VmReadScalarAttemptBinding::getBundleIdentity() const
	{
		return getCertificate()->getBundleIdentity();
	}

	unsigned long long VmReadScalarAttemptBinding::getReplayEpoch() const
	{
		return getCertificate()->getReplayEpoch();
	}

	unsigned long long VmReadScalarAttemptBinding::getDomainTag() const
	{
		return getCertificate()->getDomainTag();
	}

	VmxOperationKind VmReadScalarAttemptBinding::getOperation() const
	{
		return getCertificate()->getOperation();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VmReadScalarResultReceipt(IVmReadScalarResultReceiptOwner owner, VmReadScalarAttemptBinding attempt, string decisionId, Object sourceOwner, ulong sourceEpoch, ExecutionDomainRuntime.SourceCapture? executionDomainCapture, ulong profileGeneration, ulong attemptId, ulong issuerGeneration, ulong bundleIdentity, ulong replayEpoch, ulong restoreGeneration, ulong domainTag, ulong addressSpaceTag, PrivilegedExecutionStateEpoch descriptorEpoch, VmcsField field, byte destinationRegister, ulong value) : this(owner, attempt, decisionId, sourceOwner, sourceEpoch, executionDomainCapture, memoryDomainCapture: null, profileGeneration, attemptId, issuerGeneration, bundleIdentity, replayEpoch, restoreGeneration, domainTag, addressSpaceTag, descriptorEpoch, field, destinationRegister, value)
	VmReadScalarResultReceipt::VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<ExecutionDomainRuntime::SourceCapture> executionDomainCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<PrivilegedExecutionStateEpoch> descriptorEpoch, VmcsField field, unsigned char destinationRegister, unsigned long long value) : VmReadScalarResultReceipt(owner, attempt, decisionId, sourceOwner, sourceEpoch, executionDomainCapture, nullptr, std::nullopt, nullptr, profileGeneration, attemptId, issuerGeneration, bundleIdentity, replayEpoch, restoreGeneration, domainTag, addressSpaceTag, descriptorEpoch, field, destinationRegister, value)
	{
	}

	VmReadScalarResultReceipt::VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<NeutralCompletionObservationSnapshot> completionObservationCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, VmcsField field, unsigned char destinationRegister, unsigned long long value) : VmReadScalarResultReceipt(owner, attempt, decisionId, sourceOwner, sourceEpoch, nullptr, nullptr, completionObservationCapture ? std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>>(std::move(completionObservationCapture)) : std::nullopt, nullptr, profileGeneration, attemptId, issuerGeneration, bundleIdentity, replayEpoch, restoreGeneration, domainTag, 0, nullptr, field, destinationRegister, value)
	{
	}

	VmReadScalarResultReceipt::VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<CompatibilityControlPolicySnapshot> compatibilityControlCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, VmcsField field, unsigned char destinationRegister, unsigned long long value) : VmReadScalarResultReceipt(owner, attempt, decisionId, sourceOwner, sourceEpoch, nullptr, nullptr, std::nullopt, std::move(compatibilityControlCapture), profileGeneration, attemptId, issuerGeneration, bundleIdentity, replayEpoch, restoreGeneration, domainTag, 0, nullptr, field, destinationRegister, value)
	{
	}

	VmReadScalarResultReceipt::VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<MemoryDomainRuntime::SourceCapture> memoryDomainCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<PrivilegedExecutionStateEpoch> descriptorEpoch, VmcsField field, unsigned char destinationRegister, unsigned long long value) : VmReadScalarResultReceipt(owner, attempt, decisionId, sourceOwner, sourceEpoch, nullptr, memoryDomainCapture, std::nullopt, nullptr, profileGeneration, attemptId, issuerGeneration, bundleIdentity, replayEpoch, restoreGeneration, domainTag, addressSpaceTag, descriptorEpoch, field, destinationRegister, value)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private VmReadScalarResultReceipt(IVmReadScalarResultReceiptOwner owner, VmReadScalarAttemptBinding attempt, string decisionId, Object sourceOwner, ulong sourceEpoch, ExecutionDomainRuntime.SourceCapture? executionDomainCapture, System.Nullable<MemoryDomainRuntime.SourceCapture> memoryDomainCapture, ulong profileGeneration, ulong attemptId, ulong issuerGeneration, ulong bundleIdentity, ulong replayEpoch, ulong restoreGeneration, ulong domainTag, ulong addressSpaceTag, PrivilegedExecutionStateEpoch descriptorEpoch, VmcsField field, byte destinationRegister, ulong value)
	VmReadScalarResultReceipt::VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<ExecutionDomainRuntime::SourceCapture> executionDomainCapture, std::shared_ptr<MemoryDomainRuntime::SourceCapture> memoryDomainCapture, std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>> completionObservationCapture, std::shared_ptr<CompatibilityControlPolicySnapshot> compatibilityControlCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<PrivilegedExecutionStateEpoch> descriptorEpoch, VmcsField field, unsigned char destinationRegister, unsigned long long value)
		: DecisionId(decisionId), SourceOwner(std::move(sourceOwner)), SourceEpoch(sourceEpoch),
		  ExecutionDomainCapture(std::move(executionDomainCapture)), MemoryDomainCapture(std::move(memoryDomainCapture)),
		  CompletionObservationCapture(std::move(completionObservationCapture)), CompatibilityControlCapture(std::move(compatibilityControlCapture)),
		  ProfileGeneration(profileGeneration), AttemptId(attemptId), IssuerGeneration(issuerGeneration), BundleIdentity(bundleIdentity),
		  ReplayEpoch(replayEpoch), RestoreGeneration(restoreGeneration), DomainTag(domainTag), AddressSpaceTag(addressSpaceTag),
		  DescriptorEpoch(std::move(descriptorEpoch)), Field(field), DestinationRegister(destinationRegister), Value(value),
		  _owner(std::move(owner)), _attempt(std::move(attempt))
	{
	}

	std::wstring VmReadScalarResultReceipt::getDecisionId() const
	{
		return DecisionId;
	}

	std::any VmReadScalarResultReceipt::getSourceOwner() const
	{
		return SourceOwner;
	}

	unsigned long long VmReadScalarResultReceipt::getSourceEpoch() const
	{
		return SourceEpoch;
	}

	std::shared_ptr<ExecutionDomainRuntime::SourceCapture> VmReadScalarResultReceipt::getExecutionDomainCapture() const
	{
		return ExecutionDomainCapture;
	}

	std::shared_ptr<MemoryDomainRuntime::SourceCapture> VmReadScalarResultReceipt::getMemoryDomainCapture() const
	{
		return MemoryDomainCapture;
	}

	std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>> VmReadScalarResultReceipt::getCompletionObservationCapture() const
	{
		return CompletionObservationCapture;
	}

	std::shared_ptr<CompatibilityControlPolicySnapshot> VmReadScalarResultReceipt::getCompatibilityControlCapture() const
	{
		return CompatibilityControlCapture;
	}

	unsigned long long VmReadScalarResultReceipt::getProfileGeneration() const
	{
		return ProfileGeneration;
	}

	unsigned long long VmReadScalarResultReceipt::getAttemptId() const
	{
		return AttemptId;
	}

	unsigned long long VmReadScalarResultReceipt::getIssuerGeneration() const
	{
		return IssuerGeneration;
	}

	unsigned long long VmReadScalarResultReceipt::getBundleIdentity() const
	{
		return BundleIdentity;
	}

	unsigned long long VmReadScalarResultReceipt::getReplayEpoch() const
	{
		return ReplayEpoch;
	}

	unsigned long long VmReadScalarResultReceipt::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}

	unsigned long long VmReadScalarResultReceipt::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long VmReadScalarResultReceipt::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	std::shared_ptr<PrivilegedExecutionStateEpoch> VmReadScalarResultReceipt::getDescriptorEpoch() const
	{
		return DescriptorEpoch;
	}

	VmcsField VmReadScalarResultReceipt::getField() const
	{
		return Field;
	}

	unsigned char VmReadScalarResultReceipt::getDestinationRegister() const
	{
		return DestinationRegister;
	}

	unsigned long long VmReadScalarResultReceipt::getValue() const
	{
		return Value;
	}

	bool VmReadScalarResultReceipt::isConsumed() const
	{
		return _consumed.load(std::memory_order_acquire) != 0;
	}

	bool VmReadScalarResultReceipt::getBackendExecutionAuthorized() const
	{
		return false;
	}

	bool VmReadScalarResultReceipt::getCompletionPublicationAuthorized() const
	{
		return false;
	}

	bool VmReadScalarResultReceipt::getVmxRetireEffectAuthorized() const
	{
		return false;
	}

	bool VmReadScalarResultReceipt::MatchesCarrier(std::shared_ptr<VmxMicroOp> carrier, unsigned long long fieldSelector)
	{
		VmxOperationKind operation;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return fieldSelector <= ushort.MaxValue && unchecked((VmcsField)(ushort)fieldSelector) == Field && ReferenceEquals(carrier.VirtualizationAdmission, _attempt.Certificate) && _attempt.Operation == VmxOperationKind.VmRead && _attempt.AttemptId == AttemptId && _attempt.IssuerGeneration == IssuerGeneration && _attempt.BundleIdentity == BundleIdentity && _attempt.ReplayEpoch == ReplayEpoch && _attempt.Certificate.VirtualThreadId == carrier.VirtualThreadId && _attempt.Certificate.OwnerContextId == carrier.OwnerContextId && _attempt.DomainTag == carrier.Placement.DomainTag && DomainTag == carrier.Placement.DomainTag && DestinationRegister == carrier.Rd && carrier.Rs2 == 0 && carrier.TryResolveFrozenOperation(out VmxOperationKind operation) && operation == VmxOperationKind.VmRead;
		return fieldSelector <= std::numeric_limits<unsigned short>::max() && static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector)) == getField() && carrier->getVirtualizationAdmission() == _attempt->getCertificate() && _attempt->getOperation() == VmxOperationKind::VmRead && _attempt->getAttemptId() == getAttemptId() && _attempt->getIssuerGeneration() == getIssuerGeneration() && _attempt->getBundleIdentity() == getBundleIdentity() && _attempt->getReplayEpoch() == getReplayEpoch() && _attempt->getCertificate()->getVirtualThreadId() == carrier->getVirtualThreadId() && _attempt->getCertificate()->getOwnerContextId() == carrier->getOwnerContextId() && _attempt->getDomainTag() == carrier->getPlacement().DomainTag && getDomainTag() == carrier->getPlacement().DomainTag && getDestinationRegister() == carrier->getRd() && carrier->getRs2() == 0 && carrier->TryResolveFrozenOperation(operation) && operation == VmxOperationKind::VmRead;
	}

	bool VmReadScalarResultReceipt::TryValidateSpeculative(std::optional<unsigned long long> currentRestoreGeneration)
	{
		return _owner->ValidateLive(shared_from_this(), currentRestoreGeneration);
	}

	bool VmReadScalarResultReceipt::MatchesCurrentCompletionObservation(std::shared_ptr<DomainCompletionObservationOwner> observationOwner, std::optional<unsigned long long> currentRestoreGeneration)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		const auto capture = getCompletionObservationCapture();
		if (!observationOwner || !capture || !capture.value() || getSourceEpoch() != capture.value()->CompletionGeneration || getRestoreGeneration() != capture.value()->RestoreGeneration || (currentRestoreGeneration && getRestoreGeneration() != currentRestoreGeneration.value()))
		{
			return false;
		}

		const std::any sourceOwner = getSourceOwner();
		const auto capturedOwner = std::any_cast<std::shared_ptr<DomainCompletionObservationOwner>>(&sourceOwner);
		if (!capturedOwner || *capturedOwner != observationOwner)
		{
			return false;
		}

		std::shared_ptr<CompletionObservationResult> current = observationOwner->Observe(capture.value()->Scope);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return current && current->IsObserved && current->Snapshot && current->Snapshot.value() == capture.value();
	}

	bool VmReadScalarResultReceipt::TryConsumeAtRetire(unsigned long long currentRestoreGeneration)
	{
		if (!_owner->ValidateLive(shared_from_this(), currentRestoreGeneration))
		{
			return false;
		}
		int expected = 0;
		return _consumed.compare_exchange_strong(expected, 1, std::memory_order_acq_rel) &&
			_owner->ValidateConsumedBinding(shared_from_this(), currentRestoreGeneration);
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal VmReadScalarDeliveryCanonicalComposition(DomainRuntimeContext context, RootAuthorityDescriptor root, EvidencePolicyDescriptor evidence, PrivilegedExecutionStateDescriptor descriptor, PrivilegedExecutionStateEpoch currentEpoch, ulong restoreGeneration, in VmReadScalarDeliveryPolicyLookup lookup, bool conformanceProven = true)
	VmReadScalarDeliveryCanonicalComposition::VmReadScalarDeliveryCanonicalComposition(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, std::shared_ptr<PrivilegedExecutionStateEpoch> currentEpoch, unsigned long long restoreGeneration, std::shared_ptr<VmReadScalarDeliveryPolicyLookup> lookup, bool conformanceProven)
		: PolicyResolution(VmReadScalarDeliveryAcceptedPolicyResolver::Resolve(lookup)),
		  _context((context != nullptr) ? std::move(context) : throw std::invalid_argument("context")),
		  _root((root != nullptr) ? std::move(root) : throw std::invalid_argument("root")),
		  _evidence((evidence != nullptr) ? std::move(evidence) : throw std::invalid_argument("evidence")),
		  _conformanceProven(conformanceProven), _descriptor(std::move(descriptor)), _currentEpoch(std::move(currentEpoch)),
		  _restoreGeneration(restoreGeneration)
	{
	}

	std::shared_ptr<VmReadScalarDeliveryPolicyResolution> VmReadScalarDeliveryCanonicalComposition::getPolicyResolution() const
	{
		return PolicyResolution;
	}

	bool VmReadScalarDeliveryCanonicalComposition::isEnabled() const
	{
		{
		std::scoped_lock<std::mutex> lock(_sync);
			return _enabled;
		}
	}

	unsigned long long VmReadScalarDeliveryCanonicalComposition::getGeneration() const
	{
		{
		std::scoped_lock<std::mutex> lock(_sync);
			return _generation;
		}
	}

	bool VmReadScalarDeliveryCanonicalComposition::EnableExact()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			const auto resolution = getPolicyResolution();
			if (!resolution || !resolution->isResolved() || _enabled)
			{
				return false;
			}
			_enabled = true;
			return true;
		}
	}

	void VmReadScalarDeliveryCanonicalComposition::Disable()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			_enabled = false;
			AdvanceGeneration();
		}
	}

	void VmReadScalarDeliveryCanonicalComposition::ReplaceAfterRestore(std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, std::shared_ptr<PrivilegedExecutionStateEpoch> currentEpoch, unsigned long long restoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			_descriptor = descriptor;
			_currentEpoch = currentEpoch;
			_restoreGeneration = restoreGeneration;
			_enabled = false;
			AdvanceGeneration();
		}
	}

	void VmReadScalarDeliveryCanonicalComposition::ObserveReplayPhase(ReplayPhaseContext const phase)
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
// ORIGINAL LINE: internal VmReadScalarDeliveryResult Prepare(ReplayPhaseContext replayPhase, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate? e1, ulong fieldSelector, ulong restoreGeneration)
	std::shared_ptr<VmReadScalarDeliveryResult> VmReadScalarDeliveryCanonicalComposition::Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (!_enabled)
			{
				return Deny(VmReadScalarDeliveryDecision::Disabled, L"Exact scalar-delivery profile is disabled.");
			}
			const auto resolution = getPolicyResolution();
			if (!resolution || !resolution->isResolved())
			{
				return Deny(VmReadScalarDeliveryDecision::PolicyDenied, resolution ? resolution->Reason : L"Scalar-delivery policy resolution is unavailable.");
			}
			if (restoreGeneration != _restoreGeneration)
			{
				_enabled = false;
				AdvanceGeneration();
				return Deny(VmReadScalarDeliveryDecision::StaleReceipt, L"Restore generation changed before descriptor/epoch revalidation; exact profile was disabled.");
			}
			if (!_hasReplayObservation || !_replayActive || _replayEpoch != replayPhase.getEpochId() || !_currentEpoch || !carrier || !e1 || e1->getOperation() != VmxOperationKind::VmRead || e1->getAttemptId() == 0 || e1->getBundleIdentity() == 0 || e1->getReplayEpoch() != replayPhase.getEpochId() || e1->getDomainTag() != _context->getDomainTag() || carrier->getVirtualizationAdmission() != e1)
			{
				return Deny(VmReadScalarDeliveryDecision::AdmissionDenied, L"Live canonical VMREAD E1 identity is required.");
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (fieldSelector > ushort.MaxValue || unchecked((VmcsField)(ushort)fieldSelector) is not(VmcsField.GuestCr0 or VmcsField.GuestCr4))
			const VmcsField selectedField = fieldSelector <= std::numeric_limits<unsigned short>::max()
				? static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector)) : static_cast<VmcsField>(0);
			if (fieldSelector > std::numeric_limits<unsigned short>::max() ||
				(selectedField != VmcsField::GuestCr0 && selectedField != VmcsField::GuestCr4))
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"Only exact GuestCr0 or GuestCr4 is admitted.");
			}
			if (carrier->getRd() == 0 || carrier->getRd() > 31 || carrier->getRs2() != 0)
			{
				return Deny(VmReadScalarDeliveryDecision::DestinationDenied, L"VMREAD scalar delivery requires x1-x31 and reserved x0 Rs2.");
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: VmcsField field = unchecked((VmcsField)(ushort)fieldSelector);
			VmcsField field = static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector));
			std::shared_ptr<VmxCompatibilityVmReadAdmissionRequest> projectionRequest = std::make_shared<VmxCompatibilityVmReadAdmissionRequest>(
				_context, _root, _evidence, nullptr, static_cast<unsigned short>(field), carrier->getRd(), carrier->getRs1(), carrier->getRs2(),
				true, true, true, true, true, nullptr, _descriptor,
				*_currentEpoch, _conformanceProven);
			std::shared_ptr<VmxCompatibilityVmReadAdmissionResult> projection = _admission->AdmitVmReadProjection(projectionRequest);
			if (!projection->IsReadOnlyValueProjected)
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, projection->Reason);
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: VmReadScalarResultReceipt receipt = new VmReadScalarResultReceipt(this, new VmReadScalarAttemptBinding(e1), VmReadScalarDeliveryDecisionValidatorV2.ExpectedDecisionId, _descriptor, _currentEpoch.Current, executionDomainCapture: null, _generation, e1.AttemptId, e1.IssuerGeneration, e1.BundleIdentity, e1.ReplayEpoch, restoreGeneration, _context.DomainTag, _context.AddressSpaceTag, _currentEpoch, field, carrier.Rd, unchecked((ulong)projection.Value));
			std::shared_ptr<VmReadScalarResultReceipt> receipt = std::make_shared<VmReadScalarResultReceipt>(shared_from_this(), std::make_shared<VmReadScalarAttemptBinding>(e1), VmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId, _descriptor, _currentEpoch->Current, std::shared_ptr<ExecutionDomainRuntime::SourceCapture>{}, _generation, e1->getAttemptId(), e1->getIssuerGeneration(), e1->getBundleIdentity(), e1->getReplayEpoch(), restoreGeneration, _context->getDomainTag(), _context->getAddressSpaceTag(), _currentEpoch, field, carrier->getRd(), static_cast<unsigned long long>(projection->Value));
			return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryResult{VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact read-only scalar value was projected from the neutral privileged execution-state owner."});
		}
	}

	bool VmReadScalarDeliveryCanonicalComposition::ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return ValidateBinding(receipt, currentRestoreGeneration) && !receipt->isConsumed();
		}
	}

	bool VmReadScalarDeliveryCanonicalComposition::ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return ValidateBinding(receipt, currentRestoreGeneration);
		}
	}

	bool VmReadScalarDeliveryCanonicalComposition::ValidateBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		return _enabled && getPolicyResolution() && getPolicyResolution()->isResolved() && receipt && _currentEpoch && receipt->getDecisionId() == VmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId && receipt->getSourceEpoch() == _currentEpoch->Current && receipt->getProfileGeneration() == _generation && receipt->getAttemptId() != 0 && receipt->getDomainTag() == _context->getDomainTag() && receipt->getAddressSpaceTag() == _context->getAddressSpaceTag() && receipt->getDescriptorEpoch() == _currentEpoch && receipt->getRestoreGeneration() == _restoreGeneration && (!currentRestoreGeneration || receipt->getRestoreGeneration() == currentRestoreGeneration.value());
	}

	void VmReadScalarDeliveryCanonicalComposition::AdvanceGeneration()
	{
//		unchecked
			_generation++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		if (_generation == 0)
		{
			_generation = 1;
		}
	}

	std::shared_ptr<VmReadScalarDeliveryResult> VmReadScalarDeliveryCanonicalComposition::Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryResult{decision, nullptr, reason});
	}
}
