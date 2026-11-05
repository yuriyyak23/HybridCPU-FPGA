#pragma once

#include "HostOwnedEvidence/Lane6HostOwnedEvidenceStore.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <string>
#include <map>
#include <tuple>
#include "../../IO/Dma/DmaDomainBinding.h"
#include <limits>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
    struct Lane6QueueBinding {
        unsigned short IoDomainTag = 0, OwnerVirtualThreadId = 0;
        unsigned int DomainId = 0;
        unsigned long long DomainTag = 0;
        unsigned int DeviceId = 0;
        unsigned long long GuestQueueId = 0, VirtualQueueId = 0, QueueEpoch = 0;
        Lane6QueueBinding(unsigned short io, unsigned short owner, unsigned int domain, unsigned long long tag, unsigned int device, unsigned long long guest, unsigned long long queue, unsigned long long epoch)
            : IoDomainTag(io), OwnerVirtualThreadId(owner), DomainId(domain), DomainTag(tag), DeviceId(device), GuestQueueId(guest), VirtualQueueId(queue), QueueEpoch(epoch) {}
        bool IsValid() const { return IoDomainTag != 0 && DomainId != 0 && DomainTag != 0 && DeviceId != 0 && VirtualQueueId != 0 && QueueEpoch != 0; }
    };

	class Lane6QueueRuntime final : public std::enable_shared_from_this<Lane6QueueRuntime>
	{
	private:
		const std::shared_ptr<Lane6HostOwnedEvidenceStore> HostEvidence;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<(ushort IoDomainTag, uint DomainId, uint DeviceId, ushort VtId), Lane6QueueBinding> _queues = new();
		std::map<std::tuple<unsigned short, unsigned int, unsigned int, unsigned short>, std::shared_ptr<Lane6QueueBinding>> _queues;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<(ushort IoDomainTag, uint DomainId, uint DeviceId), ulong> _fenceEpochs = new();
		std::map<std::tuple<unsigned short, unsigned int, unsigned int>, unsigned long long> _fenceEpochs;
		const int _queueCapacity;
		unsigned long long _queueEpoch = 0;
		unsigned long long _fenceEpoch = 0;
		unsigned long long _nextVirtualQueueId = 0x1'0000ULL;
		unsigned long long _nextVirtualTokenId = 0x1'0000'0000ULL;

	public:
		Lane6QueueRuntime(int queueCapacity = 64);

		Lane6QueueRuntime(int queueCapacity, unsigned long long queueEpoch, unsigned long long fenceEpoch);

		std::shared_ptr<Lane6HostOwnedEvidenceStore> getHostEvidence() const;

		unsigned long long getQueueEpoch() const;

		unsigned long long getFenceEpoch() const;

		int getActiveQueueCount() const;

		bool TryEnsureQueue(std::shared_ptr<IommuDomainBinding> binding, unsigned short ownerVirtualThreadId, unsigned long long guestQueueId, std::shared_ptr<Lane6QueueBinding> &queue, std::shared_ptr<DmaFault> &fault);

		bool TryObserveFence(std::shared_ptr<IommuDomainBinding> binding, unsigned long long guestFenceId, std::shared_ptr<DmaFault> &fault);

		unsigned long long GetFenceEpoch(std::shared_ptr<IommuDomainBinding> binding);

		bool TryMapToken(std::shared_ptr<Lane6QueueBinding> queue, unsigned long long guestTokenId, unsigned long long guestFenceId, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle, std::shared_ptr<Lane6VirtualToken> &virtualToken, std::shared_ptr<DmaFault> &fault);

		std::shared_ptr<Lane6HostEvidenceRestoreResult> PrepareHostEvidenceForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

	private:
		static bool TryAdvanceEpoch(unsigned long long &epoch, unsigned long long &advancedEpoch);

		static bool TryAllocateIdentity(unsigned long long &nextIdentity, unsigned long long &identity);

		static std::shared_ptr<DmaFault> EpochExhaustedFault(const std::wstring &identity);

		static std::shared_ptr<DmaFault> IdentityExhaustedFault(const std::wstring &identity);

		static unsigned long long BuildDefaultGuestQueueId(std::shared_ptr<IommuDomainBinding> binding, unsigned short ownerVirtualThreadId);
	};

}
