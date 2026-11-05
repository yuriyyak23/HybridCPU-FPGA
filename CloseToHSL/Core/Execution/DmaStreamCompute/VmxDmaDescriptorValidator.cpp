#include "VmxDmaDescriptorValidator.h"
#include "../../Runtime/IO/Dma/DmaTranslationResult.h"
#include "../../Runtime/IO/Dma/DmaDomainBinding.h"
#include "../../Pipeline/Certificates/StructuralIdentity/HardwareHash.h"
#include "../../Virtualization/Compatibility/Adapters/IO/YAKSys_Hybrid_CPU.Memory.IOMMU.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{

	std::shared_ptr<VmxDmaDescriptorValidationResult> VmxDmaDescriptorValidator::ValidateAndMaterialize(std::shared_ptr<DmaStreamComputeDescriptor> guestDescriptor, std::shared_ptr<IoVirtualizationBlock> ioVirt, std::shared_ptr<Lane6StateBlock> lane6State, unsigned short ioDomainTag, unsigned short addressSpaceTag, unsigned long long guestQueueId)
	{
		ArgumentNullException::ThrowIfNull(guestDescriptor);
		ArgumentNullException::ThrowIfNull(ioVirt);
		ArgumentNullException::ThrowIfNull(lane6State);

		if (ioDomainTag == 0)
		{
			return VmxDmaDescriptorValidationResult::Fail(DmaFault::Abort(DmaFaultKind::MissingDomainBinding, guestDescriptor->DescriptorReference.DescriptorAddress, false, L"DMA compatibility descriptor validation requires a non-zero I/O-domain tag."));
		}

		std::shared_ptr<IommuDomainBinding> binding;
		if (!ioVirt->TryResolveDescriptorBinding(ioDomainTag, guestDescriptor->OwnerBinding, binding))
		{
			return VmxDmaDescriptorValidationResult::Fail(DmaFault::Abort(DmaFaultKind::MissingDomainBinding, guestDescriptor->DescriptorReference.DescriptorAddress, false, L"Guest DMA descriptor has no I/O-domain tag/domain/device IOMMU binding."));
		}

		std::shared_ptr<Lane6QueueBinding> queue;
		std::shared_ptr<DmaFault> queueFault;
		if (!lane6State->TryEnsureQueue(binding, guestDescriptor->OwnerBinding->OwnerVirtualThreadId, guestQueueId, queue, queueFault))
		{
			return VmxDmaDescriptorValidationResult::Fail(queueFault ? *queueFault : DmaFault{});
		}

		if (binding->NonCoherent && binding->RequiresFence && !lane6State->HasObservedRequiredFence(binding))
		{
			return VmxDmaDescriptorValidationResult::Fail(DmaFault::Abort(DmaFaultKind::NonCoherentFenceRequired, guestDescriptor->DescriptorReference.DescriptorAddress, false, L"Non-coherent DMA domains require an observed Lane6 fence before descriptor materialization."));
		}

		std::vector<DmaStreamComputeMemoryRange> materializedReadRanges;
		DmaFault readFault;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryTranslateRanges(binding, guestDescriptor.NormalizedReadMemoryRanges, IOMMUAccessPermissions.Read, out IReadOnlyList<DmaStreamComputeMemoryRange> materializedReadRanges, out DmaFault readFault))
		if (!TryTranslateRanges(binding, guestDescriptor->NormalizedReadMemoryRanges, IOMMUAccessPermissions::Read, materializedReadRanges, readFault))
		{
			return VmxDmaDescriptorValidationResult::Fail(std::move(readFault));
		}

		std::vector<DmaStreamComputeMemoryRange> materializedWriteRanges;
		DmaFault writeFault;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryTranslateRanges(binding, guestDescriptor.NormalizedWriteMemoryRanges, IOMMUAccessPermissions.Write, out IReadOnlyList<DmaStreamComputeMemoryRange> materializedWriteRanges, out DmaFault writeFault))
		if (!TryTranslateRanges(binding, guestDescriptor->NormalizedWriteMemoryRanges, IOMMUAccessPermissions::Write, materializedWriteRanges, writeFault))
		{
			return VmxDmaDescriptorValidationResult::Fail(std::move(writeFault));
		}

		unsigned long long fenceEpoch = lane6State->getQueueRuntime()->GetFenceEpoch(binding);
		const auto evidence = std::make_shared<VmxDmaDescriptorValidationEvidence>(binding->IoDomainTag, addressSpaceTag, binding->DomainId, binding->DomainTag, binding->DeviceId, binding->DomainEpoch, IOMMU::getIotlbMappingEpoch(), queue->QueueEpoch, fenceEpoch, ComputeValidationHash(guestDescriptor, binding, queue, fenceEpoch));

		auto materialized = std::make_shared<DmaStreamComputeDescriptor>(*guestDescriptor);
		materialized->ReadMemoryRanges = materializedReadRanges;
		materialized->NormalizedReadMemoryRanges = materializedReadRanges;
		materialized->WriteMemoryRanges = materializedWriteRanges;
		materialized->NormalizedWriteMemoryRanges = materializedWriteRanges;
		materialized->VmxValidationEvidence = evidence;

		return VmxDmaDescriptorValidationResult::Valid(std::move(materialized), *evidence, queue);
	}

	bool VmxDmaDescriptorValidator::IsValidationCurrent(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<IoVirtualizationBlock> ioVirt, DmaFault &fault)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(ioVirt);
		fault = DmaFault{};
		const auto &evidence = descriptor->VmxValidationEvidence;
		if (!evidence || !evidence->IsComplete())
		{
			fault = DmaFault::Abort(DmaFaultKind::ValidationEvidenceMissing, descriptor->DescriptorReference.DescriptorAddress, false, L"VMX guest Lane6 execution requires descriptor validation evidence before MicroOp execution.");
			return false;
		}

		std::shared_ptr<IommuDomainBinding> binding;
		if (!ioVirt->TryResolveBinding(evidence->IoDomainTag, evidence->DomainTag, evidence->DeviceId, binding) || binding->DomainId != evidence->DomainId || binding->DomainEpoch != evidence->DomainEpoch || IOMMU::getIotlbMappingEpoch() != evidence->IotlbMappingEpoch)
		{
			fault = DmaFault::Abort(DmaFaultKind::ValidationEvidenceStale, descriptor->DescriptorReference.DescriptorAddress, false, L"VMX guest Lane6 descriptor validation evidence is stale after IOMMU/domain/IOTLB epoch change.");
			return false;
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryTranslateRanges(IommuDomainBinding binding, IReadOnlyList<DmaStreamComputeMemoryRange> ranges, IOMMUAccessPermissions permissions, out IReadOnlyList<DmaStreamComputeMemoryRange> materializedRanges, out DmaFault fault)
	bool VmxDmaDescriptorValidator::TryTranslateRanges(std::shared_ptr<IommuDomainBinding> binding, const std::vector<DmaStreamComputeMemoryRange> &ranges, IOMMUAccessPermissions permissions, std::vector<DmaStreamComputeMemoryRange> &materializedRanges, DmaFault &fault)
	{
		fault = DmaFault{};
		if (ranges.empty())
		{
			materializedRanges.clear();
			return true;
		}

		std::vector<DmaStreamComputeMemoryRange> materialized;
		materialized.reserve(ranges.size());
		for (const auto &range : ranges)
		{
			if (range.Length == 0 || range.Address > std::numeric_limits<unsigned long long>::max() - (range.Length - 1))
			{
				materializedRanges.clear();
				fault = DmaFault::Abort(DmaFaultKind::DescriptorFault, range.Address, (static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0, L"VMX DMA descriptor contains an empty or overflowing memory range.");
				return false;
			}

			unsigned long long hostPhysicalAddress;
			if (!TryTranslateCompleteRange(binding, range, permissions, hostPhysicalAddress, fault))
			{
				materializedRanges.clear();
				return false;
			}

			materialized.emplace_back(DmaStreamComputeMemoryRange{hostPhysicalAddress, range.Length});
		}

		materializedRanges = std::move(materialized);
		return true;
	}

	bool VmxDmaDescriptorValidator::TryTranslateCompleteRange(std::shared_ptr<IommuDomainBinding> binding, const DmaStreamComputeMemoryRange &range, IOMMUAccessPermissions permissions, unsigned long long &firstHostPhysicalAddress, DmaFault &fault)
	{
		firstHostPhysicalAddress = 0;
		fault = DmaFault{};
		unsigned long long remaining = range.Length;
		unsigned long long current = range.Address;
		unsigned long long expectedHost = 0;
		bool first = true;

		while (remaining > 0)
		{
			unsigned long long pageOffset = current & 0xFFFULL;
			unsigned long long chunkSize = std::min(4096ULL - pageOffset, remaining);
			std::shared_ptr<DmaTranslationResult> translation;
			if (!IOMMU::TryTranslateDma(binding, current, chunkSize, permissions, translation))
			{
				fault = translation->Fault;
				return false;
			}

			if (first)
			{
				firstHostPhysicalAddress = translation->HostPhysicalAddress;
				expectedHost = translation->HostPhysicalAddress + chunkSize;
				first = false;
			}
			else if (translation->HostPhysicalAddress != expectedHost)
			{
				fault = DmaFault::Abort(DmaFaultKind::IommuTranslationFault, current, (static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0, L"VMX DMA descriptor range spans non-contiguous host physical pages.");
				return false;
			}
			else
			{
				expectedHost += chunkSize;
			}

			current += chunkSize;
			remaining -= chunkSize;
		}

		return true;
	}

	unsigned long long VmxDmaDescriptorValidator::ComputeValidationHash(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<IommuDomainBinding> binding, std::shared_ptr<Lane6QueueBinding> queue, unsigned long long fenceEpoch)
	{
		HardwareHash hasher = HardwareHash();
		hasher.Initialize();
		hasher.Compress(descriptor->DescriptorIdentityHash);
		hasher.Compress(descriptor->NormalizedFootprintHash);
		hasher.Compress(binding->IoDomainTag);
		hasher.Compress(binding->DomainId);
		hasher.Compress(binding->DomainTag);
		hasher.Compress(binding->DeviceId);
		hasher.Compress(binding->DomainEpoch);
		hasher.Compress(IOMMU::getIotlbMappingEpoch());
		hasher.Compress(queue->VirtualQueueId);
		hasher.Compress(queue->QueueEpoch);
		hasher.Compress(fenceEpoch);
		unsigned long long hash = hasher.Finalize();
		return hash == 0 ? 0xD5C0'5EEDULL : hash;
	}
}
