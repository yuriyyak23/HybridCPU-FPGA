#pragma once

#include "../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include "../../../Pipeline/Safety/VirtualizationOperationAdmissionCertificate.h"
#include "VirtualizationOperandSnapshot.h"
#include "../../Migration/Checkpoint/DomainHypercallLifecycleGate.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <any>
#include <mutex>
#include <map>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
    class SafetyVerifier;
    struct DomainHypercallExecutionResult;
    struct DomainHypercallReceiptValidationResult;
	struct DomainHypercallExecutorSeal final {};
	using DomainHypercallExecutorSealToken = std::shared_ptr<const DomainHypercallExecutorSeal>;

	enum class ExactProbeExecutionMode : unsigned char
	{
		Disabled = 0,
		ExactProbeOnly = 1
	};

	enum class DomainHypercallExecutionDecision : unsigned char
	{
		Executed = 0,
		Disabled = 1,
		MissingAdmission = 2,
		InvalidAdmission = 3,
		OperationBindingMismatch = 4,
		LifecycleGateDenied = 5
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct DomainHypercallExecutionResult(DomainHypercallExecutionDecision Decision, DomainHypercallRuntimeExecutor.ExecutionReceipt? Receipt, string Reason)
//	internal readonly record struct DomainHypercallExecutionResult(DomainHypercallExecutionDecision Decision, DomainHypercallRuntimeExecutor.ExecutionReceipt Receipt, string Reason)
//	{
//		internal bool IsExecuted
//		{
//			get
//			{
//				return Decision == DomainHypercallExecutionDecision.Executed && Receipt != nullptr;
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

	enum class DomainHypercallReceiptValidationDecision : unsigned char
	{
		Valid = 0,
		Missing = 1,
		ForeignIssuer = 2,
		NotLive = 3,
		RestoreGenerationMismatch = 4,
		DigestMismatch = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct DomainHypercallReceiptValidationResult(DomainHypercallReceiptValidationDecision Decision, string Reason)
//	{
//		internal bool IsValid
//		{
//			get
//			{
//				return Decision == DomainHypercallReceiptValidationDecision.Valid;
//			}
//		}
//	}

	/// <summary>
	/// Exact neutral owner executor for PROBE_NO_STATE_V1. It is not connected to
	/// decode, VMX compatibility, completion or retire in PR-E and defaults disabled.
	/// </summary>
	struct DomainHypercallExecutionResult;
    struct DomainHypercallReceiptValidationResult;
    class DomainHypercallCompletionOwner;
    class DomainHypercallRuntimeExecutor final : public std::enable_shared_from_this<DomainHypercallRuntimeExecutor>
	{
	public:
		class ExecutionReceipt final : public std::enable_shared_from_this<ExecutionReceipt>
		{
		private:
			const unsigned long long ExecutionSequence;
			const std::wstring E2Digest;
			const unsigned long long AttemptId;
			const int VirtualThreadId;
			const unsigned long long DomainTag;
			const std::wstring DecisionId;
			const unsigned long long OwnerId;
			const unsigned int OwnerPolicyVersion;
			const unsigned int OwnerEpoch;
			const std::wstring OperationNamespace;
			const std::wstring OperationId;
			const unsigned short NumericLeaf;
			const unsigned long long RestoreGeneration;
			const VirtualizationDecisionEffectClassV2 EffectClass;
			const VirtualizationDecisionResultAbiV2 ResultAbi;
			const std::wstring EffectDigest;
			const std::wstring ResultDigest;
			const std::wstring ReceiptDigest;

			const std::any _issuerSeal;

			ExecutionReceipt(std::any issuerSeal, unsigned long long executionSequence, const std::wstring &e2Digest, unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring &operationNamespace, const std::wstring &operationId, unsigned short numericLeaf, unsigned long long restoreGeneration, VirtualizationDecisionEffectClassV2 effectClass, VirtualizationDecisionResultAbiV2 resultAbi, const std::wstring &effectDigest, const std::wstring &resultDigest, const std::wstring &receiptDigest);

		public:
			unsigned int getSchemaVersion() const;
			unsigned long long getExecutionSequence() const;
			std::wstring getE2Digest() const;
			unsigned long long getAttemptId() const;
			int getVirtualThreadId() const;
			unsigned long long getDomainTag() const;
			std::wstring getDecisionId() const;
			unsigned long long getOwnerId() const;
			unsigned int getOwnerPolicyVersion() const;
			unsigned int getOwnerEpoch() const;
			std::wstring getOperationNamespace() const;
			std::wstring getOperationId() const;
			unsigned short getNumericLeaf() const;
			unsigned long long getRestoreGeneration() const;
			VirtualizationDecisionEffectClassV2 getEffectClass() const;
			VirtualizationDecisionResultAbiV2 getResultAbi() const;
			std::wstring getEffectDigest() const;
			std::wstring getResultDigest() const;
			std::wstring getReceiptDigest() const;
			bool getHasPayload() const;
			bool getHasStateEffect() const;
			bool getCompletionPublicationAuthorized() const;
			bool getRetirePublicationAuthorized() const;

			bool WasIssuedBy(std::any issuerSeal);

			static std::shared_ptr<ExecutionReceipt> Create(std::any issuerSeal, unsigned long long executionSequence, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2);
		};

	private:
		class LiveReceipt final : public std::enable_shared_from_this<LiveReceipt>
		{
		private:
			const std::shared_ptr<VirtualizationOperationAdmissionCertificate> E2;
			const std::shared_ptr<VirtualizationRestoreGenerationOwner> RestoreOwner;
			const std::shared_ptr<DomainHypercallLifecycleGate> LifecycleGate;
			bool ConsumedByCompletionOwner = false;

		public:
			LiveReceipt(std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate);

			std::shared_ptr<VirtualizationOperationAdmissionCertificate> getE2() const;
			std::shared_ptr<VirtualizationRestoreGenerationOwner> getRestoreOwner() const;
			std::shared_ptr<DomainHypercallLifecycleGate> getLifecycleGate() const;
			bool getConsumedByCompletionOwner() const;
			void setConsumedByCompletionOwner(bool value);
		};

	private:
		static inline const DomainHypercallExecutorSealToken ExactConsumerSeal = std::make_shared<DomainHypercallExecutorSeal>();
		const DomainHypercallExecutorSealToken _issuerSeal = std::make_shared<DomainHypercallExecutorSeal>();
		std::map<std::weak_ptr<ExecutionReceipt>, std::shared_ptr<LiveReceipt>, std::owner_less<std::weak_ptr<ExecutionReceipt>>> _liveReceipts;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ExecutionReceipt> _unconsumedReceipts = new();
		std::unordered_set<std::shared_ptr<ExecutionReceipt>> _unconsumedReceipts = std::unordered_set<std::shared_ptr<ExecutionReceipt>>();
		const ExactProbeExecutionMode _mode;
		std::mutex _executionSync;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private Object? _completionConsumerSeal;
		std::any _completionConsumerSeal;
		unsigned long long _nextExecutionSequence = 1;

	public:
		DomainHypercallRuntimeExecutor(ExactProbeExecutionMode mode = ExactProbeExecutionMode::Disabled);

		ExactProbeExecutionMode getMode() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallExecutionResult ExecuteExactProbe(SafetyVerifier verifier, SafetyVerifier.VirtualizationOperationAdmissionCertificate? e2, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, System.Nullable<DomainHypercallLifecycleGate> lifecycleGate)
		std::shared_ptr<DomainHypercallExecutionResult> ExecuteExactProbe(std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallReceiptValidationResult ValidateReceipt(ExecutionReceipt? receipt, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner)
		std::shared_ptr<DomainHypercallReceiptValidationResult> ValidateReceipt(std::shared_ptr<ExecutionReceipt> receipt, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallReceiptValidationResult ConsumeReceiptForCompletion(ExecutionReceipt? receipt, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, DomainHypercallLifecycleGate lifecycleGate, Object consumerSeal)
		std::shared_ptr<DomainHypercallReceiptValidationResult> ConsumeReceiptForCompletion(std::shared_ptr<ExecutionReceipt> receipt, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate, std::any consumerSeal);

		void BindCompletionOwner(std::shared_ptr<DomainHypercallCompletionOwner> owner);

		int CountLiveReceipts(unsigned long long domainTag);

		int CancelLiveReceiptsForDrain(unsigned long long domainTag);

		static bool IsExactConsumerSeal(std::any consumerSeal);

	private:
		static bool IsExactBinding(std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2);

		unsigned long long AllocateExecutionSequence();

		static std::shared_ptr<DomainHypercallExecutionResult> Deny(DomainHypercallExecutionDecision decision, const std::wstring &reason);

		static std::shared_ptr<DomainHypercallReceiptValidationResult> Invalid(DomainHypercallReceiptValidationDecision decision, const std::wstring &reason);
	};

	class DomainHypercallExecutionReceiptDigest final
	{
	private:
		static const std::wstring NoEffectDigest;
		static const std::wstring NoResultDigest;

		static inline const std::vector<unsigned char> Envelope = {'H', 'C', 'P', 'U', 'E', '3', 'V', '2', 0};
	public:
		static std::wstring getNoEffectDigest();
		static std::wstring getNoResultDigest();

		static std::wstring Compute(unsigned long long sequence, const std::wstring &e2Digest, unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring &operationNamespace, const std::wstring &operationId, unsigned short numericLeaf, unsigned long long restoreGeneration, VirtualizationDecisionEffectClassV2 effectClass, VirtualizationDecisionResultAbiV2 resultAbi, const std::wstring &effectDigest, const std::wstring &resultDigest);

	private:
		static std::wstring HashConstant(const std::wstring &value);

		static void WriteText(std::vector<unsigned char> &stream, const std::wstring &value);

		static void WriteU16(std::vector<unsigned char> &stream, unsigned short value);

		static void WriteU32(std::vector<unsigned char> &stream, unsigned int value);

		static void WriteU64(std::vector<unsigned char> &stream, unsigned long long value);
	};

}

namespace YAKSys_Hybrid_CPU::Core {
struct DomainHypercallExecutionResult final {
    const DomainHypercallExecutionDecision Decision;
    const std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> Receipt;
    const std::wstring Reason;
    DomainHypercallExecutionResult(DomainHypercallExecutionDecision decision, std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> receipt, std::wstring reason)
        : Decision(decision), Receipt(std::move(receipt)), Reason(std::move(reason)) {}
    bool isExecuted() const noexcept { return Decision == DomainHypercallExecutionDecision::Executed && Receipt != nullptr; }
    bool completionPublicationAuthorized() const noexcept { return false; }
    bool retirePublicationAuthorized() const noexcept { return false; }
};
struct DomainHypercallReceiptValidationResult final {
    const DomainHypercallReceiptValidationDecision Decision;
    const std::wstring Reason;
    DomainHypercallReceiptValidationResult(DomainHypercallReceiptValidationDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)) {}
    bool isValid() const noexcept { return Decision == DomainHypercallReceiptValidationDecision::Valid; }
};
}
