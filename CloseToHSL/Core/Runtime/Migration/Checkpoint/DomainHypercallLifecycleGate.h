#pragma once

#include <atomic>

#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <utility>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../../../tangible_disposable_interface.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainHypercallLifecycleState : unsigned char
	{
		DisabledFaultOnly = 0,
		ActiveExactProfile = 1,
		Draining = 2
	};

	enum class DomainHypercallTransitionKind : unsigned char
	{
		NewE2 = 0,
		E2ToE3 = 1,
		E3ToE5 = 2,
		E5ToE6 = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct DomainHypercallExactActivationRequest(string DecisionId, string SpecDigest, string OperationNamespace, ushort NumericLeaf, string OperationId, ulong OwnerId, uint OwnerPolicyVersion, uint OwnerEpoch)
//	{
//		internal static DomainHypercallExactActivationRequest Phase38Exact
//		{
//			get
//			{
//				return new(VirtualizationDecisionValidatorV2.ExpectedDecisionId, Phase38VirtualizationDecisionSpecV2.Instance.SpecDigest, VirtualizationDecisionValidatorV2.ExpectedOperationNamespace, 0x0001, VirtualizationDecisionValidatorV2.ExpectedOperationId, VirtualizationDecisionValidatorV2.ExpectedOwnerId, 1, 1);
//			}
//		}
//
//		internal bool IsPhase38Exact
//		{
//			get
//			{
//				return string.Equals(DecisionId, VirtualizationDecisionValidatorV2.ExpectedDecisionId, StringComparison.Ordinal) && string.Equals(SpecDigest, Phase38VirtualizationDecisionSpecV2.Instance.SpecDigest, StringComparison.Ordinal) && string.Equals(OperationNamespace, VirtualizationDecisionValidatorV2.ExpectedOperationNamespace, StringComparison.Ordinal) && NumericLeaf == 0x0001 && string.Equals(OperationId, VirtualizationDecisionValidatorV2.ExpectedOperationId, StringComparison.Ordinal) && OwnerId == VirtualizationDecisionValidatorV2.ExpectedOwnerId && OwnerPolicyVersion == 1 && OwnerEpoch == 1;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct DomainHypercallLifecycleSnapshot(ulong DomainTag, DomainHypercallLifecycleState State, ulong LifecycleEpoch, int TransitionsInFlight)
//	{
//		internal bool AcceptsNewE2
//		{
//			get
//			{
//				return State == DomainHypercallLifecycleState.ActiveExactProfile;
//			}
//		}
//		internal bool IsTransitionQuiescent
//		{
//			get
//			{
//				return TransitionsInFlight == 0;
//			}
//		}
//	}

	/// <summary>
	/// Per-domain lifecycle/quiescence gate for the exact Phase-38 runtime profile.
	/// It owns no operation authority. E2/E3/E5/E6 remain owned by their existing
	/// neutral registries; this gate only prevents an authority handoff from being
	/// mistaken for cross-registry quiescence.
	/// </summary>
	struct DomainHypercallExactActivationRequest
	{
		const std::wstring DecisionId, SpecDigest, OperationNamespace;
		const unsigned short NumericLeaf;
		const std::wstring OperationId;
		const unsigned long long OwnerId;
		const unsigned int OwnerPolicyVersion, OwnerEpoch;
		DomainHypercallExactActivationRequest(std::wstring decisionId, std::wstring specDigest,
			std::wstring operationNamespace, unsigned short numericLeaf, std::wstring operationId,
			unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch)
			: DecisionId(std::move(decisionId)), SpecDigest(std::move(specDigest)),
			  OperationNamespace(std::move(operationNamespace)), NumericLeaf(numericLeaf),
			  OperationId(std::move(operationId)), OwnerId(ownerId),
			  OwnerPolicyVersion(ownerPolicyVersion), OwnerEpoch(ownerEpoch) {}
		bool IsPhase38Exact() const;
		static std::shared_ptr<DomainHypercallExactActivationRequest> Phase38Exact();
	};
    struct DomainHypercallLifecycleSnapshot final {
        const unsigned long long DomainTag;
        const DomainHypercallLifecycleState State;
        const unsigned long long LifecycleEpoch;
        const int TransitionsInFlight;
        const bool AcceptsNewE2;
        const bool IsTransitionQuiescent;
        DomainHypercallLifecycleSnapshot(unsigned long long domainTag, DomainHypercallLifecycleState state, unsigned long long epoch, int inFlight)
            : DomainTag(domainTag), State(state), LifecycleEpoch(epoch), TransitionsInFlight(inFlight),
              AcceptsNewE2(state == DomainHypercallLifecycleState::ActiveExactProfile), IsTransitionQuiescent(inFlight == 0) {}
    };
	struct DomainHypercallLiveAuthorityCounts
	{
		int E2, E3, E5, E6, TransitionsInFlight;
		DomainHypercallLiveAuthorityCounts(int e2, int e3, int e5, int e6, int inFlight = 0) : E2(e2), E3(e3), E5(e5), E6(e6), TransitionsInFlight(inFlight) {}
		bool getIsDrained() const { return E2 == 0 && E3 == 0 && E5 == 0 && E6 == 0 && TransitionsInFlight == 0; }
		int getTotal() const { return E2 + E3 + E5 + E6 + TransitionsInFlight; }
	};
class DomainHypercallLifecycleGate final : public std::enable_shared_from_this<DomainHypercallLifecycleGate>
	{
		friend class DomainHypercallExactRuntimeProfile;
	private:
		std::function<void(DomainHypercallTransitionKind)> TransitionGapTestHook;

	public:
		class TransitionLease final : public Disposable, public std::enable_shared_from_this<TransitionLease>
		{
		private:
			const DomainHypercallTransitionKind Kind;
			const unsigned long long LifecycleEpoch;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DomainHypercallLifecycleGate? _owner;
			std::atomic<std::shared_ptr<DomainHypercallLifecycleGate>> _owner{};

		public:
			TransitionLease(std::shared_ptr<DomainHypercallLifecycleGate> owner, DomainHypercallTransitionKind kind, unsigned long long lifecycleEpoch);

			DomainHypercallTransitionKind getKind() const;
			unsigned long long getLifecycleEpoch() const;
			std::shared_ptr<DomainHypercallLifecycleGate> getLifecycleGate() const;

			void dispose() override;
		};

	private:
		std::mutex _sync; std::condition_variable _quiescent;
		const unsigned long long _domainTag;
		DomainHypercallLifecycleState _state = DomainHypercallLifecycleState::DisabledFaultOnly;
		unsigned long long _lifecycleEpoch = 1;
		int _transitionsInFlight = 0;

#if defined(TESTING)
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal System.Action<DomainHypercallTransitionKind>? TransitionGapTestHook {get;set;}
	public:
		std::function<void(DomainHypercallTransitionKind)> getTransitionGapTestHook() const;
		void setTransitionGapTestHook(std::function<void(DomainHypercallTransitionKind)> value);
#endif

		DomainHypercallLifecycleGate(unsigned long long domainTag);

	public:
		unsigned long long getDomainTag() const;

		std::shared_ptr<DomainHypercallLifecycleSnapshot> Observe();

		bool TryActivateExact(std::shared_ptr<DomainHypercallExactActivationRequest> request);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryBeginTransition(ulong domainTag, DomainHypercallTransitionKind kind, out TransitionLease? lease)
		bool TryBeginTransition(unsigned long long domainTag, DomainHypercallTransitionKind kind, std::shared_ptr<TransitionLease> &lease);

		void BeginDrain();

		bool WaitForTransitionQuiescence(std::chrono::milliseconds timeout);

		bool ResumeAfterValidatedRestore(const std::wstring &specDigest);

		bool RevokeExactBinding();

#if defined(TESTING)
		void NotifyTransitionGapForTesting(DomainHypercallTransitionKind kind);
#endif

	private:
		void CompleteTransition(std::shared_ptr<TransitionLease> lease);

		void AdvanceEpoch();
	};

}
