#include "MicroOpScheduler.ResearchVirtualizationCanonicalComposition.h"
#include "../../MicroOps/Types/MicroOp.IO.h"

#if defined(TESTING)
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
#endif

namespace YAKSys_Hybrid_CPU::Core
{

#if defined(TESTING)
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ResearchVirtualizationCanonicalIssueComposition(ResearchVirtualizationRuntimeOwner policyOwner, ResearchVirtualizationOperationContext context, ResearchVirtualizationRuntimeOwner? executionOwner = null, System.Nullable<System.Action<SafetyVerifier>> afterAdmission = null, System.Nullable<ResearchVirtualizationOperationContext.MaterializationLease> contextLease = null)
	ResearchVirtualizationCanonicalIssueComposition::ResearchVirtualizationCanonicalIssueComposition(std::shared_ptr<ResearchVirtualizationRuntimeOwner> policyOwner, std::shared_ptr<ResearchVirtualizationOperationContext> context, std::shared_ptr<ResearchVirtualizationRuntimeOwner> executionOwner, std::function<void(std::shared_ptr<SafetyVerifier>)> afterAdmission, std::shared_ptr<ResearchVirtualizationOperationContext::MaterializationLease> contextLease) : _executionOwner((executionOwner != nullptr) ? executionOwner : policyOwner), _policy(policyOwner->CapturePolicy()), _context(context), _contextLease((contextLease != nullptr) ? contextLease : context->CaptureMaterializationLease()), _afterAdmission(afterAdmission)
	{
		ArgumentNullException::ThrowIfNull(policyOwner);
		ArgumentNullException::ThrowIfNull(context);

	}
#endif

#if defined(TESTING)
	std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult> ResearchVirtualizationCanonicalIssueComposition::Compose(std::shared_ptr<SafetyVerifier> verifier, ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int workingSlotId, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> e1)
	{
		ArgumentNullException::ThrowIfNull(verifier);
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(e1);

		{
			std::scoped_lock<std::mutex> lock(_gate);
			if (_observedAttemptId != 0)
			{
				return Deny(ResearchVirtualizationCanonicalCompositionDecision::DeniedDuplicateMaterialization, L"The TESTING-only canonical composition already observed one E1 attempt.");
			}

			_observedAttemptId = e1->getAttemptId();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: ResearchVirtualizationOperationContext.IdentitySnapshot? identity = _context.Materialize(_contextLease, e1.AttemptId, e1.ReplayEpoch);
			std::shared_ptr<ResearchVirtualizationOperationContext::IdentitySnapshot> identity = _context->Materialize(_contextLease, e1->getAttemptId(), e1->getReplayEpoch());
			if (identity == nullptr)
			{
				return Deny(ResearchVirtualizationCanonicalCompositionDecision::DeniedStaleRuntimeContextLease, L"The typed runtime context lease was foreign or stale before canonical materialization.");
			}

			std::shared_ptr<ResearchVirtualizationProbeAdmissionResult> admission = verifier->IssueResearchVirtualizationOperationAdmission(_policy, _context, identity, replayPhase, bundleMetadata, carrier, sourceSlotId, workingSlotId, e1);
			if (!admission->IsIssued || admission->Certificate == nullptr)
			{
				return new(ResearchVirtualizationCanonicalCompositionDecision::DeniedPrototypeAdmission, admission->Decision, nullptr, nullptr, admission->Reason);
			}

			if (_afterAdmission != nullptr)
			{
				_afterAdmission->Invoke(verifier);
			}
			std::shared_ptr<ResearchVirtualizationProbeExecutionResult> execution = _executionOwner->Execute(verifier, admission->Certificate, _context);
			if (!execution->Succeeded || execution->Receipt == nullptr)
			{
				return new(ResearchVirtualizationCanonicalCompositionDecision::DeniedPrototypeExecution, admission->Decision, execution->Decision, nullptr, execution->Reason);
			}

			return new(ResearchVirtualizationCanonicalCompositionDecision::MaterializedReceipt, admission->Decision, execution->Decision, execution->Receipt, L"The canonical TESTING-only issue boundary materialized one no-state/no-payload research receipt.");
		}
	}
#endif

#if defined(TESTING)
	std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult> ResearchVirtualizationCanonicalIssueComposition::Deny(ResearchVirtualizationCanonicalCompositionDecision decision, const std::wstring &reason)
	{
		return new(decision, nullptr, nullptr, nullptr, reason);
	}
#endif
}
