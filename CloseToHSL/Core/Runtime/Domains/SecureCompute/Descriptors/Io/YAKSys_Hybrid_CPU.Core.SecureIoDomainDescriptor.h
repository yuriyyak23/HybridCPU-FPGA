#pragma once

#include "SecureIoDomainDescriptor.h"
#include "../../Authority/SecureRevocationEpoch.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureIoDomainDescriptor final : public std::enable_shared_from_this<SecureIoDomainDescriptor>
	{
	private:
		static inline const std::shared_ptr<SecureIoDomainDescriptor> Disabled = std::make_shared<SecureIoDomainDescriptor>();
		const SecureIoDmaPolicy DmaPolicy;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<SecureSharedBufferDescriptor> SharedBuffers;
		const std::vector<std::shared_ptr<SecureSharedBufferDescriptor>> SharedBuffers;
		const bool RequireCompletionFence;
		const bool NeutralIoOwnerMaterialized;

	public:
		SecureIoDomainDescriptor();

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, IReadOnlyList<SecureSharedBufferDescriptor> sharedBuffers, bool requireCompletionFence) : this(dmaPolicy, sharedBuffers, requireCompletionFence, neutralIoOwnerMaterialized: false)
		SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, const std::vector<std::shared_ptr<SecureSharedBufferDescriptor>> &sharedBuffers, bool requireCompletionFence);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, IReadOnlyList<SecureSharedBufferDescriptor> sharedBuffers, bool requireCompletionFence, bool neutralIoOwnerMaterialized)
		SecureIoDomainDescriptor(SecureIoDmaPolicy dmaPolicy, const std::vector<std::shared_ptr<SecureSharedBufferDescriptor>> &sharedBuffers, bool requireCompletionFence, bool neutralIoOwnerMaterialized);

		static std::shared_ptr<SecureIoDomainDescriptor> getDisabled();

		SecureIoDmaPolicy getDmaPolicy() const;

		const std::vector<std::shared_ptr<SecureSharedBufferDescriptor>> getSharedBuffers() const;

		bool getRequireCompletionFence() const;

		bool getNeutralIoOwnerMaterialized() const;

		bool getAllowsDma() const;

		bool TryFindCurrentSharedBuffer(unsigned long long bufferId, unsigned long long ownerDomainTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, std::shared_ptr<SecureSharedBufferDescriptor> &buffer);

		bool TryFindSharedBuffer(unsigned long long address, unsigned long long length, SecureSharedBufferDirection requiredDirection, unsigned long long ownerDomainTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, std::shared_ptr<SecureSharedBufferDescriptor> &buffer);
	};
}
