#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <optional>
#include <memory>
#include <stdexcept>
#include <climits>
#include <utility>
#include <algorithm>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	class DmaStreamComputeDescriptor;
	struct DmaStreamComputeActiveTokenEntry;
	class DmaStreamComputeMemoryRange;
}

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{
	class AcceleratorCommandDescriptor;
	struct AcceleratorMemoryRange;
}

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::MemoryOrdering
{

	enum class GlobalMemoryConflictServiceMode : unsigned char
	{
		Absent = 0,
		PresentPassive = 1,
		PresentEnforcing = 2
	};

	enum class GlobalMemoryParticipantKind : unsigned char
	{
		CpuLoad = 0,
		CpuStore = 1,
		CpuAtomic = 2,
		DmaStreamComputeToken = 3,
		DmaController = 4,
		StreamEngine = 5,
		SrfAssist = 6,
		L7Accelerator = 7,
		CacheProtocol = 8
	};

	enum class GlobalMemoryOperationKind : unsigned char
	{
		Read = 0,
		Write = 1,
		ReadWrite = 2,
		Atomic = 3,
		Reservation = 4,
		Commit = 5,
		Fence = 6,
		Wait = 7,
		Poll = 8,
		Invalidate = 9,
		Flush = 10
	};

	enum class GlobalMemoryAddressSpaceKind : unsigned char
	{
		Physical = 0,
		IommuTranslated = 1,
		Unresolved = 2
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum GlobalMemoryRangeAccessKind : byte
	enum class GlobalMemoryRangeAccessKind : unsigned char
	{
		None = 0,
		Read = 1,
		Write = 2,
		Atomic = 4,
		Reservation = 8
	};

	enum class GlobalMemoryConflictClass : unsigned char
	{
		None = 0,
		ServiceAbsent = 1,
		IncompleteFootprintTruth = 2,
		UnresolvedAddressSpaceRequiresPolicy = 3,
		AddressSpaceMismatchRequiresPolicy = 4,
		DomainMismatchRequiresPolicy = 5,
		MappingEpochMismatchRequiresPolicy = 6,
		MappingEpochRequiredForEnforcingMode = 7,
		ReadAfterWriteOverlap = 8,
		WriteAfterReadOverlap = 9,
		WriteWriteOverlap = 10,
		AtomicReservationOverlap = 11,
		FenceWaitPollFutureGated = 12
	};

	enum class GlobalMemoryConflictDecisionKind : unsigned char
	{
		Accept = 0,
		Stall = 1,
		Replay = 2,
		Serialize = 3,
		Reject = 4,
		Fault = 5,
		Cancel = 6
	};

	class GlobalMemoryFootprint;

	// Value-type range contract corresponding to the C# readonly record struct.
	struct GlobalMemoryFootprintRange final
	{
		const unsigned long long Address;
		const unsigned long long Length;
		const GlobalMemoryRangeAccessKind AccessKind;
		const bool IsRead;
		const bool IsWrite;
		const bool IsAtomicOrReservation;
		const bool IsWellFormed;

		constexpr GlobalMemoryFootprintRange()
			: GlobalMemoryFootprintRange(0, 0, GlobalMemoryRangeAccessKind::None) {}

		constexpr GlobalMemoryFootprintRange(unsigned long long address, unsigned long long length, GlobalMemoryRangeAccessKind accessKind)
			: Address(address), Length(length), AccessKind(accessKind),
			IsRead((static_cast<unsigned char>(accessKind) & static_cast<unsigned char>(GlobalMemoryRangeAccessKind::Read)) != 0),
			IsWrite((static_cast<unsigned char>(accessKind) & static_cast<unsigned char>(GlobalMemoryRangeAccessKind::Write)) != 0),
			IsAtomicOrReservation((static_cast<unsigned char>(accessKind) & (static_cast<unsigned char>(GlobalMemoryRangeAccessKind::Atomic) | static_cast<unsigned char>(GlobalMemoryRangeAccessKind::Reservation))) != 0),
			IsWellFormed(length != 0 && address <= ULLONG_MAX - length && accessKind != GlobalMemoryRangeAccessKind::None) {}

		constexpr bool Overlaps(const GlobalMemoryFootprintRange &other) const
		{
			if (!IsWellFormed || !other.IsWellFormed)
				return false;
			const auto leftEnd = Address + Length;
			const auto rightEnd = other.Address + other.Length;
			return Address < rightEnd && other.Address < leftEnd;
		}

		constexpr bool operator==(const GlobalMemoryFootprintRange &other) const = default;
	};

	constexpr GlobalMemoryRangeAccessKind operator|(GlobalMemoryRangeAccessKind left, GlobalMemoryRangeAccessKind right)
	{
		return static_cast<GlobalMemoryRangeAccessKind>(static_cast<unsigned char>(left) | static_cast<unsigned char>(right));
	}

	constexpr GlobalMemoryRangeAccessKind operator&(GlobalMemoryRangeAccessKind left, GlobalMemoryRangeAccessKind right)
	{
		return static_cast<GlobalMemoryRangeAccessKind>(static_cast<unsigned char>(left) & static_cast<unsigned char>(right));
	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct GlobalMemoryFootprintRange
//	{
//		public GlobalMemoryFootprintRange(ulong address, ulong length, GlobalMemoryRangeAccessKind accessKind)
//		{
//			Address = address;
//			Length = length;
//			AccessKind = accessKind;
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
//		public GlobalMemoryRangeAccessKind AccessKind
//		{
//			get;
//		}
//
//		public bool IsRead
//		{
//			get
//			{
//				return (AccessKind & GlobalMemoryRangeAccessKind.Read) != 0;
//			}
//		}
//
//		public bool IsWrite
//		{
//			get
//			{
//				return (AccessKind & GlobalMemoryRangeAccessKind.Write) != 0;
//			}
//		}
//
//		public bool IsAtomicOrReservation
//		{
//			get
//			{
//				return (AccessKind & (GlobalMemoryRangeAccessKind.Atomic | GlobalMemoryRangeAccessKind.Reservation)) != 0;
//			}
//		}
//
//		public bool IsWellFormed
//		{
//			get
//			{
//				return Length != 0 && Address <= ulong.MaxValue - Length && AccessKind != GlobalMemoryRangeAccessKind.None;
//			}
//		}
//
//		public bool Overlaps(GlobalMemoryFootprintRange other)
//		{
//			if (!IsWellFormed || !other.IsWellFormed)
//			{
//				return false;
//			}
//
//			ulong leftEnd = Address + Length;
//			ulong rightEnd = other.Address + other.Length;
//			return Address < rightEnd && other.Address < leftEnd;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record GlobalMemoryParticipantIdentity
//	{
//		public const uint CpuDeviceId = 0;
//
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
//		public ulong SubmissionSequence
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
//		public bool HasTokenOrCommandIdentity
//		{
//			get
//			{
//				return TokenId != 0 || CommandId != 0 || SubmissionSequence != 0;
//			}
//		}
//
//		public bool RequiresTokenIdentityForEnforcingMode
//		{
//			get
//			{
//				return TokenId == 0 && CommandId == 0;
//			}
//		}
//
//		public bool RequiresMappingEpochForEnforcingMode
//		{
//			get
//			{
//				return !MappingEpochKnown;
//			}
//		}
//
//		public static GlobalMemoryParticipantIdentity FromCpu(ushort ownerVirtualThreadId, uint ownerContextId, uint ownerCoreId, uint ownerPodId, ulong memoryDomainTag, ulong mappingEpoch = 0, bool mappingEpochKnown = false)
//		{
//			return new() { OwnerVirtualThreadId = ownerVirtualThreadId, OwnerContextId = ownerContextId, OwnerCoreId = ownerCoreId, OwnerPodId = ownerPodId, MemoryDomainTag = memoryDomainTag, ActiveDomainCertificate = memoryDomainTag, DeviceId = CpuDeviceId, MappingEpoch = mappingEpoch, MappingEpochKnown = mappingEpochKnown };
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record GlobalMemoryFootprint
//	{
//		public GlobalMemoryFootprint(GlobalMemoryParticipantKind participantKind, GlobalMemoryOperationKind operationKind, GlobalMemoryAddressSpaceKind addressSpaceKind, GlobalMemoryParticipantIdentity identity, IReadOnlyList<GlobalMemoryFootprintRange> ranges)
//		{
//			ArgumentNullException.ThrowIfNull(identity);
//			ParticipantKind = participantKind;
//			OperationKind = operationKind;
//			AddressSpaceKind = addressSpaceKind;
//			Identity = identity;
//			Ranges = FreezeRanges(ranges);
//		}
//
//		public GlobalMemoryParticipantKind ParticipantKind
//		{
//			get;
//		}
//
//		public GlobalMemoryOperationKind OperationKind
//		{
//			get;
//		}
//
//		public GlobalMemoryAddressSpaceKind AddressSpaceKind
//		{
//			get;
//		}
//
//		public GlobalMemoryParticipantIdentity Identity
//		{
//			get;
//		}
//
//		public IReadOnlyList<GlobalMemoryFootprintRange> Ranges
//		{
//			get;
//		}
//
//		public bool IsBoundaryObservation
//		{
//			get
//			{
//				return OperationKind is GlobalMemoryOperationKind.Fence or GlobalMemoryOperationKind.Wait or GlobalMemoryOperationKind.Poll;
//			}
//		}
//
//		public bool HasWriteAccess
//		{
//			get
//			{
//				for (int index = 0; index < Ranges.Count; index++)
//				{
//					if (Ranges[index].IsWrite)
//					{
//						return true;
//					}
//				}
//
//				return false;
//			}
//		}
//
//		public bool HasReadAccess
//		{
//			get
//			{
//				for (int index = 0; index < Ranges.Count; index++)
//				{
//					if (Ranges[index].IsRead)
//					{
//						return true;
//					}
//				}
//
//				return false;
//			}
//		}
//
//		public bool HasCompleteFootprintTruth
//		{
//			get
//			{
//				if (IsBoundaryObservation)
//				{
//					return true;
//				}
//
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
//		public bool RequiresMappingEpochBeforeEnforcing
//		{
//			get
//			{
//				return AddressSpaceKind != GlobalMemoryAddressSpaceKind.Physical && Identity.RequiresMappingEpochForEnforcingMode;
//			}
//		}
//
//		public static GlobalMemoryFootprint CpuScalarLoad(ushort ownerVirtualThreadId, uint ownerContextId, uint ownerCoreId, uint ownerPodId, ulong memoryDomainTag, ulong address, ulong length, ulong mappingEpoch = 0, bool mappingEpochKnown = false)
//		{
//			return new(GlobalMemoryParticipantKind.CpuLoad, GlobalMemoryOperationKind.Read, GlobalMemoryAddressSpaceKind.Physical, GlobalMemoryParticipantIdentity.FromCpu(ownerVirtualThreadId, ownerContextId, ownerCoreId, ownerPodId, memoryDomainTag, mappingEpoch, mappingEpochKnown), new[] { new GlobalMemoryFootprintRange(address, length, GlobalMemoryRangeAccessKind.Read) });
//		}
//
//		public static GlobalMemoryFootprint CpuScalarStore(ushort ownerVirtualThreadId, uint ownerContextId, uint ownerCoreId, uint ownerPodId, ulong memoryDomainTag, ulong address, ulong length, ulong mappingEpoch = 0, bool mappingEpochKnown = false)
//		{
//			return new(GlobalMemoryParticipantKind.CpuStore, GlobalMemoryOperationKind.Write, GlobalMemoryAddressSpaceKind.Physical, GlobalMemoryParticipantIdentity.FromCpu(ownerVirtualThreadId, ownerContextId, ownerCoreId, ownerPodId, memoryDomainTag, mappingEpoch, mappingEpochKnown), new[] { new GlobalMemoryFootprintRange(address, length, GlobalMemoryRangeAccessKind.Write) });
//		}
//
//		public static GlobalMemoryFootprint CpuAtomic(ushort ownerVirtualThreadId, uint ownerContextId, uint ownerCoreId, uint ownerPodId, ulong memoryDomainTag, ulong address, ulong length, bool isReservation, ulong mappingEpoch = 0, bool mappingEpochKnown = false)
//		{
//			return new(GlobalMemoryParticipantKind.CpuAtomic, isReservation ? GlobalMemoryOperationKind.Reservation : GlobalMemoryOperationKind.Atomic, GlobalMemoryAddressSpaceKind.Physical, GlobalMemoryParticipantIdentity.FromCpu(ownerVirtualThreadId, ownerContextId, ownerCoreId, ownerPodId, memoryDomainTag, mappingEpoch, mappingEpochKnown), new[] { new GlobalMemoryFootprintRange(address, length, GlobalMemoryRangeAccessKind.Read | GlobalMemoryRangeAccessKind.Write | GlobalMemoryRangeAccessKind.Atomic | (isReservation ? GlobalMemoryRangeAccessKind.Reservation : GlobalMemoryRangeAccessKind.None)) });
//		}
//
//		public static GlobalMemoryFootprint FutureBoundary(GlobalMemoryOperationKind operationKind, GlobalMemoryParticipantIdentity identity)
//		{
//			if (operationKind is @not(GlobalMemoryOperationKind.Fence or GlobalMemoryOperationKind.Wait or GlobalMemoryOperationKind.Poll))
//			{
//				throw new ArgumentOutOfRangeException(nameof(operationKind), operationKind, "Only fence, wait, and poll are accepted as future boundary observations.");
//			}
//
//			return new GlobalMemoryFootprint(GlobalMemoryParticipantKind.CpuLoad, operationKind, GlobalMemoryAddressSpaceKind.Unresolved, identity, Array.Empty<GlobalMemoryFootprintRange>());
//		}
//
//		public static GlobalMemoryFootprint FromDmaStreamCompute(DmaStreamComputeDescriptor descriptor, ulong tokenId = 0, ulong issueAge = 0, ulong replayGeneration = 0)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			DmaStreamComputeOwnerBinding owner = descriptor.OwnerBinding;
//			DmaStreamComputeOwnerGuardContext guardContext = descriptor.OwnerGuardDecision.RuntimeOwnerContext;
//			return new GlobalMemoryFootprint(GlobalMemoryParticipantKind.DmaStreamComputeToken, GlobalMemoryOperationKind.ReadWrite, GlobalMemoryAddressSpaceKind.Physical, new GlobalMemoryParticipantIdentity { OwnerVirtualThreadId = owner.OwnerVirtualThreadId, OwnerContextId = owner.OwnerContextId, OwnerCoreId = owner.OwnerCoreId, OwnerPodId = owner.OwnerPodId, MemoryDomainTag = owner.OwnerDomainTag, ActiveDomainCertificate = guardContext.ActiveDomainCertificate, DeviceId = owner.DeviceId, MappingEpochKnown = false, TokenId = tokenId, DescriptorIdentityHash = descriptor.DescriptorIdentityHash, NormalizedFootprintHash = descriptor.NormalizedFootprintHash, IssueAge = issueAge, ReplayGeneration = replayGeneration }, MergeRanges(descriptor.NormalizedReadMemoryRanges, descriptor.NormalizedWriteMemoryRanges));
//		}
//
//		public static GlobalMemoryFootprint FromDmaStreamCompute(DmaStreamComputeActiveTokenEntry entry)
//		{
//			ArgumentNullException.ThrowIfNull(entry);
//			return FromDmaStreamCompute(entry.Token.Descriptor, entry.Handle.TokenId, entry.Metadata.IssueCycle, entry.Metadata.ReplayEpoch);
//		}
//
//		public static GlobalMemoryFootprint FromL7Accelerator(AcceleratorCommandDescriptor descriptor, ulong tokenId = 0, ulong submissionSequence = 0, ulong issueAge = 0, ulong replayGeneration = 0)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			AcceleratorOwnerBinding owner = descriptor.OwnerBinding;
//			return new GlobalMemoryFootprint(GlobalMemoryParticipantKind.L7Accelerator, GlobalMemoryOperationKind.ReadWrite, GlobalMemoryAddressSpaceKind.IommuTranslated, new GlobalMemoryParticipantIdentity { OwnerVirtualThreadId = owner.OwnerVirtualThreadId, OwnerContextId = owner.OwnerContextId, OwnerCoreId = owner.OwnerCoreId, OwnerPodId = owner.OwnerPodId, MemoryDomainTag = owner.DomainTag, ActiveDomainCertificate = (descriptor.OwnerGuardDecision.Evidence == nullptr ? nullptr : descriptor.OwnerGuardDecision.Evidence.ActiveDomainCertificate) ?? 0, DeviceId = (uint)descriptor.AcceleratorId, MappingEpoch = descriptor.OwnerGuardDecision.MappingEpoch.Value, MappingEpochKnown = descriptor.OwnerGuardDecision.Evidence != nullptr, TokenId = tokenId, DescriptorIdentityHash = descriptor.Identity.DescriptorIdentityHash, NormalizedFootprintHash = descriptor.Identity.NormalizedFootprintHash, SubmissionSequence = submissionSequence, IssueAge = issueAge, ReplayGeneration = replayGeneration }, MergeRanges(descriptor.NormalizedFootprint.SourceRanges, descriptor.NormalizedFootprint.DestinationRanges, descriptor.NormalizedFootprint.ScratchRanges));
//		}
//
//		public static GlobalMemoryFootprint FromDmaControllerTransfer(ulong sourceAddress, ulong destinationAddress, ulong length, ulong memoryDomainTag, uint deviceId, ulong commandId = 0, bool useIommuTranslation = false, ulong mappingEpoch = 0, bool mappingEpochKnown = false)
//		{
//			return new(GlobalMemoryParticipantKind.DmaController, GlobalMemoryOperationKind.ReadWrite, useIommuTranslation ? GlobalMemoryAddressSpaceKind.IommuTranslated : GlobalMemoryAddressSpaceKind.Physical, new GlobalMemoryParticipantIdentity { OwnerVirtualThreadId = 0, OwnerContextId = 0, OwnerCoreId = 0, OwnerPodId = 0, MemoryDomainTag = memoryDomainTag, ActiveDomainCertificate = memoryDomainTag, DeviceId = deviceId, MappingEpoch = mappingEpoch, MappingEpochKnown = mappingEpochKnown, CommandId = commandId }, new[] { new GlobalMemoryFootprintRange(sourceAddress, length, GlobalMemoryRangeAccessKind.Read), new GlobalMemoryFootprintRange(destinationAddress, length, GlobalMemoryRangeAccessKind.Write) });
//		}
//
//		private static IReadOnlyList<GlobalMemoryFootprintRange> MergeRanges(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges)
//		{
//			int readCount = (readRanges == nullptr ? nullptr : readRanges.Count) ?? 0;
//			int writeCount = (writeRanges == nullptr ? nullptr : writeRanges.Count) ?? 0;
//			GlobalMemoryFootprintRange[] result = new GlobalMemoryFootprintRange[readCount + writeCount];
//			int cursor = 0;
//			for (int index = 0; index < readCount; index++)
//			{
//				result[cursor++] = new GlobalMemoryFootprintRange(readRanges![index].Address, readRanges[index].Length, GlobalMemoryRangeAccessKind.Read);
//			}
//
//			for (int index = 0; index < writeCount; index++)
//			{
//				result[cursor++] = new GlobalMemoryFootprintRange(writeRanges![index].Address, writeRanges[index].Length, GlobalMemoryRangeAccessKind.Write);
//			}
//
//			return result;
//		}
//
//		private static IReadOnlyList<GlobalMemoryFootprintRange> MergeRanges(IReadOnlyList<AcceleratorMemoryRange> readRanges, IReadOnlyList<AcceleratorMemoryRange> writeRanges, IReadOnlyList<AcceleratorMemoryRange> scratchRanges)
//		{
//			int readCount = (readRanges == nullptr ? nullptr : readRanges.Count) ?? 0;
//			int writeCount = (writeRanges == nullptr ? nullptr : writeRanges.Count) ?? 0;
//			int scratchCount = (scratchRanges == nullptr ? nullptr : scratchRanges.Count) ?? 0;
//			GlobalMemoryFootprintRange[] result = new GlobalMemoryFootprintRange[readCount + writeCount + scratchCount];
//			int cursor = 0;
//			for (int index = 0; index < readCount; index++)
//			{
//				result[cursor++] = new GlobalMemoryFootprintRange(readRanges![index].Address, readRanges[index].Length, GlobalMemoryRangeAccessKind.Read);
//			}
//
//			for (int index = 0; index < writeCount; index++)
//			{
//				result[cursor++] = new GlobalMemoryFootprintRange(writeRanges![index].Address, writeRanges[index].Length, GlobalMemoryRangeAccessKind.Write);
//			}
//
//			for (int index = 0; index < scratchCount; index++)
//			{
//				result[cursor++] = new GlobalMemoryFootprintRange(scratchRanges![index].Address, scratchRanges[index].Length, GlobalMemoryRangeAccessKind.Read | GlobalMemoryRangeAccessKind.Write);
//			}
//
//			return result;
//		}
//
//		private static IReadOnlyList<GlobalMemoryFootprintRange> FreezeRanges(System.Nullable<IReadOnlyList<GlobalMemoryFootprintRange>> ranges)
//		{
//			if (ranges == nullptr || ranges.Count == 0)
//			{
//				return Array.Empty<GlobalMemoryFootprintRange>();
//			}
//
//			GlobalMemoryFootprintRange[] copy = new GlobalMemoryFootprintRange[ranges.Count];
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				copy[index] = ranges[index];
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

	// Immutable participant identity corresponding to the C# record contract.
	class GlobalMemoryParticipantIdentity final
	{
	public:
		static constexpr unsigned int CpuDeviceId = 0;
		const unsigned short OwnerVirtualThreadId;
		const unsigned int OwnerContextId;
		const unsigned int OwnerCoreId;
		const unsigned int OwnerPodId;
		const unsigned long long MemoryDomainTag;
		const unsigned long long ActiveDomainCertificate;
		const unsigned int DeviceId;
		const unsigned long long MappingEpoch;
		const bool MappingEpochKnown;
		const unsigned long long TokenId;
		const unsigned long long CommandId;
		const unsigned long long DescriptorIdentityHash;
		const unsigned long long NormalizedFootprintHash;
		const unsigned long long SubmissionSequence;
		const unsigned long long IssueAge;
		const unsigned long long ReplayGeneration;
		const bool HasTokenOrCommandIdentity;
		const bool RequiresTokenIdentityForEnforcingMode;
		const bool RequiresMappingEpochForEnforcingMode;

		GlobalMemoryParticipantIdentity(unsigned short ownerVirtualThreadId,
			unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId,
			unsigned long long memoryDomainTag, unsigned long long activeDomainCertificate,
			unsigned int deviceId, unsigned long long mappingEpoch = 0, bool mappingEpochKnown = false,
			unsigned long long tokenId = 0, unsigned long long commandId = 0,
			unsigned long long descriptorIdentityHash = 0, unsigned long long normalizedFootprintHash = 0,
			unsigned long long submissionSequence = 0, unsigned long long issueAge = 0,
			unsigned long long replayGeneration = 0)
			: OwnerVirtualThreadId(ownerVirtualThreadId), OwnerContextId(ownerContextId), OwnerCoreId(ownerCoreId),
			OwnerPodId(ownerPodId), MemoryDomainTag(memoryDomainTag), ActiveDomainCertificate(activeDomainCertificate),
			DeviceId(deviceId), MappingEpoch(mappingEpoch), MappingEpochKnown(mappingEpochKnown), TokenId(tokenId),
			CommandId(commandId), DescriptorIdentityHash(descriptorIdentityHash),
			NormalizedFootprintHash(normalizedFootprintHash), SubmissionSequence(submissionSequence),
			IssueAge(issueAge), ReplayGeneration(replayGeneration),
			HasTokenOrCommandIdentity(tokenId != 0 || commandId != 0 || submissionSequence != 0),
			RequiresTokenIdentityForEnforcingMode(tokenId == 0 && commandId == 0),
			RequiresMappingEpochForEnforcingMode(!mappingEpochKnown) {}

		static std::shared_ptr<GlobalMemoryParticipantIdentity> FromCpu(unsigned short ownerVirtualThreadId,
			unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId,
			unsigned long long memoryDomainTag, unsigned long long mappingEpoch = 0,
			bool mappingEpochKnown = false)
		{
			return std::make_shared<GlobalMemoryParticipantIdentity>(ownerVirtualThreadId, ownerContextId,
				ownerCoreId, ownerPodId, memoryDomainTag, memoryDomainTag, CpuDeviceId, mappingEpoch,
				mappingEpochKnown);
		}

		bool operator==(const GlobalMemoryParticipantIdentity &other) const = default;
	};

	// Frozen read-only value collection. Its operator-> and indexing preserve the
	// existing C++ call surface while each input range is copied into owned storage.
	class GlobalMemoryFootprintRangeList final
	{
	private:
		static std::vector<std::shared_ptr<GlobalMemoryFootprintRange>> CloneRanges(
			const std::vector<GlobalMemoryFootprintRange> &ranges)
		{
			std::vector<std::shared_ptr<GlobalMemoryFootprintRange>> copy;
			copy.reserve(ranges.size());
			for (const auto &range : ranges)
				copy.push_back(std::make_shared<GlobalMemoryFootprintRange>(range));
			return copy;
		}

		const std::vector<std::shared_ptr<GlobalMemoryFootprintRange>> _ranges;

	public:
		const std::size_t Count;

		GlobalMemoryFootprintRangeList() : _ranges(), Count(0) {}
		explicit GlobalMemoryFootprintRangeList(const std::vector<GlobalMemoryFootprintRange> &ranges)
			: _ranges(CloneRanges(ranges)), Count(_ranges.size()) {}

		const GlobalMemoryFootprintRangeList *operator->() const noexcept { return this; }
		std::shared_ptr<GlobalMemoryFootprintRange> operator[](std::size_t index) const { return _ranges.at(index); }
		std::vector<std::shared_ptr<GlobalMemoryFootprintRange>>::const_iterator begin() const noexcept { return _ranges.begin(); }
		std::vector<std::shared_ptr<GlobalMemoryFootprintRange>>::const_iterator end() const noexcept { return _ranges.end(); }

		bool operator==(const GlobalMemoryFootprintRangeList &other) const
		{
			if (Count != other.Count)
				return false;
			for (std::size_t index = 0; index < Count; ++index)
				if (*_ranges[index] != *other._ranges[index])
					return false;
			return true;
		}
	};

	class GlobalMemoryFootprint final
	{
	private:
		static bool IsBoundaryOperation(GlobalMemoryOperationKind operationKind)
		{
			return operationKind == GlobalMemoryOperationKind::Fence ||
				operationKind == GlobalMemoryOperationKind::Wait ||
				operationKind == GlobalMemoryOperationKind::Poll;
		}

		static bool HasRangeAccess(const GlobalMemoryFootprintRangeList &ranges, bool wantWrite)
		{
			for (std::size_t index = 0; index < ranges.Count; ++index)
			{
				const auto range = ranges[index];
				if (wantWrite ? range->IsWrite : range->IsRead)
					return true;
			}
			return false;
		}

		static bool HasCompleteRanges(const GlobalMemoryFootprintRangeList &ranges)
		{
			if (ranges.Count == 0)
				return false;
			for (std::size_t index = 0; index < ranges.Count; ++index)
				if (!ranges[index]->IsWellFormed)
					return false;
			return true;
		}

	public:
		const GlobalMemoryParticipantKind ParticipantKind;
		const GlobalMemoryOperationKind OperationKind;
		const GlobalMemoryAddressSpaceKind AddressSpaceKind;
		const std::shared_ptr<GlobalMemoryParticipantIdentity> Identity;
		const GlobalMemoryFootprintRangeList Ranges;
		const bool IsBoundaryObservation;
		const bool HasWriteAccess;
		const bool HasReadAccess;
		const bool HasCompleteFootprintTruth;
		const bool RequiresMappingEpochBeforeEnforcing;

		GlobalMemoryFootprint(GlobalMemoryParticipantKind participantKind,
			GlobalMemoryOperationKind operationKind, GlobalMemoryAddressSpaceKind addressSpaceKind,
			std::shared_ptr<GlobalMemoryParticipantIdentity> identity,
			const std::vector<GlobalMemoryFootprintRange> &ranges)
			: ParticipantKind(participantKind), OperationKind(operationKind), AddressSpaceKind(addressSpaceKind),
			Identity(std::move(identity)), Ranges(ranges), IsBoundaryObservation(IsBoundaryOperation(operationKind)),
			HasWriteAccess(HasRangeAccess(Ranges, true)), HasReadAccess(HasRangeAccess(Ranges, false)),
			HasCompleteFootprintTruth(IsBoundaryObservation || HasCompleteRanges(Ranges)),
			RequiresMappingEpochBeforeEnforcing(addressSpaceKind != GlobalMemoryAddressSpaceKind::Physical &&
				Identity != nullptr && Identity->RequiresMappingEpochForEnforcingMode)
		{
			if (Identity == nullptr)
				throw std::invalid_argument("identity");
		}

		static std::shared_ptr<GlobalMemoryFootprint> CpuScalarLoad(unsigned short ownerVirtualThreadId,
			unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId,
			unsigned long long memoryDomainTag, unsigned long long address, unsigned long long length,
			unsigned long long mappingEpoch = 0, bool mappingEpochKnown = false)
		{
			return std::make_shared<GlobalMemoryFootprint>(GlobalMemoryParticipantKind::CpuLoad,
				GlobalMemoryOperationKind::Read, GlobalMemoryAddressSpaceKind::Physical,
				GlobalMemoryParticipantIdentity::FromCpu(ownerVirtualThreadId, ownerContextId, ownerCoreId,
					ownerPodId, memoryDomainTag, mappingEpoch, mappingEpochKnown),
				std::vector<GlobalMemoryFootprintRange>{GlobalMemoryFootprintRange(address, length, GlobalMemoryRangeAccessKind::Read)});
		}

		static std::shared_ptr<GlobalMemoryFootprint> CpuScalarStore(unsigned short ownerVirtualThreadId,
			unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId,
			unsigned long long memoryDomainTag, unsigned long long address, unsigned long long length,
			unsigned long long mappingEpoch = 0, bool mappingEpochKnown = false)
		{
			return std::make_shared<GlobalMemoryFootprint>(GlobalMemoryParticipantKind::CpuStore,
				GlobalMemoryOperationKind::Write, GlobalMemoryAddressSpaceKind::Physical,
				GlobalMemoryParticipantIdentity::FromCpu(ownerVirtualThreadId, ownerContextId, ownerCoreId,
					ownerPodId, memoryDomainTag, mappingEpoch, mappingEpochKnown),
				std::vector<GlobalMemoryFootprintRange>{GlobalMemoryFootprintRange(address, length, GlobalMemoryRangeAccessKind::Write)});
		}

		static std::shared_ptr<GlobalMemoryFootprint> CpuAtomic(unsigned short ownerVirtualThreadId,
			unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId,
			unsigned long long memoryDomainTag, unsigned long long address, unsigned long long length,
			bool isReservation, unsigned long long mappingEpoch = 0, bool mappingEpochKnown = false)
		{
			const auto kind = isReservation ? GlobalMemoryOperationKind::Reservation : GlobalMemoryOperationKind::Atomic;
			const auto access = GlobalMemoryRangeAccessKind::Read | GlobalMemoryRangeAccessKind::Write |
				GlobalMemoryRangeAccessKind::Atomic |
				(isReservation ? GlobalMemoryRangeAccessKind::Reservation : GlobalMemoryRangeAccessKind::None);
			return std::make_shared<GlobalMemoryFootprint>(GlobalMemoryParticipantKind::CpuAtomic, kind,
				GlobalMemoryAddressSpaceKind::Physical,
				GlobalMemoryParticipantIdentity::FromCpu(ownerVirtualThreadId, ownerContextId, ownerCoreId,
					ownerPodId, memoryDomainTag, mappingEpoch, mappingEpochKnown),
				std::vector<GlobalMemoryFootprintRange>{GlobalMemoryFootprintRange(address, length, access)});
		}

		static std::shared_ptr<GlobalMemoryFootprint> FutureBoundary(GlobalMemoryOperationKind operationKind,
			std::shared_ptr<GlobalMemoryParticipantIdentity> identity)
		{
			if (!IsBoundaryOperation(operationKind))
				throw std::out_of_range("Only fence, wait, and poll are accepted as future boundary observations.");
			return std::make_shared<GlobalMemoryFootprint>(GlobalMemoryParticipantKind::CpuLoad, operationKind,
				GlobalMemoryAddressSpaceKind::Unresolved, std::move(identity), std::vector<GlobalMemoryFootprintRange>{});
		}

		static std::shared_ptr<GlobalMemoryFootprint> FromDmaStreamCompute(
			std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long tokenId = 0,
			unsigned long long issueAge = 0, unsigned long long replayGeneration = 0);
		static std::shared_ptr<GlobalMemoryFootprint> FromDmaStreamCompute(
			std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry);
		static std::shared_ptr<GlobalMemoryFootprint> FromL7Accelerator(
			std::shared_ptr<AcceleratorCommandDescriptor> descriptor, unsigned long long tokenId = 0,
			unsigned long long submissionSequence = 0, unsigned long long issueAge = 0,
			unsigned long long replayGeneration = 0);

		static std::shared_ptr<GlobalMemoryFootprint> FromDmaControllerTransfer(
			unsigned long long sourceAddress, unsigned long long destinationAddress,
			unsigned long long length, unsigned long long memoryDomainTag, unsigned int deviceId,
			unsigned long long commandId = 0, bool useIommuTranslation = false,
			unsigned long long mappingEpoch = 0, bool mappingEpochKnown = false)
		{
			auto identity = std::make_shared<GlobalMemoryParticipantIdentity>(0, 0, 0, 0,
				memoryDomainTag, memoryDomainTag, deviceId, mappingEpoch, mappingEpochKnown, 0, commandId);
			const auto addressSpace = useIommuTranslation ? GlobalMemoryAddressSpaceKind::IommuTranslated : GlobalMemoryAddressSpaceKind::Physical;
			return std::make_shared<GlobalMemoryFootprint>(GlobalMemoryParticipantKind::DmaController,
				GlobalMemoryOperationKind::ReadWrite, addressSpace, std::move(identity),
				std::vector<GlobalMemoryFootprintRange>{
					GlobalMemoryFootprintRange(sourceAddress, length, GlobalMemoryRangeAccessKind::Read),
					GlobalMemoryFootprintRange(destinationAddress, length, GlobalMemoryRangeAccessKind::Write)});
		}

		bool operator==(const GlobalMemoryFootprint &other) const
		{
			return ParticipantKind == other.ParticipantKind && OperationKind == other.OperationKind &&
				AddressSpaceKind == other.AddressSpaceKind && Identity != nullptr && other.Identity != nullptr &&
				*Identity == *other.Identity && Ranges == other.Ranges;
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record GlobalMemoryConflictObservation
//	{
//		public GlobalMemoryConflictObservation(GlobalMemoryConflictServiceMode serviceMode, GlobalMemoryConflictDecisionKind decisionKind, GlobalMemoryConflictClass conflictClass, GlobalMemoryFootprint incomingFootprint, System.Nullable<GlobalMemoryFootprint> conflictingFootprint, GlobalMemoryFootprintRange conflictRange, string message)
//		{
//			ArgumentNullException.ThrowIfNull(incomingFootprint);
//			ServiceMode = serviceMode;
//			DecisionKind = decisionKind;
//			ConflictClass = conflictClass;
//			IncomingFootprint = incomingFootprint;
//			ConflictingFootprint = conflictingFootprint;
//			ConflictRange = conflictRange;
//			Message = string.IsNullOrWhiteSpace(message) ? string.Format("Global memory conflict observation: {0}.", conflictClass) : message;
//		}
//
//		public GlobalMemoryConflictServiceMode ServiceMode
//		{
//			get;
//		}
//
//		public GlobalMemoryConflictDecisionKind DecisionKind
//		{
//			get;
//		}
//
//		public GlobalMemoryConflictClass ConflictClass
//		{
//			get;
//		}
//
//		public GlobalMemoryFootprint IncomingFootprint
//		{
//			get;
//		}
//
//		public System.Nullable<GlobalMemoryFootprint> ConflictingFootprint
//		{
//			get;
//		}
//
//		public GlobalMemoryFootprintRange ConflictRange
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool IsConflict
//		{
//			get
//			{
//				return ConflictClass is @not GlobalMemoryConflictClass.None @and @not GlobalMemoryConflictClass.ServiceAbsent;
//			}
//		}
//
//		public bool IsPassiveOnly
//		{
//			get
//			{
//				return ServiceMode == GlobalMemoryConflictServiceMode.PresentPassive;
//			}
//		}
//
//		public bool IsCurrentArchitecturalEffect
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool ChangesArchitecturalMemoryResults
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanPublishArchitecturalMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool FutureEnforcingDecisionWouldBlock
//		{
//			get
//			{
//				return DecisionKind is GlobalMemoryConflictDecisionKind.Stall or GlobalMemoryConflictDecisionKind.Replay or GlobalMemoryConflictDecisionKind.Serialize or GlobalMemoryConflictDecisionKind.Reject or GlobalMemoryConflictDecisionKind.Fault or GlobalMemoryConflictDecisionKind.Cancel;
//			}
//		}
//	}

	class GlobalMemoryConflictObservation final
	{
	private:
		static const wchar_t *ConflictClassName(GlobalMemoryConflictClass conflictClass)
		{
			switch (conflictClass)
			{
			case GlobalMemoryConflictClass::None: return L"None";
			case GlobalMemoryConflictClass::ServiceAbsent: return L"ServiceAbsent";
			case GlobalMemoryConflictClass::IncompleteFootprintTruth: return L"IncompleteFootprintTruth";
			case GlobalMemoryConflictClass::UnresolvedAddressSpaceRequiresPolicy: return L"UnresolvedAddressSpaceRequiresPolicy";
			case GlobalMemoryConflictClass::AddressSpaceMismatchRequiresPolicy: return L"AddressSpaceMismatchRequiresPolicy";
			case GlobalMemoryConflictClass::DomainMismatchRequiresPolicy: return L"DomainMismatchRequiresPolicy";
			case GlobalMemoryConflictClass::MappingEpochMismatchRequiresPolicy: return L"MappingEpochMismatchRequiresPolicy";
			case GlobalMemoryConflictClass::MappingEpochRequiredForEnforcingMode: return L"MappingEpochRequiredForEnforcingMode";
			case GlobalMemoryConflictClass::ReadAfterWriteOverlap: return L"ReadAfterWriteOverlap";
			case GlobalMemoryConflictClass::WriteAfterReadOverlap: return L"WriteAfterReadOverlap";
			case GlobalMemoryConflictClass::WriteWriteOverlap: return L"WriteWriteOverlap";
			case GlobalMemoryConflictClass::AtomicReservationOverlap: return L"AtomicReservationOverlap";
			case GlobalMemoryConflictClass::FenceWaitPollFutureGated: return L"FenceWaitPollFutureGated";
			default: return nullptr;
			}
		}

		static bool IsFutureBlockingDecision(GlobalMemoryConflictDecisionKind decisionKind)
		{
			switch (decisionKind)
			{
			case GlobalMemoryConflictDecisionKind::Stall:
			case GlobalMemoryConflictDecisionKind::Replay:
			case GlobalMemoryConflictDecisionKind::Serialize:
			case GlobalMemoryConflictDecisionKind::Reject:
			case GlobalMemoryConflictDecisionKind::Fault:
			case GlobalMemoryConflictDecisionKind::Cancel:
				return true;
			default:
				return false;
			}
		}

	public:
		const GlobalMemoryConflictServiceMode ServiceMode;
		const GlobalMemoryConflictDecisionKind DecisionKind;
		const GlobalMemoryConflictClass ConflictClass;
		const std::shared_ptr<GlobalMemoryFootprint> IncomingFootprint;
		const std::shared_ptr<GlobalMemoryFootprint> ConflictingFootprint;
		const GlobalMemoryFootprintRange ConflictRange;
		const std::wstring Message;
		const bool IsConflict;
		const bool IsPassiveOnly;
		const bool IsCurrentArchitecturalEffect;
		const bool ChangesArchitecturalMemoryResults;
		const bool CanPublishArchitecturalMemory;
		const bool FutureEnforcingDecisionWouldBlock;

		GlobalMemoryConflictObservation(GlobalMemoryConflictServiceMode serviceMode,
			GlobalMemoryConflictDecisionKind decisionKind,
			GlobalMemoryConflictClass conflictClass,
			std::shared_ptr<GlobalMemoryFootprint> incomingFootprint,
			std::shared_ptr<GlobalMemoryFootprint> conflictingFootprint,
			GlobalMemoryFootprintRange conflictRange,
			const std::wstring &message)
			: ServiceMode(serviceMode), DecisionKind(decisionKind), ConflictClass(conflictClass),
			IncomingFootprint(std::move(incomingFootprint)), ConflictingFootprint(std::move(conflictingFootprint)),
			ConflictRange(conflictRange),
			Message(message.empty() || std::all_of(message.begin(), message.end(), [](wchar_t character) { return character == L' ' || character == L'\t' || character == L'\n' || character == L'\r' || character == L'\v' || character == L'\f'; })
				? (ConflictClassName(conflictClass) != nullptr
					? std::wstring(L"Global memory conflict observation: ") + ConflictClassName(conflictClass) + L"."
					: L"Global memory conflict observation." )
				: message),
			IsConflict(conflictClass != GlobalMemoryConflictClass::None && conflictClass != GlobalMemoryConflictClass::ServiceAbsent),
			IsPassiveOnly(serviceMode == GlobalMemoryConflictServiceMode::PresentPassive),
			IsCurrentArchitecturalEffect(false),
			ChangesArchitecturalMemoryResults(false),
			CanPublishArchitecturalMemory(false),
			FutureEnforcingDecisionWouldBlock(IsFutureBlockingDecision(decisionKind))
		{
			if (IncomingFootprint == nullptr)
				throw std::invalid_argument("incomingFootprint");
		}

	};

	class GlobalMemoryConflictService final : public std::enable_shared_from_this<GlobalMemoryConflictService>
	{
	private:
		const GlobalMemoryConflictServiceMode Mode;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<GlobalMemoryFootprint> _activeFootprints = new();
		std::vector<std::shared_ptr<GlobalMemoryFootprint>> _activeFootprints = std::vector<std::shared_ptr<GlobalMemoryFootprint>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<GlobalMemoryConflictObservation> _observations = new();
		std::vector<std::shared_ptr<GlobalMemoryConflictObservation>> _observations = std::vector<std::shared_ptr<GlobalMemoryConflictObservation>>();

		GlobalMemoryConflictService(GlobalMemoryConflictServiceMode mode);

	public:
		GlobalMemoryConflictServiceMode getMode() const;

		int getActiveFootprintCount() const;

		const std::vector<std::shared_ptr<GlobalMemoryConflictObservation>> getObservations() const;

		static std::shared_ptr<GlobalMemoryConflictService> CreateAbsent();

		static std::shared_ptr<GlobalMemoryConflictService> CreatePresentPassive();

		static std::shared_ptr<GlobalMemoryConflictService> CreatePresentEnforcingForFutureGate();

		std::shared_ptr<GlobalMemoryConflictObservation> RegisterActive(std::shared_ptr<GlobalMemoryFootprint> footprint);

		std::shared_ptr<GlobalMemoryConflictObservation> ObserveAccess(std::shared_ptr<GlobalMemoryFootprint> footprint);

		int ReleaseByTokenOrCommand(GlobalMemoryParticipantKind participantKind, unsigned long long tokenId, unsigned long long commandId = 0);

	private:
		std::shared_ptr<GlobalMemoryConflictObservation> ClassifyAgainstActive(std::shared_ptr<GlobalMemoryFootprint> incoming);

		std::shared_ptr<GlobalMemoryConflictObservation> ClassifyPair(std::shared_ptr<GlobalMemoryFootprint> active, std::shared_ptr<GlobalMemoryFootprint> incoming);

		static bool TryFindFirstOverlap(std::shared_ptr<GlobalMemoryFootprint> left, std::shared_ptr<GlobalMemoryFootprint> right, std::shared_ptr<GlobalMemoryFootprintRange> &leftRange, std::shared_ptr<GlobalMemoryFootprintRange> &rightRange, std::shared_ptr<GlobalMemoryFootprintRange> &overlap);

		std::shared_ptr<GlobalMemoryConflictObservation> BoundaryObservation(std::shared_ptr<GlobalMemoryFootprint> incoming);

		std::shared_ptr<GlobalMemoryConflictObservation> Observation(GlobalMemoryConflictDecisionKind decisionKind, GlobalMemoryConflictClass conflictClass, std::shared_ptr<GlobalMemoryFootprint> incoming, std::optional<std::shared_ptr<GlobalMemoryFootprint>> conflicting, std::shared_ptr<GlobalMemoryFootprintRange> conflictRange, const std::wstring &message);

		static std::shared_ptr<GlobalMemoryConflictObservation> AbsentObservation(std::shared_ptr<GlobalMemoryFootprint> incoming);
	};

}
