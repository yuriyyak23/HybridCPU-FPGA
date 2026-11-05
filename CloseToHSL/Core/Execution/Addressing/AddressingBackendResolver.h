#pragma once

#include "../BurstIO/IBurstBackend.h"
#include "../DmaStreamCompute/DmaStreamComputeToken.h"
#include "../DmaStreamCompute/DmaStreamComputeDescriptor.h"
#include "../ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include <string>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>
#include <vector>
#include <limits>
#include "../../../../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Execution;

namespace YAKSys_Hybrid_CPU::Core::Execution::Addressing
{

	enum class MemoryAddressSpaceKind : unsigned char
	{
		Physical = 0,
		IommuTranslated = 1,
		ReservedFuture = 2
	};

	enum class AddressingBackendAccessKind : unsigned char
	{
		Read = 0,
		Write = 1,
		ReadWrite = 2,
		Commit = 3
	};

	enum class AddressingBackendKind : unsigned char
	{
		None = 0,
		PhysicalMainMemory = 1,
		IommuBurst = 2
	};

	enum class AddressingBackendDecisionKind : unsigned char
	{
		Selected = 0,
		Rejected = 1,
		Faulted = 2
	};

	enum class AddressingBackendFaultKind : unsigned char
	{
		None = 0,
		UnsupportedAddressSpace = 1,
		CapabilityMismatch = 2,
		IncompleteRangeTruth = 3,
		OwnerDomainMismatch = 4,
		DeviceMismatch = 5,
		MissingDeviceBinding = 6,
		MissingMappingEpoch = 7,
		MappingEpochRevoked = 8,
		TranslationFault = 9,
		PermissionFault = 10,
		AlignmentFault = 11,
		BoundsFault = 12,
		DmaDeviceFault = 13,
		BackendMemoryFault = 14
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum AddressingBackendCapabilities : ushort
	enum class AddressingBackendCapabilities : unsigned short
	{
		None = 0,
		ExplicitAddressSpaceContract = 1 << 0,
		PhysicalMainMemoryBackend = 1 << 1,
		IommuBurstBackend = 1 << 2,
		OwnerDomainDeviceBinding = 1 << 3,
		MappingEpoch = 1 << 4,
		TypedFaultClassification = 1 << 5
	};

	struct AddressingBackendRange final
	{
		const unsigned long long Address;
		const unsigned long long Length;

		AddressingBackendRange(unsigned long long address = 0, unsigned long long length = 0)
			: Address(address), Length(length) {}

		bool IsWellFormed() const { return Length != 0 && Address <= std::numeric_limits<unsigned long long>::max() - Length; }
	};

	class AddressingBackendParticipantIdentity final
	{
	public:
		const unsigned short OwnerVirtualThreadId;
		const unsigned int OwnerContextId;
		const unsigned int OwnerCoreId;
		const unsigned int OwnerPodId;
		const unsigned long long MemoryDomainTag;
		const unsigned long long ActiveDomainCertificate;
		const unsigned int DeviceId;
		const bool DeviceIdBound;
		const unsigned long long MappingEpoch;
		const bool MappingEpochKnown;
		const unsigned long long TokenId;
		const unsigned long long CommandId;
		const unsigned long long DescriptorIdentityHash;
		const unsigned long long NormalizedFootprintHash;
		const unsigned long long IssueAge;
		const unsigned long long ReplayGeneration;

		AddressingBackendParticipantIdentity(unsigned short ownerVirtualThreadId, unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId, unsigned long long memoryDomainTag, unsigned long long activeDomainCertificate, unsigned int deviceId, bool deviceIdBound, unsigned long long mappingEpoch, bool mappingEpochKnown, unsigned long long tokenId, unsigned long long commandId, unsigned long long descriptorIdentityHash, unsigned long long normalizedFootprintHash, unsigned long long issueAge, unsigned long long replayGeneration)
			: OwnerVirtualThreadId(ownerVirtualThreadId), OwnerContextId(ownerContextId), OwnerCoreId(ownerCoreId), OwnerPodId(ownerPodId), MemoryDomainTag(memoryDomainTag), ActiveDomainCertificate(activeDomainCertificate), DeviceId(deviceId), DeviceIdBound(deviceIdBound), MappingEpoch(mappingEpoch), MappingEpochKnown(mappingEpochKnown), TokenId(tokenId), CommandId(commandId), DescriptorIdentityHash(descriptorIdentityHash), NormalizedFootprintHash(normalizedFootprintHash), IssueAge(issueAge), ReplayGeneration(replayGeneration) {}

