#pragma once

#include "../../Runtime/Domains/Descriptors/IoDomain/IoVirtualizationBlock.h"
#include "../../Runtime/Lanes/Lane6/Lane6StateBlock.h"
#include "../../Runtime/IO/Dma/DmaDomainBinding.h"
#include "DmaStreamComputeDescriptor.h"
#include "../../../Memory/MMU/IOMMU.h"
#include <algorithm>
#include <cwctype>
#include <vector>
#include <cmath>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <utility>

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxDmaDescriptorValidationEvidence(ushort IoDomainTag, ushort AddressSpaceTag, uint DomainId, ulong DomainTag, uint DeviceId, ulong DomainEpoch, ulong IotlbMappingEpoch, ulong QueueEpoch, ulong FenceEpoch, ulong ValidationHash)
//	{
//		public bool IsComplete
//		{
//			get
//			{
//				return IoDomainTag != 0 && DomainId != 0 && DomainTag != 0 && DeviceId != 0 && DomainEpoch != 0 && IotlbMappingEpoch != 0 && QueueEpoch != 0 && ValidationHash != 0;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record VmxDmaDescriptorValidationResult
//	{
//		private VmxDmaDescriptorValidationResult(bool isValid, System.Nullable<DmaStreamComputeDescriptor> materializedDescriptor, VmxDmaDescriptorValidationEvidence evidence, Lane6QueueBinding queue, DmaFault fault, string message)
//		{
//			IsValid = isValid;
//			MaterializedDescriptor = materializedDescriptor;
//			Evidence = evidence;
//			Queue = queue;
//			Fault = fault;
//			Message = message;
//		}
//
//		public bool IsValid
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeDescriptor> MaterializedDescriptor
//		{
//			get;
//		}
//
//		public VmxDmaDescriptorValidationEvidence Evidence
//		{
//			get;
//		}
//
//		public Lane6QueueBinding Queue
//		{
//			get;
//		}
//
//		public DmaFault Fault
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool IsAbort
//		{
//			get
//			{
//				return Fault.Disposition == DmaFaultDisposition.Abort;
//			}
//		}
//
//		public bool IsReplay
//		{
//			get
//			{
//				return Fault.Disposition == DmaFaultDisposition.Replay;
//			}
//		}
//
//		public static VmxDmaDescriptorValidationResult Valid(DmaStreamComputeDescriptor materializedDescriptor, VmxDmaDescriptorValidationEvidence evidence, Lane6QueueBinding queue)
//		{
//			return new(true, materializedDescriptor, evidence, queue, DmaFault.None, "VMX DMA descriptor accepted and materialized into host-owned Lane6 evidence.");
//		}
//
//		public static VmxDmaDescriptorValidationResult Fail(DmaFault fault, System.Nullable<string> message = nullptr)
//		{
//			return new(false, nullptr, default, default, fault, string.IsNullOrWhiteSpace(message) ? fault.Message : message);
//		}
//	}

	class VmxDmaDescriptorValidationEvidence final
	{
	public:
		const unsigned short IoDomainTag = 0;
		const unsigned short AddressSpaceTag = 0;
		const unsigned int DomainId = 0;
		const unsigned long long DomainTag = 0;
		const unsigned int DeviceId = 0;
		const unsigned long long DomainEpoch = 0;
		const unsigned long long IotlbMappingEpoch = 0;
		const unsigned long long QueueEpoch = 0;
		const unsigned long long FenceEpoch = 0;
		const unsigned long long ValidationHash = 0;

		VmxDmaDescriptorValidationEvidence() = default;
		VmxDmaDescriptorValidationEvidence(unsigned short ioDomainTag, unsigned short addressSpaceTag,
			unsigned int domainId, unsigned long long domainTag, unsigned int deviceId,
			unsigned long long domainEpoch, unsigned long long iotlbMappingEpoch,
			unsigned long long queueEpoch, unsigned long long fenceEpoch,
			unsigned long long validationHash)
			: IoDomainTag(ioDomainTag), AddressSpaceTag(addressSpaceTag), DomainId(domainId),
			  DomainTag(domainTag), DeviceId(deviceId), DomainEpoch(domainEpoch),
			  IotlbMappingEpoch(iotlbMappingEpoch), QueueEpoch(queueEpoch), FenceEpoch(fenceEpoch),
			  ValidationHash(validationHash) {}

		bool IsComplete() const noexcept
		{
			return IoDomainTag != 0 && DomainId != 0 && DomainTag != 0 && DeviceId != 0 &&
				DomainEpoch != 0 && IotlbMappingEpoch != 0 && QueueEpoch != 0 && ValidationHash != 0;
		}
		bool operator==(const VmxDmaDescriptorValidationEvidence&) const = default;
	};

	class VmxDmaDescriptorValidationResult final
	{
		VmxDmaDescriptorValidationResult(bool isValid,
			std::shared_ptr<DmaStreamComputeDescriptor> materializedDescriptor,
			VmxDmaDescriptorValidationEvidence evidence,
			std::shared_ptr<Lane6QueueBinding> queue, DmaFault fault, std::wstring message)
			: IsValid(isValid), MaterializedDescriptor(std::move(materializedDescriptor)), Evidence(evidence),
			  Queue(std::move(queue)), Fault(std::move(fault)), Message(std::move(message)) {}

	public:
		const bool IsValid;
		const std::shared_ptr<DmaStreamComputeDescriptor> MaterializedDescriptor;
		const VmxDmaDescriptorValidationEvidence Evidence;
		const std::shared_ptr<Lane6QueueBinding> Queue;
		const DmaFault Fault;
		const std::wstring Message;

		bool IsAbort() const noexcept { return Fault.Disposition == DmaFaultDisposition::Abort; }
		bool IsReplay() const noexcept { return Fault.Disposition == DmaFaultDisposition::Replay; }

		static std::shared_ptr<VmxDmaDescriptorValidationResult> Valid(
			std::shared_ptr<DmaStreamComputeDescriptor> materializedDescriptor,
			VmxDmaDescriptorValidationEvidence evidence, std::shared_ptr<Lane6QueueBinding> queue)
		{
			return std::shared_ptr<VmxDmaDescriptorValidationResult>(new VmxDmaDescriptorValidationResult(
				true, std::move(materializedDescriptor), evidence, std::move(queue), DmaFault{},
				L"VMX DMA descriptor accepted and materialized into host-owned Lane6 evidence."));
		}

		static std::shared_ptr<VmxDmaDescriptorValidationResult> Fail(
			DmaFault fault, std::optional<std::wstring> message = std::nullopt)
		{
			const bool hasMessage = message.has_value() &&
				std::any_of(message->begin(), message->end(), [](wchar_t ch) { return !std::iswspace(ch); });
			std::wstring finalMessage = hasMessage ? *message : fault.Message;
			return std::shared_ptr<VmxDmaDescriptorValidationResult>(new VmxDmaDescriptorValidationResult(
				false, nullptr, VmxDmaDescriptorValidationEvidence{}, nullptr, std::move(fault),
				std::move(finalMessage)));
		}
	};

	class VmxDmaDescriptorValidator final
	{
	public:
		static std::shared_ptr<VmxDmaDescriptorValidationResult> ValidateAndMaterialize(std::shared_ptr<DmaStreamComputeDescriptor> guestDescriptor, std::shared_ptr<IoVirtualizationBlock> ioVirt, std::shared_ptr<Lane6StateBlock> lane6State, unsigned short ioDomainTag, unsigned short addressSpaceTag = 0, unsigned long long guestQueueId = 0);

		static bool IsValidationCurrent(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<IoVirtualizationBlock> ioVirt, DmaFault &fault);

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryTranslateRanges(IommuDomainBinding binding, IReadOnlyList<DmaStreamComputeMemoryRange> ranges, IOMMUAccessPermissions permissions, out IReadOnlyList<DmaStreamComputeMemoryRange> materializedRanges, out DmaFault fault)
		static bool TryTranslateRanges(std::shared_ptr<IommuDomainBinding> binding, const std::vector<DmaStreamComputeMemoryRange> &ranges, IOMMUAccessPermissions permissions, std::vector<DmaStreamComputeMemoryRange> &materializedRanges, DmaFault &fault);

		static bool TryTranslateCompleteRange(std::shared_ptr<IommuDomainBinding> binding, const DmaStreamComputeMemoryRange &range, IOMMUAccessPermissions permissions, unsigned long long &firstHostPhysicalAddress, DmaFault &fault);

		static unsigned long long ComputeValidationHash(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<IommuDomainBinding> binding, std::shared_ptr<Lane6QueueBinding> queue, unsigned long long fenceEpoch);
	};
}
