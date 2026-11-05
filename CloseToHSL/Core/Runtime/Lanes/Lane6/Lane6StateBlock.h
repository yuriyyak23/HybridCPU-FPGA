#pragma once

#include "Lane6QueueRuntime.h"
#include "HostOwnedEvidence/Lane6HostOwnedEvidenceStore.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane6QueueBinding(ushort IoDomainTag, ushort OwnerVirtualThreadId, uint DomainId, ulong DomainTag, uint DeviceId, ulong GuestQueueId, ulong VirtualQueueId, ulong QueueEpoch)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return IoDomainTag != 0 && DomainId != 0 && DomainTag != 0 && DeviceId != 0 && VirtualQueueId != 0 && QueueEpoch != 0;
//			}
//		}
//	}


	class Lane6StateBlock final : public std::enable_shared_from_this<Lane6StateBlock>
	{
	private:
		const std::shared_ptr<Lane6QueueRuntime> QueueRuntime;

	public:
		Lane6StateBlock();

		Lane6StateBlock(int queueCapacity);

		std::shared_ptr<Lane6QueueRuntime> getQueueRuntime() const;

		std::shared_ptr<Lane6HostOwnedEvidenceStore> getHostEvidence() const;

		unsigned long long getQueueEpoch() const;

		unsigned long long getFenceEpoch() const;

		int getActiveQueueCount() const;

		bool TryEnsureQueue(std::shared_ptr<IommuDomainBinding> binding, unsigned short ownerVirtualThreadId, unsigned long long guestQueueId, std::shared_ptr<Lane6QueueBinding> &queue, std::shared_ptr<DmaFault> &fault);

		bool TryObserveFence(std::shared_ptr<IommuDomainBinding> binding, unsigned long long guestFenceId, std::shared_ptr<DmaFault> &fault);

		bool HasObservedRequiredFence(std::shared_ptr<IommuDomainBinding> binding);

		bool TryMapToken(std::shared_ptr<Lane6QueueBinding> queue, unsigned long long guestTokenId, unsigned long long guestFenceId, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle, std::shared_ptr<Lane6VirtualToken> &virtualToken, std::shared_ptr<DmaFault> &fault);

		bool TryResolveHostToken(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> &hostHandle);

		std::shared_ptr<Lane6HostEvidenceRestoreResult> PrepareHostEvidenceForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

		std::shared_ptr<Lane6HostEvidenceRestoreResult> RebuildHostEvidenceAfterRestore(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle);
	};
}
