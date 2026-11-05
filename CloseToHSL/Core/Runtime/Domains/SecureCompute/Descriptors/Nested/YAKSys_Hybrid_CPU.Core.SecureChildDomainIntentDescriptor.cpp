#include "YAKSys_Hybrid_CPU.Core.SecureChildDomainIntentDescriptor.h"
#include "../../Authority/YAKSys_Hybrid_CPU.Core.SecurePolicyDerivationRecord.h"
#include "../Domain/YAKSys_Hybrid_CPU.Core.SecureComputeDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureChildDomainIntentDescriptor::SecureChildDomainIntentDescriptor() : SecureChildDomainIntentDescriptor(0, 0, SecureComputeSecurityLevel::Disabled, SecureAuthorityBounds::getNone(), SecurePolicyDerivationRecord::None, SecureChildDomainIntentState::Missing)
	{
	}

	SecureChildDomainIntentDescriptor::SecureChildDomainIntentDescriptor(unsigned long long parentDomainTag, unsigned long long childDomainTag, SecureComputeSecurityLevel requestedSecurityLevel, std::shared_ptr<SecureAuthorityBounds> requestedBounds, std::shared_ptr<SecurePolicyDerivationRecord> derivation, SecureChildDomainIntentState state)
		: ParentDomainTag(parentDomainTag), ChildDomainTag(childDomainTag),
		  RequestedSecurityLevel(SecureComputeDomainDescriptor::NormalizeSecurityLevel(requestedSecurityLevel)),
		  RequestedBounds(std::move(requestedBounds)), Derivation(std::move(derivation)), State(state)
	{
	}

	std::shared_ptr<SecureChildDomainIntentDescriptor> SecureChildDomainIntentDescriptor::getMissing()
	{
		return Missing;
	}

	unsigned long long SecureChildDomainIntentDescriptor::getParentDomainTag() const
	{
		return ParentDomainTag;
	}

	unsigned long long SecureChildDomainIntentDescriptor::getChildDomainTag() const
	{
		return ChildDomainTag;
	}

	SecureComputeSecurityLevel SecureChildDomainIntentDescriptor::getRequestedSecurityLevel() const
	{
		return RequestedSecurityLevel;
	}

	std::shared_ptr<SecureAuthorityBounds> SecureChildDomainIntentDescriptor::getRequestedBounds() const
	{
		return RequestedBounds;
	}

	std::shared_ptr<SecurePolicyDerivationRecord> SecureChildDomainIntentDescriptor::getDerivation() const
	{
		return Derivation;
	}

	SecureChildDomainIntentState SecureChildDomainIntentDescriptor::getState() const
	{
		return State;
	}

	bool SecureChildDomainIntentDescriptor::isMaterialized() const
	{
		return getState() != SecureChildDomainIntentState::Missing && getParentDomainTag() != 0 && getChildDomainTag() != 0;
	}

	bool SecureChildDomainIntentDescriptor::isSecureRequest() const
	{
		return getRequestedSecurityLevel() != SecureComputeSecurityLevel::Disabled;
	}

	SecurePolicyDerivationDecision SecureChildDomainIntentDescriptor::ValidateMonotonicDerivation(std::shared_ptr<SecureAuthorityBounds> parentBounds, std::shared_ptr<SecureRevocationEpoch> currentEpoch)
	{
		if (!isMaterialized() || !isSecureRequest() || parentBounds == nullptr)
		{
			return SecurePolicyDerivationDecision::ParentMissing;
		}

		std::shared_ptr<SecurePolicyDerivationRecord> derivation = getDerivation();
		if (!derivation || !derivation->HasProvenance)
		{
			return SecurePolicyDerivationDecision::ProvenanceMissing;
		}

		if (!derivation->MatchesEpoch(currentEpoch))
		{
			return SecurePolicyDerivationDecision::EpochMismatch;
		}

		std::shared_ptr<SecureAuthorityBounds> requestedBounds = getRequestedBounds();
		if (!requestedBounds || !requestedBounds->IsSubsetOf(parentBounds))
		{
			return SecurePolicyDerivationDecision::ChildExpandsAuthority;
		}

		return SecurePolicyDerivationDecision::AllowedSubset;
	}
}
