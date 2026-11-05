#pragma once




#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../Domains/Authority/YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"
#include "../../Capabilities/Grants/RuntimeCapabilityGrantLease.h"
#include "VirtualizationOperandSnapshot.h"
#include "DomainHypercallRuntimeExecutor.h"
#include "../../Completion/Records/DomainHypercallCompletionOwner.h"
#include "../../Migration/Checkpoint/DomainHypercallLifecycleGate.h"
#include "../../Completion/Records/DomainHypercallRetireOwner.h"
#include "../../../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../../../Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include <string>
#include <optional>
#include <mutex>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxMicroOp;
	class SafetyVerifier;

// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//	using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

	enum class DomainHypercallCompositionDecision : unsigned char
	{
		Prepared = 0,
		Disabled = 1,
		MissingCanonicalVerifier = 2,
		InvalidRuntimeBinding = 3,
		E2Denied = 4,
		DuplicateCarrier = 5,
		Draining = 6
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct DomainHypercallCompositionResult(DomainHypercallCompositionDecision Decision, SafetyVerifier.VirtualizationOperationAdmissionCertificate? E2, string Reason)
//	internal readonly record struct DomainHypercallCompositionResult(DomainHypercallCompositionDecision Decision, SafetyVerifier.VirtualizationOperationAdmissionCertificate E2, string Reason)
//	{
//		internal bool IsPrepared
//		{
//			get
//			{
//				return Decision == DomainHypercallCompositionDecision.Prepared && E2 != nullptr;
//			}
//		}
//		internal bool BackendExecutionAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool CompletionPublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RetirePublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

	/// <summary>
	/// Neutral owner binding for the exact Phase-38 operation. The binding is
	/// production-compiled but not globally registered: only the canonical scheduler
	/// seam may consume it. Disabling is irreversible and revokes its grant generation.
	/// </summary>
	class SmtBundleMetadata4Way;
struct DomainHypercallCompositionResult final {
    const DomainHypercallCompositionDecision Decision;
    const std::shared_ptr<VirtualizationOperationAdmissionCertificate> E2;
    const std::wstring Reason;
    DomainHypercallCompositionResult(DomainHypercallCompositionDecision decision, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2, std::wstring reason) : Decision(decision), E2(std::move(e2)), Reason(std::move(reason)) {}
    bool isPrepared() const noexcept { return Decision == DomainHypercallCompositionDecision::Prepared && E2 != nullptr; }
    bool backendExecutionAuthorized() const noexcept { return false; }
    bool completionPublicationAuthorized() const noexcept { return false; }
    bool retirePublicationAuthorized() const noexcept { return false; }
};
class DomainHypercallCanonicalComposition final : public std::enable_shared_from_this<DomainHypercallCanonicalComposition>
	{
	public:
		class ExecutionDispatch final : public std::enable_shared_from_this<ExecutionDispatch>
		{
		private:
			const std::shared_ptr<DomainHypercallCanonicalComposition> _owner;
			const std::shared_ptr<VmxMicroOp> _carrier;
			const std::shared_ptr<SafetyVerifier> _verifier;
			const std::shared_ptr<VirtualizationOperationAdmissionCertificate> _e2;

			ExecutionDispatch(std::shared_ptr<DomainHypercallCanonicalComposition> owner, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2);

		public:
			std::shared_ptr<DomainHypercallExecutionResult> Execute(std::shared_ptr<VmxMicroOp> carrier, std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> &publication);

			static std::shared_ptr<ExecutionDispatch> Create(std::shared_ptr<DomainHypercallCanonicalComposition> owner, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2);
		};

	private:
		mutable std::mutex _sync;
		const std::shared_ptr<DomainRuntimeContext> _context;
		const std::shared_ptr<RootAuthorityDescriptor> _root;
		const std::shared_ptr<RuntimeCapabilityGrantOwner> _capabilityOwner;
		const std::shared_ptr<RuntimeCapabilityGrantLease> _capabilityLease;
		const std::shared_ptr<VirtualizationRestoreGenerationOwner> _restoreOwner;
		const std::shared_ptr<DomainHypercallRuntimeExecutor> _executor;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly DomainHypercallCompletionOwner? _completionOwner;
		const std::shared_ptr<DomainHypercallCompletionOwner> _completionOwner;
		const std::shared_ptr<DomainHypercallLifecycleGate> _lifecycleGate;
		bool _enabled = false;
		bool _draining = false;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallCanonicalComposition(DomainRuntimeContext context, RootAuthorityDescriptor root, RuntimeCapabilityGrantOwner capabilityOwner, RuntimeCapabilityGrantLease capabilityLease, VirtualizationRestoreGenerationOwner restoreOwner, DomainHypercallRuntimeExecutor executor, DomainHypercallCompletionOwner? completionOwner = null, System.Nullable<DomainHypercallRetireOwner> retireOwner = null, System.Nullable<DomainHypercallLifecycleGate> lifecycleGate = null)
		DomainHypercallCanonicalComposition(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<RuntimeCapabilityGrantOwner> capabilityOwner, std::shared_ptr<RuntimeCapabilityGrantLease> capabilityLease, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallRuntimeExecutor> executor, std::shared_ptr<DomainHypercallCompletionOwner> completionOwner = nullptr, std::shared_ptr<DomainHypercallRetireOwner> retireOwner = nullptr, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate = nullptr);

		bool isEnabled() const;

		std::shared_ptr<DomainHypercallCompositionResult> Prepare(std::shared_ptr<SafetyVerifier> verifier, ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int workingSlotId, std::shared_ptr<VirtualizationAdmissionCertificate> e1, std::shared_ptr<VirtualizationOperandSnapshot> operand);

		void Disable();

		void BeginDrain();

		bool ResumeAfterValidatedRestore(const std::wstring &specDigest);

		bool isDraining() const;

		std::shared_ptr<DomainHypercallLifecycleGate> getLifecycleGate() const;

	private:
		std::shared_ptr<DomainHypercallExecutionResult> Execute(std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2, std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> &publication);

		static std::shared_ptr<DomainHypercallCompositionResult> Deny(DomainHypercallCompositionDecision decision, const std::wstring &reason);
	};

}