		bool HasOwnerDomainAuthority() const { return IsDomainCoveredByCertificate(MemoryDomainTag, ActiveDomainCertificate); }
		bool HasOwnerDeviceAndDomainBinding() const { return DeviceIdBound && HasOwnerDomainAuthority(); }
		bool HasSameOwnerDomainAndDevice(const std::shared_ptr<const AddressingBackendParticipantIdentity>& current, AddressingBackendFaultKind& faultKind) const;
		static std::shared_ptr<const AddressingBackendParticipantIdentity> FromDmaStreamCompute(const std::shared_ptr<DmaStreamComputeDescriptor>& descriptor, unsigned long long tokenId = 0, unsigned long long issueAge = 0, unsigned long long replayGeneration = 0, unsigned long long mappingEpoch = 0, bool mappingEpochKnown = false);
		static std::shared_ptr<const AddressingBackendParticipantIdentity> FromL7Accelerator(const std::shared_ptr<AcceleratorCommandDescriptor>& descriptor, unsigned long long tokenId = 0, unsigned long long commandId = 0, unsigned long long issueAge = 0, unsigned long long replayGeneration = 0);

	private:
		static bool IsDomainCoveredByCertificate(unsigned long long memoryDomainTag, unsigned long long activeDomainCertificate)
		{
			return memoryDomainTag == 0 ? activeDomainCertificate == 0 : (activeDomainCertificate == 0 || (memoryDomainTag & activeDomainCertificate) != 0);
		}
	};

	class AddressingBackendResolutionRequest final
	{
	public:
		const MemoryAddressSpaceKind AddressSpace;
		const AddressingBackendAccessKind AccessKind;
		const std::vector<AddressingBackendRange> Ranges;
		const std::shared_ptr<const AddressingBackendParticipantIdentity> AcceptedIdentity;
		const std::shared_ptr<const AddressingBackendParticipantIdentity> CurrentAuthority;
		const AddressingBackendCapabilities Capabilities;

		AddressingBackendResolutionRequest(MemoryAddressSpaceKind addressSpace, AddressingBackendAccessKind accessKind, const std::vector<AddressingBackendRange>& ranges, std::shared_ptr<const AddressingBackendParticipantIdentity> acceptedIdentity, AddressingBackendCapabilities capabilities, std::shared_ptr<const AddressingBackendParticipantIdentity> currentAuthority = nullptr)
			: AddressSpace(addressSpace), AccessKind(accessKind), Ranges(FreezeRanges(ranges)), AcceptedIdentity(FreezeRequiredIdentity(acceptedIdentity)), CurrentAuthority(FreezeOptionalIdentity(currentAuthority)), Capabilities(capabilities) {}

		bool HasCapability(AddressingBackendCapabilities capability) const { return (static_cast<unsigned short>(Capabilities) & static_cast<unsigned short>(capability)) == static_cast<unsigned short>(capability); }
		bool HasCompleteRangeTruth() const { if (Ranges.empty()) return false; for (const auto& range : Ranges) if (!range.IsWellFormed()) return false; return true; }
		static std::shared_ptr<AddressingBackendResolutionRequest> ForDmaStreamCompute(const std::shared_ptr<DmaStreamComputeDescriptor>& descriptor, MemoryAddressSpaceKind addressSpace, AddressingBackendCapabilities capabilities, unsigned long long tokenId = 0, unsigned long long issueAge = 0, unsigned long long replayGeneration = 0, unsigned long long mappingEpoch = 0, bool mappingEpochKnown = false, std::shared_ptr<AddressingBackendParticipantIdentity> currentAuthority = nullptr);
		static std::shared_ptr<AddressingBackendResolutionRequest> ForL7Accelerator(const std::shared_ptr<AcceleratorCommandDescriptor>& descriptor, MemoryAddressSpaceKind addressSpace, AddressingBackendCapabilities capabilities, unsigned long long tokenId = 0, unsigned long long commandId = 0, unsigned long long issueAge = 0, unsigned long long replayGeneration = 0, std::shared_ptr<AddressingBackendParticipantIdentity> currentAuthority = nullptr);

