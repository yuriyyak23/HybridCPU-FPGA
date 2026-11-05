#include "Lane6QueueRuntime.h"
#include "../../IO/Dma/DmaDomainBinding.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	Lane6QueueRuntime::Lane6QueueRuntime(int queueCapacity) : Lane6QueueRuntime(queueCapacity, 0, 0)
	{
	}

	Lane6QueueRuntime::Lane6QueueRuntime(int queueCapacity, unsigned long long queueEpoch, unsigned long long fenceEpoch) : HostEvidence(std::make_shared<Lane6HostOwnedEvidenceStore>()), _queueCapacity(queueCapacity < 0 ? 0 : queueCapacity)
	{
		_queueEpoch = queueEpoch;
		_fenceEpoch = fenceEpoch;
		
	}

	std::shared_ptr<Lane6HostOwnedEvidenceStore> Lane6QueueRuntime::getHostEvidence() const
	{
		return HostEvidence;
	}

	unsigned long long Lane6QueueRuntime::getQueueEpoch() const
	{
		return _queueEpoch;
	}

	unsigned long long Lane6QueueRuntime::getFenceEpoch() const
	{
		return _fenceEpoch;
	}

	int Lane6QueueRuntime::getActiveQueueCount() const
	{
		return _queues.size();
	}

	bool Lane6QueueRuntime::TryEnsureQueue(std::shared_ptr<IommuDomainBinding> binding, unsigned short ownerVirtualThreadId, unsigned long long guestQueueId, std::shared_ptr<Lane6QueueBinding> &queue, std::shared_ptr<DmaFault> &fault)
	{
		queue = nullptr;
		fault = std::make_shared<DmaFault>();

		if (!binding || !binding->isValid())
		{
			fault = std::make_shared<DmaFault>(DmaFault::Abort(DmaFaultKind::QueueOwnershipFault, 0, false, L"Lane6 queue runtime requires I/O-domain tag/domain/device ownership before queue binding."));
			return false;
		}

auto key = std::make_tuple(binding->IoDomainTag, binding->DomainId, binding->DeviceId, ownerVirtualThreadId);
		auto existing = _queues.find(key);
		if (existing != _queues.end()) { queue = existing->second; return true; }

		if (_queues.size() >= _queueCapacity)
		{
			fault = std::make_shared<DmaFault>(DmaFault::Replay(DmaFaultKind::Lane6QueuePressure, 0, false, L"Lane6 virtual queue pressure is transient and must replay, not abort the guest descriptor."));
			return false;
		}

		unsigned long long queueEpoch;
		if (!TryAdvanceEpoch(_queueEpoch, queueEpoch))
		{
			fault = EpochExhaustedFault(L"Lane6 queue epoch");
			return false;
		}

		unsigned long long virtualQueueId;
		if (!TryAllocateIdentity(_nextVirtualQueueId, virtualQueueId))
		{
			fault = IdentityExhaustedFault(L"Lane6 virtual queue identity");
			return false;
		}

		queue = std::make_shared<Lane6QueueBinding>(binding->IoDomainTag, ownerVirtualThreadId, binding->DomainId, binding->DomainTag, binding->DeviceId, guestQueueId == 0 ? BuildDefaultGuestQueueId(binding, ownerVirtualThreadId) : guestQueueId, virtualQueueId, queueEpoch);
		_queues[key] = queue;
		return true;
	}

	bool Lane6QueueRuntime::TryObserveFence(std::shared_ptr<IommuDomainBinding> binding, unsigned long long guestFenceId, std::shared_ptr<DmaFault> &fault)
	{
		(void)guestFenceId;
		fault = std::make_shared<DmaFault>();
		if (!binding || !binding->isValid())
		{
			fault = std::make_shared<DmaFault>(DmaFault::Abort(DmaFaultKind::QueueOwnershipFault, 0, false, L"Lane6 fence observation requires a valid I/O-domain binding."));
			return false;
		}

		unsigned long long epoch;
		if (!TryAdvanceEpoch(_fenceEpoch, epoch))
		{
			fault = EpochExhaustedFault(L"Lane6 fence epoch");
			return false;
		}

		_fenceEpochs[std::make_tuple(binding->IoDomainTag, binding->DomainId, binding->DeviceId)] = epoch;
		return true;
	}

	unsigned long long Lane6QueueRuntime::GetFenceEpoch(std::shared_ptr<IommuDomainBinding> binding)
	{
		if (!binding) return 0;
		auto it = _fenceEpochs.find(std::make_tuple(binding->IoDomainTag, binding->DomainId, binding->DeviceId));
		return it == _fenceEpochs.end() ? 0 : it->second;
	}

	bool Lane6QueueRuntime::TryMapToken(std::shared_ptr<Lane6QueueBinding> queue, unsigned long long guestTokenId, unsigned long long guestFenceId, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle, std::shared_ptr<Lane6VirtualToken> &virtualToken, std::shared_ptr<DmaFault> &fault)
	{
		virtualToken = nullptr;
		fault = std::make_shared<DmaFault>();

		if (!queue || !queue->IsValid() || guestTokenId == 0 || !hostHandle || hostHandle->IsDefault())
		{
			fault = std::make_shared<DmaFault>(DmaFault::Abort(DmaFaultKind::QueueOwnershipFault, 0, false, L"Lane6 token binding requires a valid queue, guest token, and host-owned token handle."));
			return false;
		}

		unsigned long long virtualTokenId;
		if (!TryAllocateIdentity(_nextVirtualTokenId, virtualTokenId))
		{
			fault = IdentityExhaustedFault(L"Lane6 virtual token identity");
			return false;
		}

		auto it = _fenceEpochs.find(std::make_tuple(queue->IoDomainTag, queue->DomainId, queue->DeviceId));
		unsigned long long fenceEpoch = it == _fenceEpochs.end() ? 0 : it->second;
		virtualToken = std::make_shared<Lane6VirtualToken>(queue->IoDomainTag, queue->OwnerVirtualThreadId, queue->DomainId, queue->DomainTag, queue->DeviceId, guestTokenId, virtualTokenId, guestFenceId, queue->QueueEpoch, fenceEpoch);

		if (!getHostEvidence()->TryBind(virtualToken, hostHandle))
		{
			virtualToken = nullptr;
			fault = std::make_shared<DmaFault>(DmaFault::Abort(DmaFaultKind::QueueOwnershipFault, 0, false, L"Lane6 host evidence store denied native token binding."));
			return false;
		}

		return true;
	}

	std::shared_ptr<Lane6HostEvidenceRestoreResult> Lane6QueueRuntime::PrepareHostEvidenceForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		return getHostEvidence()->PrepareForRestore(evidencePolicy);
	}

	bool Lane6QueueRuntime::TryAdvanceEpoch(unsigned long long &epoch, unsigned long long &advancedEpoch)
	{
		if (epoch == std::numeric_limits<unsigned long long>::max())
		{
			advancedEpoch = 0;
			return false;
		}

		epoch++;
		advancedEpoch = epoch;
		return true;
	}

	bool Lane6QueueRuntime::TryAllocateIdentity(unsigned long long &nextIdentity, unsigned long long &identity)
	{
		if (nextIdentity == 0 || nextIdentity == std::numeric_limits<unsigned long long>::max())
		{
			identity = 0;
			return false;
		}

		identity = nextIdentity++;
		return true;
	}

	std::shared_ptr<DmaFault> Lane6QueueRuntime::EpochExhaustedFault(const std::wstring &identity)
	{
		return std::make_shared<DmaFault>(DmaFault::Abort(DmaFaultKind::EpochExhausted, 0, false, identity + L" exhausted; runtime refuses wraparound and requires a new domain binding."));
	}

	std::shared_ptr<DmaFault> Lane6QueueRuntime::IdentityExhaustedFault(const std::wstring &identity)
	{
		return std::make_shared<DmaFault>(DmaFault::Abort(DmaFaultKind::ResourceIdentityExhausted, 0, false, identity + L" exhausted; runtime refuses identity reuse."));
	}

	unsigned long long Lane6QueueRuntime::BuildDefaultGuestQueueId(std::shared_ptr<IommuDomainBinding> binding, unsigned short ownerVirtualThreadId)
	{
		return (static_cast<unsigned long long>(binding->IoDomainTag) << 48) | (static_cast<unsigned long long>(binding->DeviceId) << 32) | (static_cast<unsigned long long>(ownerVirtualThreadId) << 16) | binding->DomainId;
	}
}
