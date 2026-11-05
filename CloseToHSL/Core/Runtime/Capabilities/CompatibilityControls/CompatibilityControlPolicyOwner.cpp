#include "CompatibilityControlPolicyOwner.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CompatibilityControlPolicySnapshot::CompatibilityControlPolicySnapshot(std::any issuerSeal, std::shared_ptr<CompatibilityControlPolicyIdentity> identity, std::shared_ptr<CompatibilityControlDescriptor> descriptor) : Identity(std::move(identity)), Descriptor(std::move(descriptor)), _issuerSeal(std::move(issuerSeal))
	{
	}

	std::shared_ptr<CompatibilityControlPolicyIdentity> CompatibilityControlPolicySnapshot::getIdentity() const
	{
		return Identity;
	}

	std::shared_ptr<CompatibilityControlDescriptor> CompatibilityControlPolicySnapshot::getDescriptor() const
	{
		return Descriptor;
	}

	CompatibilityEventRoutingPolicy CompatibilityControlPolicySnapshot::getEventRoutingPolicy() const
	{
		return getDescriptor()->getReadOnlyView()->EventRoutingPolicy;
	}

	bool CompatibilityControlPolicySnapshot::isMaterialized() const
	{
		return getIdentity()->isMaterialized() && getDescriptor()->getHasMaterializedReadOnlyProjection();
	}

	bool CompatibilityControlPolicySnapshot::getRuntimeAuthorityGranted() const
	{
		return false;
	}

	bool CompatibilityControlPolicySnapshot::isCompatibilityValue() const
	{
		return false;
	}

	bool CompatibilityControlPolicySnapshot::WasIssuedBy(std::any issuerSeal)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		const auto *ours = std::any_cast<std::shared_ptr<const unsigned char>>(&_issuerSeal);
		const auto *candidate = std::any_cast<std::shared_ptr<const unsigned char>>(&issuerSeal);
		return ours != nullptr && candidate != nullptr && *ours == *candidate;
	}

	CompatibilityControlPolicyOwner::CompatibilityControlPolicyOwner(std::shared_ptr<CompatibilityControlPolicyConstructionProfile> profile)
	{
		if (!profile->IsConfigured)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Compatibility-control policy construction requires an explicit profile.", nameof(profile));
			throw std::invalid_argument("Compatibility-control policy construction requires an explicit profile.");
		}

		_domainIdentity = profile->DomainIdentity;
		_policyGeneration = 1;
		_current = IssueSnapshot(profile->EventRoutingPolicy);
	}

	std::shared_ptr<CompatibilityControlPolicySnapshot> CompatibilityControlPolicyOwner::CaptureCurrent()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return _current;
		}
	}

	unsigned long long CompatibilityControlPolicyOwner::getCurrentDomainIdentity() const
	{
		{
		std::scoped_lock<std::mutex> lock(_sync);
			return _domainIdentity;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool IsCurrent(CompatibilityControlPolicySnapshot? snapshot)
	bool CompatibilityControlPolicyOwner::IsCurrent(std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return IsCurrentUnderLock(snapshot);
		}
	}

	std::shared_ptr<CompatibilityControlPolicySnapshot> CompatibilityControlPolicyOwner::ReplacePolicy(CompatibilityEventRoutingPolicy eventRoutingPolicy)
	{
		ValidatePolicy(eventRoutingPolicy);
		{
			std::scoped_lock<std::mutex> lock(_sync);
			AdvanceNonZero(_ownerEpoch);
			AdvanceNonZero(_policyGeneration);
			_current = IssueSnapshot(eventRoutingPolicy);
			return _current;
		}
	}

	std::shared_ptr<CompatibilityControlPolicySnapshot> CompatibilityControlPolicyOwner::RebindDomain(unsigned long long domainIdentity)
	{
		if (domainIdentity == 0)
		{
			throw ArgumentOutOfRangeException(L"domainIdentity");
		}
		{
			std::scoped_lock<std::mutex> lock(_sync);
			CompatibilityEventRoutingPolicy policy = _current->getEventRoutingPolicy();
			_domainIdentity = domainIdentity;
			AdvanceNonZero(_ownerEpoch);
			AdvanceNonZero(_policyGeneration);
			_current = IssueSnapshot(policy);
			return _current;
		}
	}

	std::shared_ptr<CompatibilityControlPolicySnapshot> CompatibilityControlPolicyOwner::InvalidateAfterRestore()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			CompatibilityEventRoutingPolicy policy = _current->getEventRoutingPolicy();
			AdvanceNonZero(_policyGeneration);
			_current = IssueSnapshot(policy);
			return _current;
		}
	}

	std::shared_ptr<CompatibilityControlPolicySnapshot> CompatibilityControlPolicyOwner::ReplaceAfterArchitecturalStateReplacement()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			CompatibilityEventRoutingPolicy policy = _current->getEventRoutingPolicy();
			AdvanceNonZero(_ownerEpoch);
			AdvanceNonZero(_policyGeneration);
			_current = IssueSnapshot(policy);
			return _current;
		}
	}

	std::shared_ptr<CompatibilityControlPolicySnapshot> CompatibilityControlPolicyOwner::IssueSnapshot(CompatibilityEventRoutingPolicy eventRoutingPolicy)
	{
		auto baseline = std::make_shared<CompatibilityControlReadOnlyView>(*CompatibilityControlReadOnlyView::FailClosedProjectionOnly);
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
		baseline->EventRoutingPolicy = eventRoutingPolicy;
		auto descriptor = CompatibilityControlDescriptor::FromNeutralSemantics(std::move(baseline));
		return std::make_shared<CompatibilityControlPolicySnapshot>(_issuerSeal, std::make_shared<CompatibilityControlPolicyIdentity>(CanonicalOwnerIdentity, _ownerEpoch, _domainIdentity, _policyGeneration), descriptor);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool IsCurrentUnderLock(CompatibilityControlPolicySnapshot? snapshot)
	bool CompatibilityControlPolicyOwner::IsCurrentUnderLock(std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot)
	{
		return snapshot != nullptr && snapshot->isMaterialized() && snapshot->WasIssuedBy(_issuerSeal) && snapshot == _current && snapshot->getIdentity()->OwnerIdentity == CanonicalOwnerIdentity && snapshot->getIdentity()->OwnerEpoch == _ownerEpoch && snapshot->getIdentity()->DomainIdentity == _domainIdentity && snapshot->getIdentity()->PolicyGeneration == _policyGeneration;
	}

	void CompatibilityControlPolicyOwner::ValidatePolicy(CompatibilityEventRoutingPolicy policy)
	{
		if (policy == CompatibilityEventRoutingPolicy::None || (static_cast<int>(policy) & ~static_cast<int>(KnownEventRoutingPolicyMask)) != 0)
		{
			throw ArgumentOutOfRangeException(L"policy");
		}
	}

	void CompatibilityControlPolicyOwner::AdvanceNonZero(unsigned long long &value)
	{
//		checked
			value++;
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
		if (value == 0)
		{
			throw InvalidOperationException(L"Compatibility-control policy freshness exhausted.");
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal NeutralInterruptRoutingPolicyResult Validate(CompatibilityControlPolicyOwner owner, CompatibilityControlPolicySnapshot? snapshot, ulong expectedDomainIdentity)
	std::shared_ptr<NeutralInterruptRoutingPolicyResult> NeutralInterruptRoutingPolicyConsumer::Validate(std::shared_ptr<CompatibilityControlPolicyOwner> owner, std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot, unsigned long long expectedDomainIdentity)
	{
		if (!owner->IsCurrent(snapshot))
		{
			return Denied(NeutralInterruptRoutingPolicyDecision::DeniedStaleOrForeignSnapshot, L"Neutral interrupt routing requires the current owner-issued compatibility-control policy snapshot.");
		}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (snapshot!.Identity.DomainIdentity != expectedDomainIdentity || expectedDomainIdentity == 0)
		if (snapshot->getIdentity()->DomainIdentity != expectedDomainIdentity || expectedDomainIdentity == 0)
		{
			return Denied(NeutralInterruptRoutingPolicyDecision::DeniedDomainMismatch, L"Neutral interrupt routing policy must match the non-zero runtime domain identity.");
		}

		CompatibilityEventRoutingPolicy policy = snapshot->getEventRoutingPolicy();
		if ((static_cast<int>(policy) & static_cast<int>(CompatibilityEventRoutingPolicy::RuntimeTrapPolicyRequired)) == 0)
		{
			return Denied(NeutralInterruptRoutingPolicyDecision::DeniedRuntimeTrapPolicy, L"Neutral interrupt routing requires the runtime trap policy predicate.");
		}
		if ((static_cast<int>(policy) & static_cast<int>(CompatibilityEventRoutingPolicy::NeutralTrapResultRequired)) == 0)
		{
			return Denied(NeutralInterruptRoutingPolicyDecision::DeniedNeutralTrapResult, L"Neutral interrupt routing requires neutral trap-result semantics.");
		}
		if ((static_cast<int>(policy) & static_cast<int>(CompatibilityEventRoutingPolicy::PublicationFenceRequired)) == 0)
		{
			return Denied(NeutralInterruptRoutingPolicyDecision::DeniedPublicationFence, L"Neutral interrupt routing requires the publication fence predicate.");
		}
		return std::make_shared<NeutralInterruptRoutingPolicyResult>(NeutralInterruptRoutingPolicyDecision::Allowed, L"Current owner-issued neutral event-routing policy admitted interrupt routing.");
	}

	std::shared_ptr<NeutralInterruptRoutingPolicyResult> NeutralInterruptRoutingPolicyConsumer::Denied(NeutralInterruptRoutingPolicyDecision decision, const std::wstring &reason)
	{
		return std::make_shared<NeutralInterruptRoutingPolicyResult>(decision, reason);
	}
}