	private:
		static std::vector<AddressingBackendRange> FreezeRanges(const std::vector<AddressingBackendRange>& ranges) { return ranges; }
		static std::shared_ptr<const AddressingBackendParticipantIdentity> FreezeRequiredIdentity(const std::shared_ptr<const AddressingBackendParticipantIdentity>& identity)
		{
			ArgumentNullException::ThrowIfNull(identity);
			return std::make_shared<const AddressingBackendParticipantIdentity>(*identity);
		}
		static std::shared_ptr<const AddressingBackendParticipantIdentity> FreezeOptionalIdentity(const std::shared_ptr<const AddressingBackendParticipantIdentity>& identity)
		{
			return identity ? std::make_shared<const AddressingBackendParticipantIdentity>(*identity) : nullptr;
		}
	};

	class AddressingBackendResolution final
	{
	public:
		AddressingBackendDecisionKind DecisionKind{};
		AddressingBackendKind BackendKind{};
		std::shared_ptr<IBurstBackend> Backend;
		AddressingBackendFaultKind FaultKind{};
		std::wstring Message;
		bool IsSelected() const { return DecisionKind == AddressingBackendDecisionKind::Selected; }
		bool IsRejected() const { return DecisionKind == AddressingBackendDecisionKind::Rejected; }
		bool IsFaulted() const { return DecisionKind == AddressingBackendDecisionKind::Faulted; }
		bool IsCurrentArchitecturalExecution() const { return false; }
		bool ChangesCurrentDscHelperPath() const { return false; }
		bool ChangesCurrentL7CarrierExecution() const { return false; }
		bool AllowsSilentFallbackToPhysical() const { return false; }
		static std::shared_ptr<AddressingBackendResolution> Selected(AddressingBackendKind backendKind, std::shared_ptr<IBurstBackend> backend, const std::wstring& message) { return std::shared_ptr<AddressingBackendResolution>(new AddressingBackendResolution(AddressingBackendDecisionKind::Selected, backendKind, std::move(backend), AddressingBackendFaultKind::None, message)); }
		static std::shared_ptr<AddressingBackendResolution> Rejected(AddressingBackendFaultKind faultKind, const std::wstring& message) { return std::shared_ptr<AddressingBackendResolution>(new AddressingBackendResolution(AddressingBackendDecisionKind::Rejected, AddressingBackendKind::None, nullptr, faultKind, message)); }
		static std::shared_ptr<AddressingBackendResolution> Faulted(AddressingBackendFaultKind faultKind, const std::wstring& message) { return std::shared_ptr<AddressingBackendResolution>(new AddressingBackendResolution(AddressingBackendDecisionKind::Faulted, AddressingBackendKind::None, nullptr, faultKind, message)); }
	private:
		AddressingBackendResolution(AddressingBackendDecisionKind decisionKind, AddressingBackendKind backendKind, std::shared_ptr<IBurstBackend> backend, AddressingBackendFaultKind faultKind, const std::wstring& message)
			: DecisionKind(decisionKind), BackendKind(backendKind), Backend(std::move(backend)), FaultKind(faultKind), Message(message) {}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AddressingBackendRange
//	{
//		public AddressingBackendRange(ulong address, ulong length)
//		{
//			Address = address;
//			Length = length;
//		}
//
//		public ulong Address
//		{
//			get;
//		}
//
//		public ulong Length
//		{
//			get;
//		}
//
//		public bool IsWellFormed
//		{
//			get
//			{
//				return Length != 0 && Address <= ulong.MaxValue - Length;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AddressingBackendParticipantIdentity
//	{
//		public required ushort OwnerVirtualThreadId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerContextId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerCoreId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerPodId
//		{
//			get;
//			set;
//		}
//
//		public required ulong MemoryDomainTag
//		{
//			get;
//			set;
//		}
//
//		public required ulong ActiveDomainCertificate
//		{
//			get;
//			set;
//		}
//
//		public required uint DeviceId
//		{
//			get;
//			set;
//		}
//
//		public bool DeviceIdBound
//		{
//			get;
//			set;
//		}
//
//		public ulong MappingEpoch
//		{
//			get;
//			set;
//		}
//
//		public bool MappingEpochKnown
//		{
//			get;
//			set;
//		}
//
//		public ulong TokenId
//		{
//			get;
//			set;
//		}
//
//		public ulong CommandId
//		{
//			get;
//			set;
//		}
//
//		public ulong DescriptorIdentityHash
//		{
//			get;
//			set;
//		}
//
//		public ulong NormalizedFootprintHash
//		{
//			get;
//			set;
//		}
//
//		public ulong IssueAge
//		{
//			get;
//			set;
//		}
//
//		public ulong ReplayGeneration
//		{
//			get;
//			set;
//		}
//
//		public bool HasOwnerDomainAuthority
//		{
//			get
//			{
//				return IsDomainCoveredByCertificate(MemoryDomainTag, ActiveDomainCertificate);
//			}
//		}
//
//		public bool HasOwnerDeviceAndDomainBinding
//		{
//			get
//			{
//				return DeviceIdBound && HasOwnerDomainAuthority;
//			}
//		}
//
//		public static AddressingBackendParticipantIdentity FromDmaStreamCompute(DmaStreamComputeDescriptor descriptor, ulong tokenId = 0, ulong issueAge = 0, ulong replayGeneration = 0, ulong mappingEpoch = 0, bool mappingEpochKnown = false)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			DmaStreamComputeOwnerBinding owner = descriptor.OwnerBinding;
//			DmaStreamComputeOwnerGuardContext guard = descriptor.OwnerGuardDecision.RuntimeOwnerContext;
//
//			return new AddressingBackendParticipantIdentity { OwnerVirtualThreadId = owner.OwnerVirtualThreadId, OwnerContextId = owner.OwnerContextId, OwnerCoreId = owner.OwnerCoreId, OwnerPodId = owner.OwnerPodId, MemoryDomainTag = owner.OwnerDomainTag, ActiveDomainCertificate = guard.ActiveDomainCertificate, DeviceId = owner.DeviceId, DeviceIdBound = descriptor.OwnerGuardDecision.IsAllowed && owner.DeviceId == guard.DeviceId, MappingEpoch = mappingEpoch, MappingEpochKnown = mappingEpochKnown, TokenId = tokenId, DescriptorIdentityHash = descriptor.DescriptorIdentityHash, NormalizedFootprintHash = descriptor.NormalizedFootprintHash, IssueAge = issueAge, ReplayGeneration = replayGeneration };
//		}
//
//		public static AddressingBackendParticipantIdentity FromL7Accelerator(AcceleratorCommandDescriptor descriptor, ulong tokenId = 0, ulong commandId = 0, ulong issueAge = 0, ulong replayGeneration = 0)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			AcceleratorOwnerBinding owner = descriptor.OwnerBinding;
//			return new AddressingBackendParticipantIdentity { OwnerVirtualThreadId = owner.OwnerVirtualThreadId, OwnerContextId = owner.OwnerContextId, OwnerCoreId = owner.OwnerCoreId, OwnerPodId = owner.OwnerPodId, MemoryDomainTag = owner.DomainTag, ActiveDomainCertificate = (descriptor.OwnerGuardDecision.Evidence == nullptr ? nullptr : descriptor.OwnerGuardDecision.Evidence.ActiveDomainCertificate) ?? 0, DeviceId = (uint)descriptor.AcceleratorId, DeviceIdBound = descriptor.OwnerGuardDecision.IsAllowed, MappingEpoch = descriptor.OwnerGuardDecision.MappingEpoch.Value, MappingEpochKnown = descriptor.OwnerGuardDecision.Evidence != nullptr, TokenId = tokenId, CommandId = commandId, DescriptorIdentityHash = descriptor.Identity.DescriptorIdentityHash, NormalizedFootprintHash = descriptor.Identity.NormalizedFootprintHash, IssueAge = issueAge, ReplayGeneration = replayGeneration };
//		}
//
//		internal bool HasSameOwnerDomainAndDevice(AddressingBackendParticipantIdentity current, out AddressingBackendFaultKind faultKind)
//		{
//			faultKind = AddressingBackendFaultKind.None;
//			if (OwnerVirtualThreadId != current.OwnerVirtualThreadId || OwnerContextId != current.OwnerContextId || OwnerCoreId != current.OwnerCoreId || OwnerPodId != current.OwnerPodId || MemoryDomainTag != current.MemoryDomainTag)
//			{
//				faultKind = AddressingBackendFaultKind.OwnerDomainMismatch;
//				return false;
//			}
//
//			if (DeviceId != current.DeviceId || DeviceIdBound != current.DeviceIdBound)
//			{
//				faultKind = AddressingBackendFaultKind.DeviceMismatch;
//				return false;
//			}
//
//			return true;
//		}
//
//		private static bool IsDomainCoveredByCertificate(ulong memoryDomainTag, ulong activeDomainCertificate)
//		{
//			if (memoryDomainTag == 0)
//			{
//				return activeDomainCertificate == 0;
//			}
//
//			return activeDomainCertificate == 0 || (memoryDomainTag & activeDomainCertificate) != 0;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AddressingBackendResolutionRequest
//	{
//		public AddressingBackendResolutionRequest(MemoryAddressSpaceKind addressSpace, AddressingBackendAccessKind accessKind, IReadOnlyList<AddressingBackendRange> ranges, AddressingBackendParticipantIdentity acceptedIdentity, AddressingBackendCapabilities capabilities, System.Nullable<AddressingBackendParticipantIdentity> currentAuthority = nullptr)
//		{
//			ArgumentNullException.ThrowIfNull(acceptedIdentity);
//			AddressSpace = addressSpace;
//			AccessKind = accessKind;
//			Ranges = FreezeRanges(ranges);
//			AcceptedIdentity = acceptedIdentity;
//			Capabilities = capabilities;
//			CurrentAuthority = currentAuthority;
//		}
//
//		public MemoryAddressSpaceKind AddressSpace
//		{
//			get;
//		}
//
//		public AddressingBackendAccessKind AccessKind
//		{
//			get;
//		}
//
//		public IReadOnlyList<AddressingBackendRange> Ranges
//		{
//			get;
//		}
//
//		public AddressingBackendParticipantIdentity AcceptedIdentity
//		{
//			get;
//		}
//
//		public System.Nullable<AddressingBackendParticipantIdentity> CurrentAuthority
//		{
//			get;
//		}
//
//		public AddressingBackendCapabilities Capabilities
//		{
//			get;
//		}
//
//		public bool HasCapability(AddressingBackendCapabilities capability)
//		{
//			return (Capabilities & capability) == capability;
//		}
//
//		public bool HasCompleteRangeTruth
//		{
//			get
//			{
//				if (Ranges.Count == 0)
//				{
//					return false;
//				}
//
//				for (int index = 0; index < Ranges.Count; index++)
//				{
//					if (!Ranges[index].IsWellFormed)
//					{
//						return false;
//					}
//				}
//
//				return true;
//			}
//		}
//
//		public static AddressingBackendResolutionRequest ForDmaStreamCompute(DmaStreamComputeDescriptor descriptor, MemoryAddressSpaceKind addressSpace, AddressingBackendCapabilities capabilities, ulong tokenId = 0, ulong issueAge = 0, ulong replayGeneration = 0, ulong mappingEpoch = 0, bool mappingEpochKnown = false, System.Nullable<AddressingBackendParticipantIdentity> currentAuthority = nullptr)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			return new AddressingBackendResolutionRequest(addressSpace, AddressingBackendAccessKind.ReadWrite, MergeRanges(descriptor.NormalizedReadMemoryRanges, descriptor.NormalizedWriteMemoryRanges), AddressingBackendParticipantIdentity.FromDmaStreamCompute(descriptor, tokenId, issueAge, replayGeneration, mappingEpoch: mappingEpoch, mappingEpochKnown: mappingEpochKnown), capabilities, currentAuthority);
//		}
//
//		public static AddressingBackendResolutionRequest ForL7Accelerator(AcceleratorCommandDescriptor descriptor, MemoryAddressSpaceKind addressSpace, AddressingBackendCapabilities capabilities, ulong tokenId = 0, ulong commandId = 0, ulong issueAge = 0, ulong replayGeneration = 0, System.Nullable<AddressingBackendParticipantIdentity> currentAuthority = nullptr)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			return new AddressingBackendResolutionRequest(addressSpace, AddressingBackendAccessKind.ReadWrite, MergeRanges(descriptor.NormalizedFootprint.SourceRanges, descriptor.NormalizedFootprint.DestinationRanges, descriptor.NormalizedFootprint.ScratchRanges), AddressingBackendParticipantIdentity.FromL7Accelerator(descriptor, tokenId, commandId, issueAge, replayGeneration), capabilities, currentAuthority);
//		}
//
//		private static IReadOnlyList<AddressingBackendRange> MergeRanges(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges)
//		{
//			int readCount = (readRanges == nullptr ? nullptr : readRanges.Count) ?? 0;
//			int writeCount = (writeRanges == nullptr ? nullptr : writeRanges.Count) ?? 0;
//			AddressingBackendRange[] ranges = new AddressingBackendRange[readCount + writeCount];
//			int cursor = 0;
//			for (int index = 0; index < readCount; index++)
//			{
//				ranges[cursor++] = new AddressingBackendRange(readRanges![index].Address, readRanges[index].Length);
//			}
//
//			for (int index = 0; index < writeCount; index++)
//			{
//				ranges[cursor++] = new AddressingBackendRange(writeRanges![index].Address, writeRanges[index].Length);
//			}
//
//			return ranges;
//		}
//
//		private static IReadOnlyList<AddressingBackendRange> MergeRanges(IReadOnlyList<AcceleratorMemoryRange> sourceRanges, IReadOnlyList<AcceleratorMemoryRange> destinationRanges, IReadOnlyList<AcceleratorMemoryRange> scratchRanges)
//		{
//			int sourceCount = (sourceRanges == nullptr ? nullptr : sourceRanges.Count) ?? 0;
//			int destinationCount = (destinationRanges == nullptr ? nullptr : destinationRanges.Count) ?? 0;
//			int scratchCount = (scratchRanges == nullptr ? nullptr : scratchRanges.Count) ?? 0;
//			AddressingBackendRange[] ranges = new AddressingBackendRange[sourceCount + destinationCount + scratchCount];
//			int cursor = 0;
//			for (int index = 0; index < sourceCount; index++)
//			{
//				ranges[cursor++] = new AddressingBackendRange(sourceRanges![index].Address, sourceRanges[index].Length);
//			}
//
//			for (int index = 0; index < destinationCount; index++)
//			{
//				ranges[cursor++] = new AddressingBackendRange(destinationRanges![index].Address, destinationRanges[index].Length);
//			}
//
//			for (int index = 0; index < scratchCount; index++)
//			{
//				ranges[cursor++] = new AddressingBackendRange(scratchRanges![index].Address, scratchRanges[index].Length);
//			}
//
//			return ranges;
//		}
//
//		private static IReadOnlyList<AddressingBackendRange> FreezeRanges(System.Nullable<IReadOnlyList<AddressingBackendRange>> ranges)
//		{
//			if (ranges == nullptr || ranges.Count == 0)
//			{
//				return Array.Empty<AddressingBackendRange>();
//			}
//
//			AddressingBackendRange[] copy = new AddressingBackendRange[ranges.Count];
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				copy[index] = ranges[index];
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AddressingBackendResolution
//	{
//		private AddressingBackendResolution(AddressingBackendDecisionKind decisionKind, AddressingBackendKind backendKind, System.Nullable<IBurstBackend> backend, AddressingBackendFaultKind faultKind, string message)
//		{
//			DecisionKind = decisionKind;
//			BackendKind = backendKind;
//			Backend = backend;
//			FaultKind = faultKind;
//			Message = message;
//		}
//
//		public AddressingBackendDecisionKind DecisionKind
//		{
//			get;
//		}
//
//		public AddressingBackendKind BackendKind
//		{
//			get;
//		}
//
//		public System.Nullable<IBurstBackend> Backend
//		{
//			get;
//		}
//
//		public AddressingBackendFaultKind FaultKind
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool IsSelected
//		{
//			get
//			{
//				return DecisionKind == AddressingBackendDecisionKind.Selected;
//			}
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return DecisionKind == AddressingBackendDecisionKind.Rejected;
//			}
//		}
//
//		public bool IsFaulted
//		{
//			get
//			{
//				return DecisionKind == AddressingBackendDecisionKind.Faulted;
//			}
//		}
//
//		public bool IsCurrentArchitecturalExecution
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool ChangesCurrentDscHelperPath
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool ChangesCurrentL7CarrierExecution
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool AllowsSilentFallbackToPhysical
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		internal static AddressingBackendResolution Selected(AddressingBackendKind backendKind, IBurstBackend backend, string message)
//		{
//			return new(AddressingBackendDecisionKind.Selected, backendKind, backend, AddressingBackendFaultKind.None, message);
//		}
//
//		internal static AddressingBackendResolution Rejected(AddressingBackendFaultKind faultKind, string message)
//		{
//			return new(AddressingBackendDecisionKind.Rejected, AddressingBackendKind.None, backend: nullptr, faultKind, message);
//		}
//
//		internal static AddressingBackendResolution Faulted(AddressingBackendFaultKind faultKind, string message)
//		{
//			return new(AddressingBackendDecisionKind.Faulted, AddressingBackendKind.None, backend: nullptr, faultKind, message);
//		}
//	}

