#include "Lane6StateBlock.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	Lane6StateBlock::Lane6StateBlock() : Lane6StateBlock(64)
	{
	}

	Lane6StateBlock::Lane6StateBlock(int queueCapacity) : QueueRuntime(std::make_shared<Lane6QueueRuntime>(queueCapacity))
	{
	}

	std::shared_ptr<Lane6QueueRuntime> Lane6StateBlock::getQueueRuntime() const
	{
		return QueueRuntime;
	}

	std::shared_ptr<Lane6HostOwnedEvidenceStore> Lane6StateBlock::getHostEvidence() const
	{
		return getQueueRuntime()->getHostEvidence();
	}

	unsigned long long Lane6StateBlock::getQueueEpoch() const
	{
		return getQueueRuntime()->getQueueEpoch();
	}

	unsigned long long Lane6StateBlock::getFenceEpoch() const
	{
		return getQueueRuntime()->getFenceEpoch();
	}

	int Lane6StateBlock::getActiveQueueCount() const
	{
		return getQueueRuntime()->getActiveQueueCount();
	}

	bool Lane6StateBlock::TryEnsureQueue(std::shared_ptr<IommuDomainBinding> binding, unsigned short ownerVirtualThreadId, unsigned long long guestQueueId, std::shared_ptr<Lane6QueueBinding> &queue, std::shared_ptr<DmaFault> &fault)
	{
		return getQueueRuntime()->TryEnsureQueue(binding, ownerVirtualThreadId, guestQueueId, queue, fault);
	}

	bool Lane6StateBlock::TryObserveFence(std::shared_ptr<IommuDomainBinding> binding, unsigned long long guestFenceId, std::shared_ptr<DmaFault> &fault)
	{
		return getQueueRuntime()->TryObserveFence(binding, guestFenceId, fault);
	}

	bool Lane6StateBlock::HasObservedRequiredFence(std::shared_ptr<IommuDomainBinding> binding)
	{
		return getQueueRuntime()->GetFenceEpoch(binding) != 0;
	}

	bool Lane6StateBlock::TryMapToken(std::shared_ptr<Lane6QueueBinding> queue, unsigned long long guestTokenId, unsigned long long guestFenceId, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle, std::shared_ptr<Lane6VirtualToken> &virtualToken, std::shared_ptr<DmaFault> &fault)
	{
		return getQueueRuntime()->TryMapToken(queue, guestTokenId, guestFenceId, hostHandle, virtualToken, fault);
	}

	bool Lane6StateBlock::TryResolveHostToken(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> &hostHandle)
	{
		return getHostEvidence()->TryResolve(virtualToken, hostHandle);
	}

	std::shared_ptr<Lane6HostEvidenceRestoreResult> Lane6StateBlock::PrepareHostEvidenceForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		return getQueueRuntime()->PrepareHostEvidenceForRestore(evidencePolicy);
	}

	std::shared_ptr<Lane6HostEvidenceRestoreResult> Lane6StateBlock::RebuildHostEvidenceAfterRestore(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle)
	{
		return getHostEvidence()->RebuildAfterRestore(virtualToken, hostHandle);
	}
}
