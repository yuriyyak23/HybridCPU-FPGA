#include "DomainHypercallCanonicalComposition.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "Governance/VirtualizationOperationOwnerSnapshot.h"
#include "Governance/Phase38VirtualizationDecisionSpecV2.h"
#include <codecvt>
#include <locale>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	DomainHypercallCanonicalComposition::ExecutionDispatch::ExecutionDispatch(
		std::shared_ptr<DomainHypercallCanonicalComposition> owner,
		std::shared_ptr<VmxMicroOp> carrier,
		std::shared_ptr<SafetyVerifier> verifier,
		std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2)
		: _owner(std::move(owner)), _carrier(std::move(carrier)), _verifier(std::move(verifier)), _e2(std::move(e2))
	{
	}

	std::shared_ptr<DomainHypercallExecutionResult> DomainHypercallCanonicalComposition::ExecutionDispatch::Execute(
		std::shared_ptr<VmxMicroOp> carrier,
		std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> &publication)
	{
		publication = std::nullopt;
		if (carrier != _carrier)
			return std::make_shared<DomainHypercallExecutionResult>(
				DomainHypercallExecutionDecision::InvalidAdmission, nullptr,
				L"Exact hypercall dispatch cannot be transplanted to another carrier.");
		return _owner->Execute(_verifier, _e2, publication);
	}

	std::shared_ptr<DomainHypercallCanonicalComposition::ExecutionDispatch>
	DomainHypercallCanonicalComposition::ExecutionDispatch::Create(
		std::shared_ptr<DomainHypercallCanonicalComposition> owner,
		std::shared_ptr<VmxMicroOp> carrier,
		std::shared_ptr<SafetyVerifier> verifier,
		std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2)
	{
		return std::shared_ptr<ExecutionDispatch>(new ExecutionDispatch(
			std::move(owner), std::move(carrier), std::move(verifier), std::move(e2)));
	}

	DomainHypercallCanonicalComposition::DomainHypercallCanonicalComposition(
		std::shared_ptr<DomainRuntimeContext> context,
		std::shared_ptr<RootAuthorityDescriptor> root,
		std::shared_ptr<RuntimeCapabilityGrantOwner> capabilityOwner,
		std::shared_ptr<RuntimeCapabilityGrantLease> capabilityLease,
		std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner,
		std::shared_ptr<DomainHypercallRuntimeExecutor> executor,
		std::shared_ptr<DomainHypercallCompletionOwner> completionOwner,
		std::shared_ptr<DomainHypercallRetireOwner> retireOwner,
		std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate)
		: _context(context), _root(root), _capabilityOwner(capabilityOwner), _capabilityLease(capabilityLease),
		  _restoreOwner(restoreOwner), _executor(executor), _completionOwner(completionOwner), _lifecycleGate(lifecycleGate)
	{
		ArgumentNullException::ThrowIfNull(context);
		ArgumentNullException::ThrowIfNull(root);
		ArgumentNullException::ThrowIfNull(capabilityOwner);
		ArgumentNullException::ThrowIfNull(capabilityLease);
		ArgumentNullException::ThrowIfNull(restoreOwner);
		ArgumentNullException::ThrowIfNull(executor);
		ArgumentNullException::ThrowIfNull(lifecycleGate);
		if (lifecycleGate->getDomainTag() != context->getDomainTag() || !lifecycleGate->Observe()->AcceptsNewE2)
			throw InvalidOperationException(L"Exact composition requires the active matching per-domain lifecycle gate.");

		if (completionOwner)
		{
			_executor->BindCompletionOwner(completionOwner);
			if (retireOwner)
				completionOwner->BindRetireOwner(retireOwner);
		}
		_enabled = true;
	}

	bool DomainHypercallCanonicalComposition::isEnabled() const
	{
		std::scoped_lock<std::mutex> lock(_sync);
		return _enabled;
	}

	std::shared_ptr<DomainHypercallCompositionResult> DomainHypercallCanonicalComposition::Prepare(
		std::shared_ptr<SafetyVerifier> verifier,
		ReplayPhaseContext replayPhase,
		SmtBundleMetadata4Way bundleMetadata,
		std::shared_ptr<VmxMicroOp> carrier,
		int sourceSlotId,
		int workingSlotId,
		std::shared_ptr<VirtualizationAdmissionCertificate> e1,
		std::shared_ptr<VirtualizationOperandSnapshot> operand)
	{
		ArgumentNullException::ThrowIfNull(verifier);
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(e1);
		ArgumentNullException::ThrowIfNull(operand);

		std::scoped_lock<std::mutex> lock(_sync);
		if (!_enabled)
			return Deny(DomainHypercallCompositionDecision::Disabled, L"Exact hypercall composition is disabled.");
		if (_draining)
			return Deny(DomainHypercallCompositionDecision::Draining, L"DrainOnly gate has closed new exact E2 admission.");
		if (_executor->getMode() != ExactProbeExecutionMode::ExactProbeOnly ||
			!_capabilityOwner->IsLive(_capabilityLease) || _restoreOwner->getCurrentGeneration() == 0)
			return Deny(DomainHypercallCompositionDecision::InvalidRuntimeBinding,
				L"Exact hypercall composition requires a live exact owner binding and enabled executor.");

		auto request = std::make_shared<VirtualizationE2IssueRequest>(VirtualizationE2IssueRequest{
			replayPhase, bundleMetadata, carrier, sourceSlotId, workingSlotId, e1,
			Phase38VirtualizationOperationOwnerSnapshotRegistry::getExactSnapshot(), operand,
			_context, _root, _capabilityOwner, _capabilityLease, _restoreOwner, _lifecycleGate});
		auto issued = verifier->IssueVirtualizationE2(request);
		if (!issued || !issued->IsIssued() || !issued->Certificate)
			return Deny(DomainHypercallCompositionDecision::E2Denied,
				issued ? issued->Reason : L"Virtualization E2 issuance returned no result.");

		auto dispatch = ExecutionDispatch::Create(shared_from_this(), carrier, verifier, issued->Certificate);
		try
		{
			carrier->AttachExactHypercallExecutionDispatch(dispatch);
		}
		catch (const InvalidOperationException &exception)
		{
			verifier->RevokeVirtualizationE2(issued->Certificate);
			const std::wstring reason = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(exception.what());
			return Deny(DomainHypercallCompositionDecision::DuplicateCarrier, reason);
		}

		return std::make_shared<DomainHypercallCompositionResult>(
			DomainHypercallCompositionDecision::Prepared, issued->Certificate,
			L"Canonical E1/operand seam prepared exact E2 for execute-stage consumption.");
	}

	void DomainHypercallCanonicalComposition::Disable()
	{
		_lifecycleGate->BeginDrain();
		std::scoped_lock<std::mutex> lock(_sync);
		if (!_enabled) return;
		_enabled = false;
		_capabilityOwner->RevokeAll();
	}

	void DomainHypercallCanonicalComposition::BeginDrain()
	{
		_lifecycleGate->BeginDrain();
		std::scoped_lock<std::mutex> lock(_sync);
		if (_enabled) _draining = true;
	}

	bool DomainHypercallCanonicalComposition::ResumeAfterValidatedRestore(const std::wstring &specDigest)
	{
		std::scoped_lock<std::mutex> lock(_sync);
		if (!_enabled || specDigest != Phase38VirtualizationDecisionSpecV2::getInstance()->SpecDigest)
			return false;
		if (!_lifecycleGate->ResumeAfterValidatedRestore(specDigest))
			return false;
		_draining = false;
		return true;
	}

	bool DomainHypercallCanonicalComposition::isDraining() const
	{
		std::scoped_lock<std::mutex> lock(_sync);
		return _draining;
	}

	std::shared_ptr<DomainHypercallLifecycleGate> DomainHypercallCanonicalComposition::getLifecycleGate() const
	{
		return _lifecycleGate;
	}

	std::shared_ptr<DomainHypercallExecutionResult> DomainHypercallCanonicalComposition::Execute(
		std::shared_ptr<SafetyVerifier> verifier,
		std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2,
		std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> &publication)
	{
		publication = std::nullopt;
		std::scoped_lock<std::mutex> lock(_sync);
		if (!_enabled)
		{
			verifier->RevokeVirtualizationE2(e2);
			return std::make_shared<DomainHypercallExecutionResult>(
				DomainHypercallExecutionDecision::Disabled, nullptr,
				L"Exact hypercall composition was disabled before execute.");
		}

		auto execution = _executor->ExecuteExactProbe(verifier, e2, _restoreOwner, _lifecycleGate);
		if (execution && execution->isExecuted() && execution->Receipt && _completionOwner)
			publication = _completionOwner->PublishExactProbe(_executor, execution->Receipt, _restoreOwner, _context, _root, _lifecycleGate);
		return execution;
	}

	std::shared_ptr<DomainHypercallCompositionResult> DomainHypercallCanonicalComposition::Deny(
		DomainHypercallCompositionDecision decision, const std::wstring &reason)
	{
		return std::make_shared<DomainHypercallCompositionResult>(decision, nullptr, reason);
	}
}
