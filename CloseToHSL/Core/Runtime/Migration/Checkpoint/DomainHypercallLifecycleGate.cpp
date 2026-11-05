#include "DomainHypercallLifecycleGate.h"
#include "../../Events/Hypercalls/Governance/Phase38VirtualizationDecisionSpecV2.h"
#include "../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"
#include "../../Events/Hypercalls/Governance/Phase38VirtualizationDecisionAcceptanceV2.h"

namespace YAKSys_Hybrid_CPU::Core
{
	bool DomainHypercallExactActivationRequest::IsPhase38Exact() const
	{
		return DecisionId == VirtualizationDecisionValidatorV2::ExpectedDecisionId && SpecDigest == Phase38VirtualizationDecisionSpecV2::getInstance()->SpecDigest && OperationNamespace == VirtualizationDecisionValidatorV2::ExpectedOperationNamespace && NumericLeaf == 1 && OperationId == VirtualizationDecisionValidatorV2::ExpectedOperationId && OwnerId == VirtualizationDecisionValidatorV2::ExpectedOwnerId && OwnerPolicyVersion == 1 && OwnerEpoch == 1;
	}

	std::shared_ptr<DomainHypercallExactActivationRequest> DomainHypercallExactActivationRequest::Phase38Exact()
	{
		return std::make_shared<DomainHypercallExactActivationRequest>(VirtualizationDecisionValidatorV2::ExpectedDecisionId, Phase38VirtualizationDecisionSpecV2::getInstance()->SpecDigest, VirtualizationDecisionValidatorV2::ExpectedOperationNamespace, 1, VirtualizationDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionValidatorV2::ExpectedOwnerId, 1, 1);
	}

	DomainHypercallLifecycleGate::TransitionLease::TransitionLease(std::shared_ptr<DomainHypercallLifecycleGate> owner, DomainHypercallTransitionKind kind, unsigned long long lifecycleEpoch)
		: Kind(kind), LifecycleEpoch(lifecycleEpoch), _owner(std::move(owner))
	{
	}

	DomainHypercallTransitionKind DomainHypercallLifecycleGate::TransitionLease::getKind() const
	{
		return Kind;
	}

	unsigned long long DomainHypercallLifecycleGate::TransitionLease::getLifecycleEpoch() const
	{
		return LifecycleEpoch;
	}

	std::shared_ptr<DomainHypercallLifecycleGate> DomainHypercallLifecycleGate::TransitionLease::getLifecycleGate() const
	{
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: return _owner ?? throw new ObjectDisposedException(nameof(TransitionLease));
		std::shared_ptr<DomainHypercallLifecycleGate> owner = _owner.load(std::memory_order_acquire);
		if (!owner) throw std::runtime_error("TransitionLease has been disposed.");
		return owner;
	}

	void DomainHypercallLifecycleGate::TransitionLease::dispose()
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: DomainHypercallLifecycleGate? owner = Interlocked.Exchange(ref _owner, null);
		std::shared_ptr<DomainHypercallLifecycleGate> owner = _owner.exchange(nullptr, std::memory_order_acq_rel);
		if (owner != nullptr)
		{
			owner->CompleteTransition(shared_from_this());
		}
	}

#if defined(TESTING)
	std::function<void(DomainHypercallTransitionKind)> DomainHypercallLifecycleGate::getTransitionGapTestHook() const
	{
		return TransitionGapTestHook;
	}
#endif

#if defined(TESTING)
	void DomainHypercallLifecycleGate::setTransitionGapTestHook(const std::function<void(DomainHypercallTransitionKind)> &value)
	{
		TransitionGapTestHook = value;
	}