	class AddressingBackendResolver final : public std::enable_shared_from_this<AddressingBackendResolver>
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly IBurstBackend? _physicalBackend;
		const std::shared_ptr<IBurstBackend> _physicalBackend;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly IBurstBackend? _iommuBackend;
		const std::shared_ptr<IBurstBackend> _iommuBackend;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AddressingBackendResolver(IBurstBackend? physicalBackend = null, IBurstBackend? iommuBackend = null)
		AddressingBackendResolver(std::shared_ptr<IBurstBackend> physicalBackend = nullptr, std::shared_ptr<IBurstBackend> iommuBackend = nullptr);

		std::shared_ptr<AddressingBackendResolution> Resolve(std::shared_ptr<AddressingBackendResolutionRequest> request);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamComputeFaultRecord CreateDmaStreamComputeFaultRecord(DmaStreamComputeDescriptor descriptor, AddressingBackendFaultKind faultKind, ulong faultAddress, bool isWrite, string? message = null)
		std::shared_ptr<DmaStreamComputeFaultRecord> CreateDmaStreamComputeFaultRecord(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, AddressingBackendFaultKind faultKind, unsigned long long faultAddress, bool isWrite, const std::wstring &message = L"");

	private:
		std::shared_ptr<AddressingBackendResolution> ResolvePhysical(std::shared_ptr<AddressingBackendResolutionRequest> request);

		std::shared_ptr<AddressingBackendResolution> ResolveIommu(std::shared_ptr<AddressingBackendResolutionRequest> request);

		static std::optional<std::shared_ptr<AddressingBackendResolution>> ValidateAuthority(std::shared_ptr<AddressingBackendResolutionRequest> request);

		static DmaStreamComputeTokenFaultKind MapFaultKind(AddressingBackendFaultKind faultKind);

		static DmaStreamComputeFaultSourcePhase MapSourcePhase(AddressingBackendFaultKind faultKind);
	};

}
