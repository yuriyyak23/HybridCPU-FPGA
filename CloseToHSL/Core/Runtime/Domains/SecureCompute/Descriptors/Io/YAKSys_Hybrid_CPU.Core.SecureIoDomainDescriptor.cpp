#include "YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureIoDomainDescriptor::SecureIoDomainDescriptor() : SecureIoDomainDescriptor(SecureIoDmaPolicy::Denied, std::vector<std::shared_ptr<SecureSharedBufferDescriptor>>{}, true, false)
	{
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, IReadOnlyList<SecureSharedBufferDescriptor> sharedBuffers, bool requireCompletionFence) : this(dmaPolicy, sharedBuffers, requireCompletionFence, neutralIoOwnerMaterialized: false)
	SecureIoDomainDescriptor::SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, const std::vector<std::shared_ptr<SecureSharedBufferDescriptor>> &sharedBuffers, bool requireCompletionFence) : SecureIoDomainDescriptor(dmaPolicy, sharedBuffers, requireCompletionFence, false)
	{
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, IReadOnlyList<SecureSharedBufferDescriptor> sharedBuffers, bool requireCompletionFence, bool neutralIoOwnerMaterialized)
	SecureIoDomainDescriptor::SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, const std::vector<std::shared_ptr<SecureSharedBufferDescriptor>> &sharedBuffers, bool requireCompletionFence, bool neutralIoOwnerMaterialized) : DmaPolicy(dmaPolicy), SharedBuffers(sharedBuffers), RequireCompletionFence(requireCompletionFence), NeutralIoOwnerMaterialized(neutralIoOwnerMaterialized) {}

	std::shared_ptr<SecureIoDomainDescriptor> SecureIoDomainDescriptor::getDisabled()
	{
		return Disabled;
	}

	SecureIoDmaPolicy SecureIoDomainDescriptor::getDmaPolicy() const
	{
		return DmaPolicy;
	}

	const std::vector<std::shared_ptr<SecureSharedBufferDescriptor>> SecureIoDomainDescriptor::getSharedBuffers() const
	{
		return SharedBuffers;
	}

	bool SecureIoDomainDescriptor::getRequireCompletionFence() const
	{
		return RequireCompletionFence;
	}

	bool SecureIoDomainDescriptor::getNeutralIoOwnerMaterialized() const
	{
		return NeutralIoOwnerMaterialized;
	}

	bool SecureIoDomainDescriptor::getAllowsDma() const
	{
		return getDmaPolicy() == SecureIoDmaPolicy::ExplicitSharedBuffersOnly && getSharedBuffers().size() != 0;
	}

	bool SecureIoDomainDescriptor::TryFindCurrentSharedBuffer(unsigned long long bufferId, unsigned long long ownerDomainTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, std::shared_ptr<SecureSharedBufferDescriptor> &buffer)
	{
		if (getDmaPolicy() != SecureIoDmaPolicy::ExplicitSharedBuffersOnly || ownerDomainTag == 0 || (!policyEpoch || !policyEpoch->isMaterialized()))
		{
			buffer = nullptr;
			return false;
		}

		for (auto candidate : getSharedBuffers())
		{
			if (candidate->BufferId == bufferId && candidate->isMaterialized() && candidate->IsOwnedBy(ownerDomainTag) && candidate->HasCurrentLifetime(*policyEpoch) && candidate->Grant.MatchesEpoch(*policyEpoch))
			{
				buffer = candidate;
				return true;
			}
		}

		buffer = nullptr;
		return false;
	}

	bool SecureIoDomainDescriptor::TryFindSharedBuffer(unsigned long long address, unsigned long long length, SecureSharedBufferDirection requiredDirection, unsigned long long ownerDomainTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, std::shared_ptr<SecureSharedBufferDescriptor> &buffer)
	{
		if (getDmaPolicy() != SecureIoDmaPolicy::ExplicitSharedBuffersOnly || !policyEpoch)
		{
			buffer = nullptr;
			return false;
		}

		for (auto candidate : getSharedBuffers())
		{
			if (candidate->isMaterialized() && candidate->Contains(address, length) && candidate->AllowsDirection(requiredDirection) && candidate->IsOwnedBy(ownerDomainTag) && candidate->HasCurrentLifetime(*policyEpoch))
			{
				buffer = candidate;
				return true;
			}
		}

		buffer = nullptr;
		return false;
	}
}
