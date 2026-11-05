#pragma once

#include "../Domain/SecureComputeDomainDescriptor.h"
#include "../../Authority/YAKSys_Hybrid_CPU.Core.SecureAuthorityBounds.h"
#include "SecureChildDomainIntentDescriptor.h"
#include "../../Authority/SecurePolicyDerivationRecord.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecurePolicyDerivationRecord;
	struct SecureRevocationEpoch;
	class SecureChildDomainIntentDescriptor final : public std::enable_shared_from_this<SecureChildDomainIntentDescriptor>
	{
	private:
		static inline const std::shared_ptr<SecureChildDomainIntentDescriptor> Missing = std::make_shared<SecureChildDomainIntentDescriptor>();
		const unsigned long long ParentDomainTag;
		const unsigned long long ChildDomainTag;
		const SecureComputeSecurityLevel RequestedSecurityLevel;
		const std::shared_ptr<SecureAuthorityBounds> RequestedBounds;
		const std::shared_ptr<SecurePolicyDerivationRecord> Derivation;
		const SecureChildDomainIntentState State;

	public:
		SecureChildDomainIntentDescriptor();

		SecureChildDomainIntentDescriptor(unsigned long long parentDomainTag, unsigned long long childDomainTag, SecureComputeSecurityLevel requestedSecurityLevel, std::shared_ptr<SecureAuthorityBounds> requestedBounds, std::shared_ptr<SecurePolicyDerivationRecord> derivation, SecureChildDomainIntentState state);

		static std::shared_ptr<SecureChildDomainIntentDescriptor> getMissing();

		unsigned long long getParentDomainTag() const;

		unsigned long long getChildDomainTag() const;

		SecureComputeSecurityLevel getRequestedSecurityLevel() const;

		std::shared_ptr<SecureAuthorityBounds> getRequestedBounds() const;

		std::shared_ptr<SecurePolicyDerivationRecord> getDerivation() const;

		SecureChildDomainIntentState getState() const;

		bool isMaterialized() const;

		bool isSecureRequest() const;

		SecurePolicyDerivationDecision ValidateMonotonicDerivation(std::shared_ptr<SecureAuthorityBounds> parentBounds, std::shared_ptr<SecureRevocationEpoch> currentEpoch);
	};
}
