#include "DomainHypercallExactRuntimeProfile.h"
#include "../../Migration/Checkpoint/DomainHypercallDrainLifecycleOwner.h"
#include "../../Capabilities/Grants/CapabilityGrant.h"
#include "../../Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"
#include "../../Domains/Authority/YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"
#include "../../Capabilities/Governance/YAKSys_Hybrid_CPU.Core.RuntimeCapabilityIds.h"
#include "../../Domains/Authority/RootAuthorityDescriptor.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"
#include "../../Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrantCollection.h"
#include "../../Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DomainHypercallExactRuntimeProfile::DomainHypercallExactRuntimeProfile(unsigned long long domainTag, std::shared_ptr<MicroOpScheduler> scheduler, std::shared_ptr<DomainHypercallRetireOwner> retireOwner)
		: _domainTag(domainTag), _scheduler(std::move(scheduler)), _retireOwner(std::move(retireOwner)), _lifecycleGate(std::shared_ptr<DomainHypercallLifecycleGate>(new DomainHypercallLifecycleGate(domainTag)))
	{
		if (domainTag == 0 || domainTag > std::numeric_limits<unsigned short>::max())
		{
			throw ArgumentOutOfRangeException(L"domainTag");
		}
		if (!_scheduler)
		{
			throw std::invalid_argument("scheduler");
		}
		if (!_retireOwner)
		{
			throw std::invalid_argument("retireOwner");
		}

	}

	unsigned long long DomainHypercallExactRuntimeProfile::getDomainTag() const
	{
		return _domainTag;
	}

	std::shared_ptr<DomainHypercallLifecycleGate> DomainHypercallExactRuntimeProfile::getLifecycleGate() const
	{
		return _lifecycleGate;
	}

	std::shared_ptr<DomainHypercallDrainLifecycleOwner> DomainHypercallExactRuntimeProfile::getDrainOwner() const
	{
		return _drainOwner;
	}

	std::shared_ptr<DomainHypercallCanonicalComposition> DomainHypercallExactRuntimeProfile::getComposition() const
	{
		return _composition;
	}

	std::shared_ptr<DomainHypercallRuntimeExecutor> DomainHypercallExactRuntimeProfile::getExecutor() const
	{
		return _executor;
	}

	std::shared_ptr<DomainHypercallCompletionOwner> DomainHypercallExactRuntimeProfile::getCompletionOwner() const
	{
		return _completionOwner;
	}

	std::shared_ptr<VirtualizationRestoreGenerationOwner> DomainHypercallExactRuntimeProfile::getRestoreOwner() const
	{
		return _restoreOwner;
	}

	std::shared_ptr<DomainHypercallExactActivationResult> DomainHypercallExactRuntimeProfile::Activate(std::shared_ptr<DomainHypercallExactActivationRequest> request)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			std::shared_ptr<DomainHypercallLifecycleSnapshot> current = _lifecycleGate->Observe();
			if (current->State == DomainHypercallLifecycleState::ActiveExactProfile)
			{
				return ActivationResult(DomainHypercallExactActivationDecision::AlreadyActive, L"The exact per-domain profile is already active.");
			}
			if (!request || !request->IsPhase38Exact())
			{
				return ActivationResult(DomainHypercallExactActivationDecision::DeniedNonExactProfile, L"Activation denied every profile except the exact accepted Phase-38 operation.");
			}
			if (_scheduler->getHasExactVirtualizationComposition())
			{
				return ActivationResult(DomainHypercallExactActivationDecision::ProvisioningFailed, L"Scheduler already has an exact composition binding; replacement is denied.");
			}
			if (_domainTag == 0 || _domainTag > std::numeric_limits<unsigned short>::max())
			{
				return ActivationResult(DomainHypercallExactActivationDecision::DeniedInvalidDomain, L"Exact activation requires one non-zero neutral domain tag within trap transport width.");
			}

			try
			{
				std::shared_ptr<CapabilityGrant> grant = CreateExactGrant(_domainTag);
				std::shared_ptr<RuntimeCapabilityGrantOwner> capabilityOwner = std::make_shared<RuntimeCapabilityGrantOwner>();
				std::shared_ptr<RuntimeCapabilityGrantLease> lease = capabilityOwner->Issue(grant);
				std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner = std::make_shared<VirtualizationRestoreGenerationOwner>();
				std::shared_ptr<DomainHypercallRuntimeExecutor> executor = std::make_shared<DomainHypercallRuntimeExecutor>(ExactProbeExecutionMode::ExactProbeOnly);
				std::shared_ptr<DomainHypercallCompletionOwner> completionOwner = std::make_shared<DomainHypercallCompletionOwner>();
				std::shared_ptr<DomainRuntimeContext> context = CreateExactDomainContext(_domainTag, grant);
				std::shared_ptr<RootAuthorityDescriptor> root = std::make_shared<RootAuthorityDescriptor>(RootAuthorityClass::RuntimeRoot, 1, RuntimeCapabilityIds::VmCallProbeNoStateV1Mask, false, false);

				if (!_lifecycleGate->TryActivateExact(request))
				{
					capabilityOwner->RevokeAll();
					return ActivationResult(DomainHypercallExactActivationDecision::ProvisioningFailed, L"Exact lifecycle activation failed closed before binding publication.");
				}

				std::shared_ptr<DomainHypercallCanonicalComposition> composition = std::make_shared<DomainHypercallCanonicalComposition>(context, root, capabilityOwner, lease, restoreOwner, executor, completionOwner, _retireOwner, _lifecycleGate);
				std::shared_ptr<SafetyVerifier> verifier = _scheduler->getExactVirtualizationCanonicalVerifier();
				if (!verifier)
				{
					throw InvalidOperationException(L"Canonical SafetyVerifier is unavailable.");
				}
				std::shared_ptr<DomainHypercallDrainLifecycleOwner> drainOwner = std::make_shared<DomainHypercallDrainLifecycleOwner>(_domainTag, composition, verifier, executor, completionOwner, _retireOwner, restoreOwner);
				_scheduler->ConfigureExactVirtualizationComposition(composition);

				_capabilityOwner = capabilityOwner;
				_capabilityLease = lease;
				_restoreOwner = restoreOwner;
				_executor = executor;
				_completionOwner = completionOwner;
				_composition = composition;
				_drainOwner = drainOwner;
				return ActivationResult(DomainHypercallExactActivationDecision::Activated, L"Activated only the exact accepted no-state probe profile for one neutral domain.");
			}
			catch (const std::runtime_error &ex)
			{
				if (_composition != nullptr)
				{
					_composition->Disable();
				}
				if (_capabilityOwner != nullptr)
				{
					_capabilityOwner->RevokeAll();
				}
				_lifecycleGate->BeginDrain();
				_lifecycleGate->RevokeExactBinding();
				if (_composition != nullptr)
				{
					_scheduler->DisableExactVirtualizationComposition();
				}
				ClearBinding();
				return ActivationResult(DomainHypercallExactActivationDecision::ProvisioningFailed, std::format(L"Exact activation rolled back fail-closed: {0:s}.", std::wstring(ex.what(), ex.what() + std::char_traits<char>::length(ex.what()))));
			}
		}
	}

	std::shared_ptr<DomainHypercallKillSwitchResult> DomainHypercallExactRuntimeProfile::KillSwitch(std::chrono::milliseconds transitionTimeout)
	{
		std::scoped_lock<std::mutex> lock(_sync);
		if (!_composition || !_drainOwner)
		{
			return std::make_shared<DomainHypercallKillSwitchResult>(DomainHypercallKillSwitchDecision::AlreadyDisabledFaultOnly,
				_lifecycleGate->Observe(), DomainHypercallLiveAuthorityCounts(0, 0, 0, 0, 0), false, false,
				std::vector<DomainHypercallKillSwitchStep>{DomainHypercallKillSwitchStep::DeterministicFaultOnlyFallbackRestored},
				L"Exact binding is absent; compatibility remains deterministic fault-only.");
		}
		std::shared_ptr<DomainHypercallDrainLifecycleOwner> drainOwner = _drainOwner;
		std::shared_ptr<DomainHypercallCanonicalComposition> composition = _composition;
		std::shared_ptr<RuntimeCapabilityGrantOwner> capabilityOwner = _capabilityOwner;
		std::shared_ptr<RuntimeCapabilityGrantLease> capabilityLease = _capabilityLease;
		composition->BeginDrain();
		std::vector<DomainHypercallKillSwitchStep> trace{DomainHypercallKillSwitchStep::NewE2Closed};
		std::shared_ptr<DomainHypercallDrainResult> drained = drainOwner->CancelAndCheckpoint(transitionTimeout);
		if (!drained->IsCheckpointReady)
		{
			const bool grantLive = capabilityOwner && capabilityOwner->IsLive(capabilityLease);
			return std::make_shared<DomainHypercallKillSwitchResult>(drained->Counts.TransitionsInFlight != 0 ? DomainHypercallKillSwitchDecision::TransitionDrainTimedOut : DomainHypercallKillSwitchDecision::RegistryDrainFailed,
				_lifecycleGate->Observe(), drained->Counts, true, grantLive, trace, drained->Reason);
		}
		trace.push_back(DomainHypercallKillSwitchStep::TransitionsQuiescent);
		trace.push_back(DomainHypercallKillSwitchStep::RegistriesQuiescent);
		_scheduler->DisableExactVirtualizationComposition();
		if (capabilityOwner) capabilityOwner->RevokeAll();
		const bool revoked = _lifecycleGate->RevokeExactBinding();
		const bool grantLive = capabilityOwner && capabilityOwner->IsLive(capabilityLease);
		if (!revoked || grantLive)
		{
			return std::make_shared<DomainHypercallKillSwitchResult>(DomainHypercallKillSwitchDecision::RevocationFailed,
				_lifecycleGate->Observe(), drained->Counts, static_cast<bool>(_composition), grantLive, trace,
				L"Exact binding/grant revocation failed closed in draining state.");
		}
		trace.push_back(DomainHypercallKillSwitchStep::ExactBindingAndGrantRevoked);
		ClearBinding();
		trace.push_back(DomainHypercallKillSwitchStep::DeterministicFaultOnlyFallbackRestored);
		return std::make_shared<DomainHypercallKillSwitchResult>(DomainHypercallKillSwitchDecision::DisabledFaultOnly,
			_lifecycleGate->Observe(), drained->Counts, false, false, trace,
			L"Kill switch restored the unbound deterministic compatibility fault-only fallback.");
	}

	std::shared_ptr<DomainHypercallExactActivationResult> DomainHypercallExactRuntimeProfile::ActivationResult(DomainHypercallExactActivationDecision decision, const std::wstring &reason)
	{
		const bool grantLive = _capabilityOwner && _capabilityOwner->IsLive(_capabilityLease);
		return std::make_shared<DomainHypercallExactActivationResult>(decision, _lifecycleGate->Observe(), static_cast<bool>(_composition), grantLive, reason);
	}

	void DomainHypercallExactRuntimeProfile::ClearBinding()
	{
		_composition = nullptr;
		_drainOwner = nullptr;
		_executor = nullptr;
		_completionOwner = nullptr;
		_restoreOwner = nullptr;
		_capabilityLease = nullptr;
		_capabilityOwner = nullptr;
	}

	std::shared_ptr<CapabilityGrant> DomainHypercallExactRuntimeProfile::CreateExactGrant(unsigned long long domainTag)
	{
		return std::make_shared<CapabilityGrant>(RuntimeCapabilityIds::VmCallProbeNoStateV1Mask, CapabilityGrantScope::DomainGranted, true, domainTag,
			CapabilityDelegationPolicy::NonDelegable, CapabilityRevocationPolicy::RuntimeRevocable, CapabilityMigrationClass::DomainLocal,
			CapabilityEvidenceVisibility::HostOnly, CapabilityFrontendProjectionPolicy::NeverProject);
	}

	std::shared_ptr<DomainRuntimeContext> DomainHypercallExactRuntimeProfile::CreateExactDomainContext(unsigned long long domainTag, std::shared_ptr<CapabilityGrant> grant)
	{
		std::vector<std::shared_ptr<CapabilityGrant>> grants{grant};
		std::shared_ptr<CapabilityGrantCollection> collection = std::make_shared<CapabilityGrantCollection>(grants);
		std::shared_ptr<CapabilityDescriptorSet> capabilities = std::make_shared<CapabilityDescriptorSet>(collection);
		std::shared_ptr<ExecutionDomainDescriptor> execution = std::make_shared<ExecutionDomainDescriptor>(domainTag, std::make_shared<BundleLegalityDescriptor>(), nullptr, nullptr, false);
		return std::make_shared<DomainRuntimeContext>(execution, nullptr, nullptr, capabilities, nullptr, domainTag, 0);
	}
}
