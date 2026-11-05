#pragma once

#include "../../../Runtime/Events/Hypercalls/ResearchVirtualizationRuntimeProbe.h"
#include "../../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../Certificates/ReplayPhaseSubstrate.h"
#include "../../Certificates/ReplayPhaseSubstrate.Implementations.h"
namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }
#include <string>
#include <functional>
#include <mutex>
#include <memory>
#include <optional>

#if defined(TESTING)
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class ResearchVirtualizationCanonicalCompositionDecision : unsigned char
	{
		MaterializedReceipt = 0,
		DeniedNoCanonicalVerifier = 1,
		DeniedStaleRuntimeContextLease = 2,
		DeniedDuplicateMaterialization = 3,
		DeniedPrototypeAdmission = 4,
		DeniedPrototypeExecution = 5
	};

	struct ResearchVirtualizationCanonicalCompositionResult final
	{
		const ResearchVirtualizationCanonicalCompositionDecision Decision;
		const std::optional<ResearchVirtualizationProbeAdmissionDecision> AdmissionDecision;
		const std::optional<ResearchVirtualizationProbeExecutionDecision> ExecutionDecision;
		const std::optional<ResearchVirtualizationRuntimeOwner::ExecutionReceipt> Receipt;
		const std::wstring Reason;
		ResearchVirtualizationCanonicalCompositionResult(ResearchVirtualizationCanonicalCompositionDecision decision, std::optional<ResearchVirtualizationProbeAdmissionDecision> admissionDecision, std::optional<ResearchVirtualizationProbeExecutionDecision> executionDecision, std::optional<ResearchVirtualizationRuntimeOwner::ExecutionReceipt> receipt, std::wstring reason)
			: Decision(decision), AdmissionDecision(admissionDecision), ExecutionDecision(executionDecision), Receipt(std::move(receipt)), Reason(std::move(reason)) {}
		bool getSucceeded() const { return Decision == ResearchVirtualizationCanonicalCompositionDecision::MaterializedReceipt && Receipt.has_value(); }
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct ResearchVirtualizationCanonicalCompositionResult(ResearchVirtualizationCanonicalCompositionDecision Decision, System.Nullable<ResearchVirtualizationProbeAdmissionDecision> AdmissionDecision, System.Nullable<ResearchVirtualizationProbeExecutionDecision> ExecutionDecision, ResearchVirtualizationRuntimeOwner.ExecutionReceipt? Receipt, string Reason)
//	internal readonly record struct ResearchVirtualizationCanonicalCompositionResult(ResearchVirtualizationCanonicalCompositionDecision Decision, System.Nullable<ResearchVirtualizationProbeAdmissionDecision> AdmissionDecision, System.Nullable<ResearchVirtualizationProbeExecutionDecision> ExecutionDecision, ResearchVirtualizationRuntimeOwner.ExecutionReceipt Receipt, string Reason)
//	{
//		internal bool Succeeded
//		{
//			get
//			{
//				return Decision == ResearchVirtualizationCanonicalCompositionDecision.MaterializedReceipt && Receipt != nullptr;
//			}
//		}
//	}

	/// <summary>
	/// Default-off TESTING-only composition of the Phase 36 probe at the canonical
	/// issue/materialization seam. It owns no production dispatch, completion or retire
	/// authority; it can only consume a live E1 supplied by that seam.
	/// </summary>
	class ResearchVirtualizationCanonicalIssueComposition final : public std::enable_shared_from_this<ResearchVirtualizationCanonicalIssueComposition>
	{
	private:
		std::mutex _gate;
		const std::shared_ptr<ResearchVirtualizationRuntimeOwner> _executionOwner;
		const std::shared_ptr<ResearchVirtualizationRuntimeOwner::PolicySnapshot> _policy;
		const std::shared_ptr<ResearchVirtualizationOperationContext> _context;
		const std::shared_ptr<ResearchVirtualizationOperationContext::MaterializationLease> _contextLease;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Action<SafetyVerifier>? _afterAdmission;
		const std::function<void(std::shared_ptr<SafetyVerifier>)> _afterAdmission;
		unsigned long long _observedAttemptId = 0;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ResearchVirtualizationCanonicalIssueComposition(ResearchVirtualizationRuntimeOwner policyOwner, ResearchVirtualizationOperationContext context, ResearchVirtualizationRuntimeOwner? executionOwner = null, System.Nullable<System.Action<SafetyVerifier>> afterAdmission = null, System.Nullable<ResearchVirtualizationOperationContext.MaterializationLease> contextLease = null)
		ResearchVirtualizationCanonicalIssueComposition(std::shared_ptr<ResearchVirtualizationRuntimeOwner> policyOwner, std::shared_ptr<ResearchVirtualizationOperationContext> context, std::shared_ptr<ResearchVirtualizationRuntimeOwner> executionOwner = nullptr, std::function<void(std::shared_ptr<SafetyVerifier>)> afterAdmission = nullptr, std::shared_ptr<ResearchVirtualizationOperationContext::MaterializationLease> contextLease = nullptr);

		std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult> Compose(std::shared_ptr<SafetyVerifier> verifier, ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int workingSlotId, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> e1);

	private:
		static std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult> Deny(ResearchVirtualizationCanonicalCompositionDecision decision, const std::wstring &reason);
	};

}
#endif