#endif

	DomainHypercallLifecycleGate::DomainHypercallLifecycleGate(unsigned long long domainTag)
		: _domainTag(domainTag)
	{
		if (domainTag == 0)
		{
			throw ArgumentOutOfRangeException(L"domainTag");
		}
	}

	unsigned long long DomainHypercallLifecycleGate::getDomainTag() const
	{
		return _domainTag;
	}

	std::shared_ptr<DomainHypercallLifecycleSnapshot> DomainHypercallLifecycleGate::Observe()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return std::make_shared<DomainHypercallLifecycleSnapshot>(_domainTag, _state, _lifecycleEpoch, _transitionsInFlight);
		}
	}

	bool DomainHypercallLifecycleGate::TryActivateExact(std::shared_ptr<DomainHypercallExactActivationRequest> request)
	{
		if (!request || !request->IsPhase38Exact())
		{
			return false;
		}

		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (_state != DomainHypercallLifecycleState::DisabledFaultOnly || _transitionsInFlight != 0)
			{
				return false;
			}
			_state = DomainHypercallLifecycleState::ActiveExactProfile;
			AdvanceEpoch();
			return true;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryBeginTransition(ulong domainTag, DomainHypercallTransitionKind kind, out TransitionLease? lease)
	bool DomainHypercallLifecycleGate::TryBeginTransition(unsigned long long domainTag, DomainHypercallTransitionKind kind, std::shared_ptr<TransitionLease> &lease)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (domainTag != _domainTag || _state != DomainHypercallLifecycleState::ActiveExactProfile)
			{
				lease = nullptr;
				return false;
			}

//			checked
				_transitionsInFlight++;
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
			lease = std::make_shared<TransitionLease>(shared_from_this(), kind, _lifecycleEpoch);
			return true;
		}
	}

	void DomainHypercallLifecycleGate::BeginDrain()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (_state == DomainHypercallLifecycleState::ActiveExactProfile)
			{
				_state = DomainHypercallLifecycleState::Draining;
				AdvanceEpoch();
			}
		}
	}

	bool DomainHypercallLifecycleGate::WaitForTransitionQuiescence(std::chrono::milliseconds timeout)
    {
        constexpr auto infinite = std::chrono::milliseconds(-1);
        if (timeout < std::chrono::milliseconds::zero() && timeout != infinite)
            throw std::invalid_argument("timeout");
        std::unique_lock<std::mutex> lock(_sync);
        auto ready = [this] { return _transitionsInFlight == 0; };
        if (timeout == infinite) { _quiescent.wait(lock, ready); return true; }
        return _quiescent.wait_for(lock, timeout, ready);
    }
bool DomainHypercallLifecycleGate::ResumeAfterValidatedRestore(const std::wstring &specDigest)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
			if (_state != DomainHypercallLifecycleState::Draining || _transitionsInFlight != 0 || specDigest != Phase38VirtualizationDecisionSpecV2::getInstance()->SpecDigest)
			{
				return false;
			}
			_state = DomainHypercallLifecycleState::ActiveExactProfile;
			AdvanceEpoch();
			return true;
		}
	}

	bool DomainHypercallLifecycleGate::RevokeExactBinding()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (_transitionsInFlight != 0)
			{
				return false;
			}
			_state = DomainHypercallLifecycleState::DisabledFaultOnly;
			AdvanceEpoch();
			return true;
		}
	}

#if defined(TESTING)
	void DomainHypercallLifecycleGate::NotifyTransitionGapForTesting(DomainHypercallTransitionKind kind)
	{
		auto hook = getTransitionGapTestHook();
		if (hook) hook(kind);
	}
#endif

	void DomainHypercallLifecycleGate::CompleteTransition(std::shared_ptr<TransitionLease> lease)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (lease->getLifecycleEpoch() == 0 || _transitionsInFlight <= 0)
			{
				throw InvalidOperationException(L"Exact lifecycle transition accounting underflow.");
			}
			_transitionsInFlight--;
			if (_transitionsInFlight == 0)
			{
				_quiescent.notify_all();
			}
		}
	}

	void DomainHypercallLifecycleGate::AdvanceEpoch()
	{
//		unchecked
			_lifecycleEpoch++;
			if (_lifecycleEpoch == 0)
			{
				_lifecycleEpoch = 1;
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}
}
