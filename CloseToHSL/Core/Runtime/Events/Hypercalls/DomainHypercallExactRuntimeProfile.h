#pragma once

#include "../../../Pipeline/Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include "../../Completion/Records/DomainHypercallRetireOwner.h"
#include "../../Migration/Checkpoint/DomainHypercallLifecycleGate.h"
#include "../../Capabilities/Grants/RuntimeCapabilityGrantLease.h"
#include "VirtualizationOperandSnapshot.h"
#include "DomainHypercallRuntimeExecutor.h"
#include "../../Completion/Records/DomainHypercallCompletionOwner.h"
#include "DomainHypercallCanonicalComposition.h"
namespace YAKSys_Hybrid_CPU::Core { struct DomainHypercallExactActivationResult; struct DomainHypercallKillSwitchResult; }
namespace YAKSys_Hybrid_CPU::Core { class DomainHypercallDrainLifecycleOwner; }
#include "../../Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <optional>
#include <mutex>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainHypercallExactActivationDecision : unsigned char
	{
		Activated = 0,
		AlreadyActive = 1,
		DeniedNonExactProfile = 2,
		DeniedInvalidDomain = 3,
		ProvisioningFailed = 4
	};

	enum class DomainHypercallKillSwitchDecision : unsigned char
	{
		DisabledFaultOnly = 0,
		AlreadyDisabledFaultOnly = 1,
		TransitionDrainTimedOut = 2,
		RegistryDrainFailed = 3,
		RevocationFailed = 4
	};

	enum class DomainHypercallKillSwitchStep : unsigned char
	{
		NewE2Closed = 0,
		TransitionsQuiescent = 1,
		RegistriesQuiescent = 2,
		ExactBindingAndGrantRevoked = 3,
		DeterministicFaultOnlyFallbackRestored = 4
	};
	struct DomainHypercallExactActivationResult
	{
		DomainHypercallExactActivationDecision Decision;
		std::shared_ptr<DomainHypercallLifecycleSnapshot> Lifecycle;
		bool ExactBindingPresent, ExactGrantLive;
		std::wstring Reason;
		bool IsActivated = false;
		DomainHypercallExactActivationResult(DomainHypercallExactActivationDecision decision, std::shared_ptr<DomainHypercallLifecycleSnapshot> lifecycle, bool bindingPresent, bool grantLive, std::wstring reason)
			: Decision(decision), Lifecycle(std::move(lifecycle)), ExactBindingPresent(bindingPresent), ExactGrantLive(grantLive), Reason(std::move(reason)), IsActivated(decision == DomainHypercallExactActivationDecision::Activated && Lifecycle && Lifecycle->State == DomainHypercallLifecycleState::ActiveExactProfile && bindingPresent && grantLive) {}
	};
	struct DomainHypercallKillSwitchResult
	{
		DomainHypercallKillSwitchDecision Decision;
		std::shared_ptr<DomainHypercallLifecycleSnapshot> Lifecycle;
		DomainHypercallLiveAuthorityCounts Counts;
		bool ExactBindingPresent, ExactGrantLive;
		std::vector<DomainHypercallKillSwitchStep> Trace;
		std::wstring Reason;
		bool IsDeterministicFaultOnly = false;
		DomainHypercallKillSwitchResult(DomainHypercallKillSwitchDecision decision, std::shared_ptr<DomainHypercallLifecycleSnapshot> lifecycle, DomainHypercallLiveAuthorityCounts counts, bool bindingPresent, bool grantLive, std::vector<DomainHypercallKillSwitchStep> trace, std::wstring reason)
			: Decision(decision), Lifecycle(std::move(lifecycle)), Counts(counts), ExactBindingPresent(bindingPresent), ExactGrantLive(grantLive), Trace(std::move(trace)), Reason(std::move(reason)), IsDeterministicFaultOnly((decision == DomainHypercallKillSwitchDecision::DisabledFaultOnly || decision == DomainHypercallKillSwitchDecision::AlreadyDisabledFaultOnly) && Lifecycle && Lifecycle->State == DomainHypercallLifecycleState::DisabledFaultOnly && Counts.getIsDrained() && !bindingPresent && !grantLive) {}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct DomainHypercallExactActivationResult(DomainHypercallExactActivationDecision Decision, DomainHypercallLifecycleSnapshot Lifecycle, bool ExactBindingPresent, bool ExactGrantLive, string Reason)
//	{
//		internal bool IsActivated
//		{
//			get
//			{
//				return Decision == DomainHypercallExactActivationDecision.Activated && Lifecycle.State == DomainHypercallLifecycleState.ActiveExactProfile && ExactBindingPresent && ExactGrantLive;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct DomainHypercallKillSwitchResult(DomainHypercallKillSwitchDecision Decision, DomainHypercallLifecycleSnapshot Lifecycle, DomainHypercallLiveAuthorityCounts Counts, bool ExactBindingPresent, bool ExactGrantLive, IReadOnlyList<DomainHypercallKillSwitchStep> Trace, string Reason)
//	{
//		internal bool IsDeterministicFaultOnly
//		{
//			get
//			{
//				return Decision is DomainHypercallKillSwitchDecision.DisabledFaultOnly or DomainHypercallKillSwitchDecision.AlreadyDisabledFaultOnly && Lifecycle.State == DomainHypercallLifecycleState.DisabledFaultOnly && Counts.IsDrained && !ExactBindingPresent && !ExactGrantLive;
//			}
//		}
//	}

	/// <summary>
	/// Default-disabled provisioning contour for one exact domain profile. This
	/// object coordinates existing neutral owners and owns no E2/E3/E5/E6 authority.
	/// VMX compatibility remains an unbound, deterministic fault-only frontend.
	/// </summary>
	class DomainHypercallExactRuntimeProfile final : public std::enable_shared_from_this<DomainHypercallExactRuntimeProfile>
	{
	private:
		std::mutex _sync;
		const unsigned long long _domainTag;
		const std::shared_ptr<MicroOpScheduler> _scheduler;
		const std::shared_ptr<DomainHypercallRetireOwner> _retireOwner;
		const std::shared_ptr<DomainHypercallLifecycleGate> _lifecycleGate;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private RuntimeCapabilityGrantOwner? _capabilityOwner;
		std::shared_ptr<RuntimeCapabilityGrantOwner> _capabilityOwner;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private RuntimeCapabilityGrantLease? _capabilityLease;
		std::shared_ptr<RuntimeCapabilityGrantLease> _capabilityLease;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private VirtualizationRestoreGenerationOwner? _restoreOwner;
		std::shared_ptr<VirtualizationRestoreGenerationOwner> _restoreOwner;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DomainHypercallRuntimeExecutor? _executor;
		std::shared_ptr<DomainHypercallRuntimeExecutor> _executor;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DomainHypercallCompletionOwner? _completionOwner;
		std::shared_ptr<DomainHypercallCompletionOwner> _completionOwner;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DomainHypercallCanonicalComposition? _composition;
		std::shared_ptr<DomainHypercallCanonicalComposition> _composition;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DomainHypercallDrainLifecycleOwner? _drainOwner;
		std::shared_ptr<DomainHypercallDrainLifecycleOwner> _drainOwner;

	public:
		DomainHypercallExactRuntimeProfile(unsigned long long domainTag, std::shared_ptr<MicroOpScheduler> scheduler, std::shared_ptr<DomainHypercallRetireOwner> retireOwner);

		unsigned long long getDomainTag() const;
		std::shared_ptr<DomainHypercallLifecycleGate> getLifecycleGate() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallDrainLifecycleOwner? DrainOwner
		std::shared_ptr<DomainHypercallDrainLifecycleOwner> getDrainOwner() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallCanonicalComposition? Composition
		std::shared_ptr<DomainHypercallCanonicalComposition> getComposition() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallRuntimeExecutor? Executor
		std::shared_ptr<DomainHypercallRuntimeExecutor> getExecutor() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallCompletionOwner? CompletionOwner
		std::shared_ptr<DomainHypercallCompletionOwner> getCompletionOwner() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationRestoreGenerationOwner? RestoreOwner
		std::shared_ptr<VirtualizationRestoreGenerationOwner> getRestoreOwner() const;

		std::shared_ptr<DomainHypercallExactActivationResult> Activate(std::shared_ptr<DomainHypercallExactActivationRequest> request);

		std::shared_ptr<DomainHypercallKillSwitchResult> KillSwitch(std::chrono::milliseconds transitionTimeout);

	private:
		std::shared_ptr<DomainHypercallExactActivationResult> ActivationResult(DomainHypercallExactActivationDecision decision, const std::wstring &reason);

		void ClearBinding();

		static std::shared_ptr<CapabilityGrant> CreateExactGrant(unsigned long long domainTag);

		static std::shared_ptr<DomainRuntimeContext> CreateExactDomainContext(unsigned long long domainTag, std::shared_ptr<CapabilityGrant> grant);
	};

}
