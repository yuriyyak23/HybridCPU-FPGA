#pragma once

#include "../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "YAKSys_Hybrid_CPU.Core.CompletionRecord.h"
#include <string>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class TrapCompletionPublicationDecision : unsigned char
	{
		DeniedRuntimeAdmission = 0,
		DeniedNoNeutralTrap = 1,
		DeniedBackendExecution = 2,
		DeniedRetirePublication = 3,
		Allowed = 4,
		CompletionPublishedRetireDenied = 5
	};

	enum class TrapCompletionMigrationClass : unsigned char
	{
		Unclassified = 0,
		RecomputedAfterRestore = 1,
		GuestArchitecturalState = 2,
		HostOwnedNonMigratable = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct TrapCompletionPublicationFenceResult(TrapCompletionPublicationDecision Decision, NeutralTrapResult TrapResult, CompletionRecord Completion, bool RetirePublicationAuthorized, string Reason)
//	{
//		public bool CompletionPublicationAllowed
//		{
//			get
//			{
//				return (Decision is TrapCompletionPublicationDecision.Allowed or TrapCompletionPublicationDecision.CompletionPublishedRetireDenied) && !Completion.IsEmpty;
//			}
//		}
//
//		public bool RetirePublicationAllowed
//		{
//			get
//			{
//				return Decision == TrapCompletionPublicationDecision.Allowed && RetirePublicationAuthorized;
//			}
//		}
//
//		public bool IsDenied
//		{
//			get
//			{
//				return !CompletionPublicationAllowed;
//			}
//		}
//
//		public bool IsCompletionOnly
//		{
//			get
//			{
//				return Decision == TrapCompletionPublicationDecision.CompletionPublishedRetireDenied && CompletionPublicationAllowed && !RetirePublicationAllowed;
//			}
//		}
//
//		public bool IsRetireDenied
//		{
//			get
//			{
//				return CompletionPublicationAllowed && !RetirePublicationAllowed;
//			}
//		}
//
//		public bool DeniesBackendExecution
//		{
//			get
//			{
//				return Decision == TrapCompletionPublicationDecision.DeniedBackendExecution;
//			}
//		}
//	}

	class NeutralTrapResult;
class TrapCompletionPublicationFenceResult final {
public:
    const TrapCompletionPublicationDecision Decision;
    const std::shared_ptr<NeutralTrapResult> TrapResult;
    const std::shared_ptr<CompletionRecord> Completion;
    const bool RetirePublicationAuthorized;
    const std::wstring Reason;
    const bool CompletionPublicationAllowed, RetirePublicationAllowed, IsDenied, IsCompletionOnly, IsRetireDenied, DeniesBackendExecution;
    TrapCompletionPublicationFenceResult(TrapCompletionPublicationDecision decision, std::shared_ptr<NeutralTrapResult> trap, std::shared_ptr<CompletionRecord> completion, bool retire, std::wstring reason)
        : Decision(decision), TrapResult(std::move(trap)), Completion(std::move(completion)), RetirePublicationAuthorized(retire), Reason(std::move(reason)),
          CompletionPublicationAllowed((decision == TrapCompletionPublicationDecision::Allowed || decision == TrapCompletionPublicationDecision::CompletionPublishedRetireDenied) && Completion && !Completion->isEmpty()),
          RetirePublicationAllowed(decision == TrapCompletionPublicationDecision::Allowed && retire), IsDenied(!CompletionPublicationAllowed),
          IsCompletionOnly(decision == TrapCompletionPublicationDecision::CompletionPublishedRetireDenied && CompletionPublicationAllowed && !RetirePublicationAllowed),
          IsRetireDenied(CompletionPublicationAllowed && !RetirePublicationAllowed), DeniesBackendExecution(decision == TrapCompletionPublicationDecision::DeniedBackendExecution) {}
};
    class NeutralTrapResult;
    struct TrapCompletionRouteRequest;
    struct TrapCompletionRouteResult;
    struct TrapCompletionOwnerPolicyResult final {
        const bool IsAllowed;
        const std::wstring Reason;
        TrapCompletionOwnerPolicyResult(bool isAllowed, std::wstring reason)
            : IsAllowed(isAllowed), Reason(std::move(reason)) {}
    };
class TrapCompletionPublicationFence final : public std::enable_shared_from_this<TrapCompletionPublicationFence>
	{
	private:
		static const std::shared_ptr<TrapCompletionPublicationFence> Default;

	public:
		static std::shared_ptr<TrapCompletionPublicationFence> getDefault();

		std::shared_ptr<TrapCompletionPublicationFenceResult> DenyProjectionOnly(std::shared_ptr<NeutralTrapResult> result, bool runtimeAdmissionAllowed);

		std::shared_ptr<TrapCompletionPublicationFenceResult> Evaluate(std::shared_ptr<NeutralTrapResult> result, bool runtimeAdmissionAllowed, bool completionPublicationAuthorized, bool retirePublicationAuthorized, unsigned int neutralReasonCode, unsigned long long qualification = 0, unsigned long long faultAddress = 0, unsigned long long faultAux = 0, EvidenceVisibilityClass evidenceClass = EvidenceVisibilityClass::HostOwnedRuntimeEvidence, TrapCompletionMigrationClass migrationClass = TrapCompletionMigrationClass::Unclassified);

		std::shared_ptr<TrapCompletionOwnerPolicyResult> EvaluateOwnerPolicy(std::shared_ptr<TrapCompletionRouteRequest> request, std::shared_ptr<TrapCompletionRouteResult> route);

	private:
		static std::shared_ptr<TrapCompletionPublicationFenceResult> PublishedCompletion(std::shared_ptr<NeutralTrapResult> result, unsigned int neutralReasonCode, unsigned long long qualification, unsigned long long faultAddress, unsigned long long faultAux, bool retirePublicationAuthorized, const std::wstring &reason);

		static bool CanPublishRetireEvidence(EvidenceVisibilityClass evidenceClass);

		static bool CanPublishRetireMigrationClass(TrapCompletionMigrationClass migrationClass);

		static std::shared_ptr<TrapCompletionPublicationFenceResult> Denied(TrapCompletionPublicationDecision decision, std::shared_ptr<NeutralTrapResult> result, const std::wstring &reason);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct TrapCompletionOwnerPolicyResult(bool IsAllowed, string Reason);

}

inline const std::shared_ptr<YAKSys_Hybrid_CPU::Core::TrapCompletionPublicationFence> YAKSys_Hybrid_CPU::Core::TrapCompletionPublicationFence::Default = std::make_shared<YAKSys_Hybrid_CPU::Core::TrapCompletionPublicationFence>();
