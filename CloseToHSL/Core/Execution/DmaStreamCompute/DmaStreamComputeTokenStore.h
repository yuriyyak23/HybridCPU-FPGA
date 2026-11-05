#pragma once

#include "DmaStreamComputeToken.h"
#include "DmaStreamComputeTelemetry.h"
#include "DmaStreamComputeValidationResult.h"
#include "DmaStreamComputeOwnerBinding.h"
#include "DmaStreamComputeReplayEvidence.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <memory>
#include <utility>
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core { class DmaStreamComputeMicroOp; }

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputeIssueAdmissionStatus : unsigned char
	{
		Accepted = 0,
		Rejected = 1,
		ArchitecturalFault = 2
	};

	enum class DmaStreamComputeIssueAdmissionRejectKind : unsigned char
	{
		None = 0,
		InvalidIssuePlacement = 1,
		OwnerDomainMismatch = 2,
		StoreCapacity = 3,
		DomainQuota = 4,
		Pressure = 5
	};

	enum class DmaStreamComputeStatusQueryStatus : unsigned char
	{
		Accepted = 0,
		Rejected = 1
	};

	enum class DmaStreamComputeStatusQueryRejectKind : unsigned char
	{
		None = 0,
		InvalidToken = 1,
		OwnerDomainMismatch = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeTokenHandle
//	{
//		public DmaStreamComputeTokenHandle(ulong tokenId, ushort ownerVirtualThreadId, uint ownerContextId, uint ownerCoreId, uint ownerPodId, ulong ownerDomainTag, uint deviceId, ulong generation)
//		{
//			TokenId = tokenId;
//			OwnerVirtualThreadId = ownerVirtualThreadId;
//			OwnerContextId = ownerContextId;
//			OwnerCoreId = ownerCoreId;
//			OwnerPodId = ownerPodId;
//			OwnerDomainTag = ownerDomainTag;
//			DeviceId = deviceId;
//			Generation = generation;
//		}
//
//		public ulong TokenId
//		{
//			get;
//		}
//
//		public ushort OwnerVirtualThreadId
//		{
//			get;
//		}
//
//		public uint OwnerContextId
//		{
//			get;
//		}
//
//		public uint OwnerCoreId
//		{
//			get;
//		}
//
//		public uint OwnerPodId
//		{
//			get;
//		}
//
//		public ulong OwnerDomainTag
//		{
//			get;
//		}
//
//		public uint DeviceId
//		{
//			get;
//		}
//
//		public ulong Generation
//		{
//			get;
//		}
//
//		public bool IsDefault
//		{
//			get
//			{
//				return TokenId == 0 || Generation == 0;
//			}
//		}
//
//		public bool MatchesOwner(DmaStreamComputeOwnerBinding ownerBinding)
//		{
//			ArgumentNullException.ThrowIfNull(ownerBinding);
//			return OwnerVirtualThreadId == ownerBinding.OwnerVirtualThreadId && OwnerContextId == ownerBinding.OwnerContextId && OwnerCoreId == ownerBinding.OwnerCoreId && OwnerPodId == ownerBinding.OwnerPodId && OwnerDomainTag == ownerBinding.OwnerDomainTag && DeviceId == ownerBinding.DeviceId;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeIssueAdmissionMetadata
//	{
//		public DmaStreamComputeIssueAdmissionMetadata(ulong issuingPc, ulong bundleId, byte slotIndex, byte laneIndex, ulong issueCycle, ulong replayEpoch)
//		{
//			IssuingPc = issuingPc;
//			BundleId = bundleId;
//			SlotIndex = slotIndex;
//			LaneIndex = laneIndex;
//			IssueCycle = issueCycle;
//			ReplayEpoch = replayEpoch;
//		}
//
//		public ulong IssuingPc
//		{
//			get;
//		}
//
//		public ulong BundleId
//		{
//			get;
//		}
//
//		public byte SlotIndex
//		{
//			get;
//		}
//
//		public byte LaneIndex
//		{
//			get;
//		}
//
//		public ulong IssueCycle
//		{
//			get;
//		}
//
//		public ulong ReplayEpoch
//		{
//			get;
//		}
//
//		public bool IsLane6IssuePlacement
//		{
//			get
//			{
//				return SlotIndex == 6 && LaneIndex == 6;
//			}
//		}
//
//		public static DmaStreamComputeIssueAdmissionMetadata Lane6(ulong issuingPc = 0, ulong bundleId = 0, ulong issueCycle = 0, ulong replayEpoch = 0)
//		{
//			return new(issuingPc, bundleId, slotIndex: 6, laneIndex: 6, issueCycle, replayEpoch);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeTokenStoreOptions
//	{
//		public DmaStreamComputeTokenStoreOptions(ushort activeTokenCapacity, ushort perDomainTokenQuota, ulong generation = 1, ulong firstTokenId = 1)
//		{
//			ActiveTokenCapacity = activeTokenCapacity == 0 ? (ushort)1 : activeTokenCapacity;
//			PerDomainTokenQuota = perDomainTokenQuota == 0 ? (ushort)1 : perDomainTokenQuota;
//			Generation = generation == 0 ? 1 : generation;
//			FirstTokenId = firstTokenId == 0 ? 1 : firstTokenId;
//		}
//
//		public ushort ActiveTokenCapacity
//		{
//			get;
//		}
//
//		public ushort PerDomainTokenQuota
//		{
//			get;
//		}
//
//		public ulong Generation
//		{
//			get;
//		}
//
//		public ulong FirstTokenId
//		{
//			get;
//		}
//
//		public static DmaStreamComputeTokenStoreOptions Default
//		{
//			get;
//		} = new(activeTokenCapacity: 16, perDomainTokenQuota: 16);
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeIssueAdmissionRequest
//	{
//		public DmaStreamComputeIssueAdmissionRequest(DmaStreamComputeMicroOp carrier, DmaStreamComputeOwnerGuardDecision issueGuardDecision, DmaStreamComputeIssueAdmissionMetadata metadata, DmaStreamComputePressurePolicy pressurePolicy, DmaStreamComputePressureSnapshot pressureSnapshot, System.Nullable<DmaStreamComputeTelemetryCounters> telemetry = nullptr)
//		{
//			ArgumentNullException.ThrowIfNull(carrier);
//			Carrier = carrier;
//			IssueGuardDecision = issueGuardDecision;
//			Metadata = metadata;
//			PressurePolicy = pressurePolicy;
//			PressureSnapshot = pressureSnapshot;
//			Telemetry = telemetry;
//		}
//
//		public DmaStreamComputeMicroOp Carrier
//		{
//			get;
//		}
//
//		public DmaStreamComputeOwnerGuardDecision IssueGuardDecision
//		{
//			get;
//		}
//
//		public DmaStreamComputeIssueAdmissionMetadata Metadata
//		{
//			get;
//		}
//
//		public DmaStreamComputePressurePolicy PressurePolicy
//		{
//			get;
//		}
//
//		public DmaStreamComputePressureSnapshot PressureSnapshot
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeTelemetryCounters> Telemetry
//		{
//			get;
//		}
//
//		public static DmaStreamComputeIssueAdmissionRequest ForLane6(DmaStreamComputeMicroOp carrier, DmaStreamComputeIssueAdmissionMetadata metadata, System.Nullable<DmaStreamComputeTelemetryCounters> telemetry = nullptr)
//		{
//			ArgumentNullException.ThrowIfNull(carrier);
//			DmaStreamComputePressurePolicy policy = DmaStreamComputePressurePolicy.Default;
//			return new DmaStreamComputeIssueAdmissionRequest(carrier, carrier.Descriptor.OwnerGuardDecision, metadata, policy, DmaStreamComputePressureSnapshot.Permissive(policy), telemetry);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeActiveTokenEntry
//	{
//		public DmaStreamComputeActiveTokenEntry(DmaStreamComputeTokenHandle handle, DmaStreamComputeToken token, DmaStreamComputeIssueAdmissionMetadata metadata, ulong descriptorIdentityHash, ulong normalizedFootprintHash)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			Handle = handle;
//			Token = token;
//			Metadata = metadata;
//			DescriptorIdentityHash = descriptorIdentityHash;
//			NormalizedFootprintHash = normalizedFootprintHash;
//		}
//
//		public DmaStreamComputeTokenHandle Handle
//		{
//			get;
//		}
//
//		public DmaStreamComputeToken Token
//		{
//			get;
//		}
//
//		public DmaStreamComputeIssueAdmissionMetadata Metadata
//		{
//			get;
//		}
//
//		public ulong DescriptorIdentityHash
//		{
//			get;
//		}
//
//		public ulong NormalizedFootprintHash
//		{
//			get;
//		}
//
//		public DmaStreamComputeTokenLifecycleEvidence ExportLifecycleEvidence()
//		{
//			return Token.ExportLifecycleEvidence();
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeIssueAdmissionResult
//	{
//		private DmaStreamComputeIssueAdmissionResult(DmaStreamComputeIssueAdmissionStatus status, DmaStreamComputeIssueAdmissionRejectKind rejectKind, DmaStreamComputeValidationFault validationFault, DmaStreamComputePressureRejectKind pressureRejectKind, System.Nullable<DmaStreamComputeActiveTokenEntry> entry, System.Nullable<DmaStreamComputeFaultRecord> fault, string message)
//		{
//			Status = status;
//			RejectKind = rejectKind;
//			ValidationFault = validationFault;
//			PressureRejectKind = pressureRejectKind;
//			Entry = entry;
//			Fault = fault;
//			Message = message;
//		}
//
//		public DmaStreamComputeIssueAdmissionStatus Status
//		{
//			get;
//		}
//
//		public DmaStreamComputeIssueAdmissionRejectKind RejectKind
//		{
//			get;
//		}
//
//		public DmaStreamComputeValidationFault ValidationFault
//		{
//			get;
//		}
//
//		public DmaStreamComputePressureRejectKind PressureRejectKind
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeActiveTokenEntry> Entry
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeToken> Token
//		{
//			get
//			{
//				return (Entry == nullptr ? nullptr : Entry.Token);
//			}
//		}
//
//		public DmaStreamComputeTokenHandle Handle
//		{
//			get
//			{
//				return (Entry == nullptr ? nullptr : Entry.Handle) ?? default;
//			}
//		}
//
//		public System.Nullable<DmaStreamComputeFaultRecord> Fault
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool IsAccepted
//		{
//			get
//			{
//				return Status == DmaStreamComputeIssueAdmissionStatus.Accepted && Entry != nullptr;
//			}
//		}
//
//		public bool HasAllocatedToken
//		{
//			get
//			{
//				return IsAccepted && !Handle.IsDefault;
//			}
//		}
//
//		public bool RequiresRetireExceptionPublication
//		{
//			get
//			{
//				return (Fault == nullptr ? nullptr : Fault.RequiresRetireExceptionPublication) == true;
//			}
//		}
//
//		public static DmaStreamComputeIssueAdmissionResult Accepted(DmaStreamComputeActiveTokenEntry entry)
//		{
//			return new(DmaStreamComputeIssueAdmissionStatus.Accepted, DmaStreamComputeIssueAdmissionRejectKind.None, DmaStreamComputeValidationFault.None, DmaStreamComputePressureRejectKind.None, entry, fault: nullptr, "DmaStreamCompute token allocated by explicit issue/admission store; execution remains gated.");
//		}
//
//		public static DmaStreamComputeIssueAdmissionResult Rejected(DmaStreamComputeIssueAdmissionRejectKind rejectKind, DmaStreamComputeValidationFault validationFault, DmaStreamComputePressureRejectKind pressureRejectKind, string message)
//		{
//			return new(DmaStreamComputeIssueAdmissionStatus.Rejected, rejectKind, validationFault, pressureRejectKind, entry: nullptr, fault: nullptr, message);
//		}
//
//		public static DmaStreamComputeIssueAdmissionResult ArchitecturalFault(DmaStreamComputeIssueAdmissionRejectKind rejectKind, DmaStreamComputeValidationFault validationFault, DmaStreamComputeFaultRecord fault, string message)
//		{
//			return new(DmaStreamComputeIssueAdmissionStatus.ArchitecturalFault, rejectKind, validationFault, DmaStreamComputePressureRejectKind.None, entry: nullptr, fault, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeTokenStatusSnapshot
//	{
//		public DmaStreamComputeTokenStatusSnapshot(DmaStreamComputeTokenHandle handle, DmaStreamComputeTokenState state, DmaStreamComputeTokenCancelReason cancelReason, DmaStreamComputeTokenFaultKind faultKind, int stagedWriteCount, bool hasCommitted, ulong descriptorIdentityHash, ulong normalizedFootprintHash, DmaStreamComputeTokenLifecycleEvidence lifecycleEvidence)
//		{
//			Handle = handle;
//			State = state;
//			CancelReason = cancelReason;
//			FaultKind = faultKind;
//			StagedWriteCount = stagedWriteCount;
//			HasCommitted = hasCommitted;
//			DescriptorIdentityHash = descriptorIdentityHash;
//			NormalizedFootprintHash = normalizedFootprintHash;
//			LifecycleEvidence = lifecycleEvidence;
//			EncodedStatusWord = EncodeStatusWord(state, cancelReason, faultKind, stagedWriteCount, hasCommitted);
//		}
//
//		public DmaStreamComputeTokenHandle Handle
//		{
//			get;
//		}
//
//		public ulong TokenId
//		{
//			get
//			{
//				return Handle.TokenId;
//			}
//		}
//
//		public DmaStreamComputeTokenState State
//		{
//			get;
//		}
//
//		public DmaStreamComputeTokenCancelReason CancelReason
//		{
//			get;
//		}
//
//		public DmaStreamComputeTokenFaultKind FaultKind
//		{
//			get;
//		}
//
//		public int StagedWriteCount
//		{
//			get;
//		}
//
//		public bool HasCommitted
//		{
//			get;
//		}
//
//		public ulong DescriptorIdentityHash
//		{
//			get;
//		}
//
//		public ulong NormalizedFootprintHash
//		{
//			get;
//		}
//
//		public DmaStreamComputeTokenLifecycleEvidence LifecycleEvidence
//		{
//			get;
//		}
//
//		public ulong EncodedStatusWord
//		{
//			get;
//		}
//
//		public static DmaStreamComputeTokenStatusSnapshot FromEntry(DmaStreamComputeActiveTokenEntry entry)
//		{
//			ArgumentNullException.ThrowIfNull(entry);
//			DmaStreamComputeToken token = entry.Token;
//			return new DmaStreamComputeTokenStatusSnapshot(entry.Handle, token.State, token.CancelReason, (token.LastFault == nullptr ? nullptr : token.LastFault.FaultKind) ?? DmaStreamComputeTokenFaultKind.None, token.StagedWriteCount, token.HasCommitted, entry.DescriptorIdentityHash, entry.NormalizedFootprintHash, token.ExportLifecycleEvidence());
//		}
//
//		private static ulong EncodeStatusWord(DmaStreamComputeTokenState state, DmaStreamComputeTokenCancelReason cancelReason, DmaStreamComputeTokenFaultKind faultKind, int stagedWriteCount, bool hasCommitted)
//		{
//			ulong encoded = (byte)state;
//			encoded |= (ulong)(byte)cancelReason << 8;
//			encoded |= (ulong)(byte)faultKind << 16;
//			encoded |= (hasCommitted ? 1UL : 0UL) << 24;
//			encoded |= ((ulong)(uint)stagedWriteCount) << 32;
//			return encoded;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeStatusQueryResult
//	{
//		private DmaStreamComputeStatusQueryResult(DmaStreamComputeStatusQueryStatus status, DmaStreamComputeStatusQueryRejectKind rejectKind, System.Nullable<DmaStreamComputeTokenStatusSnapshot> snapshot, string message)
//		{
//			Status = status;
//			RejectKind = rejectKind;
//			Snapshot = snapshot;
//			Message = string.IsNullOrWhiteSpace(message) ? "DSC_STATUS token query rejected." : message;
//		}
//
//		public DmaStreamComputeStatusQueryStatus Status
//		{
//			get;
//		}
//
//		public DmaStreamComputeStatusQueryRejectKind RejectKind
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeTokenStatusSnapshot> Snapshot
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool IsAccepted
//		{
//			get
//			{
//				return Status == DmaStreamComputeStatusQueryStatus.Accepted && Snapshot != nullptr;
//			}
//		}
//
//		public bool RequiresRetireExceptionPublication
//		{
//			get
//			{
//				return !IsAccepted;
//			}
//		}
//
//		public static DmaStreamComputeStatusQueryResult Accepted(DmaStreamComputeTokenStatusSnapshot snapshot)
//		{
//			return new(DmaStreamComputeStatusQueryStatus.Accepted, DmaStreamComputeStatusQueryRejectKind.None, snapshot, "DSC_STATUS captured store-owned token status for retire-owned writeback.");
//		}
//
//		public static DmaStreamComputeStatusQueryResult Rejected(DmaStreamComputeStatusQueryRejectKind rejectKind, string message)
//		{
//			return new(DmaStreamComputeStatusQueryStatus.Rejected, rejectKind, snapshot: nullptr, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeStatusReplayEvidence
//	{
//		public DmaStreamComputeStatusReplayEvidence(ushort opcode, ulong tokenId, DmaStreamComputeStatusQueryStatus status, DmaStreamComputeStatusQueryRejectKind rejectKind, DmaStreamComputeOwnerBinding ownerBinding, DmaStreamComputeTokenLifecycleEvidence tokenLifecycleEvidence, DmaStreamComputeLanePlacementEvidence lanePlacementEvidence, ulong encodedStatusWord)
//		{
//			Opcode = opcode;
//			TokenId = tokenId;
//			Status = status;
//			RejectKind = rejectKind;
//			OwnerBinding = ownerBinding;
//			TokenLifecycleEvidence = tokenLifecycleEvidence;
//			LanePlacementEvidence = lanePlacementEvidence;
//			EncodedStatusWord = encodedStatusWord;
//		}
//
//		public ushort Opcode
//		{
//			get;
//		}
//
//		public ulong TokenId
//		{
//			get;
//		}
//
//		public DmaStreamComputeStatusQueryStatus Status
//		{
//			get;
//		}
//
//		public DmaStreamComputeStatusQueryRejectKind RejectKind
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeOwnerBinding> OwnerBinding
//		{
//			get;
//		}
//
//		public DmaStreamComputeTokenLifecycleEvidence TokenLifecycleEvidence
//		{
//			get;
//		}
//
//		public DmaStreamComputeLanePlacementEvidence LanePlacementEvidence
//		{
//			get;
//		}
//
//		public ulong EncodedStatusWord
//		{
//			get;
//		}
//
//		public bool IsComplete
//		{
//			get
//			{
//				return Opcode == Processor.CPU_Core.IsaOpcodeValues.DSC_STATUS && Status == DmaStreamComputeStatusQueryStatus.Accepted && RejectKind == DmaStreamComputeStatusQueryRejectKind.None && OwnerBinding != nullptr && TokenLifecycleEvidence.IsComplete && LanePlacementEvidence.IsComplete;
//			}
//		}
//
//		public static DmaStreamComputeStatusReplayEvidence Capture(ulong tokenId, DmaStreamComputeOwnerBinding ownerBinding, DmaStreamComputeStatusQueryResult result, DmaStreamComputeLanePlacementEvidence lanePlacementEvidence)
//		{
//			ArgumentNullException.ThrowIfNull(ownerBinding);
//			ArgumentNullException.ThrowIfNull(result);
//
//			System.Nullable<DmaStreamComputeTokenStatusSnapshot> snapshot = result.Snapshot;
//			return new DmaStreamComputeStatusReplayEvidence(Processor.CPU_Core.IsaOpcodeValues.DSC_STATUS, tokenId, result.Status, result.RejectKind, ownerBinding, (snapshot == nullptr ? nullptr : snapshot.LifecycleEvidence) ?? default, lanePlacementEvidence, (snapshot == nullptr ? nullptr : snapshot.EncodedStatusWord) ?? 0UL);
//		}
//	}

	struct DmaStreamComputeTokenHandle final {
		unsigned long long TokenId = 0;
		unsigned short OwnerVirtualThreadId = 0;
		unsigned int OwnerContextId = 0, OwnerCoreId = 0, OwnerPodId = 0;
		unsigned long long OwnerDomainTag = 0;
		unsigned int DeviceId = 0;
		unsigned long long Generation = 0;
		bool IsDefault() const noexcept { return TokenId == 0 || Generation == 0; }
		bool MatchesOwner(const std::shared_ptr<DmaStreamComputeOwnerBinding>& owner) const {
			if (!owner) throw std::invalid_argument("ownerBinding");
			return OwnerVirtualThreadId == owner->OwnerVirtualThreadId && OwnerContextId == owner->OwnerContextId && OwnerCoreId == owner->OwnerCoreId && OwnerPodId == owner->OwnerPodId && OwnerDomainTag == owner->OwnerDomainTag && DeviceId == owner->DeviceId;
		}
		bool operator==(const DmaStreamComputeTokenHandle&) const = default;
	};
	struct DmaStreamComputeTokenHandleHash {
		size_t operator()(const std::shared_ptr<DmaStreamComputeTokenHandle>& h) const noexcept {
			if (!h) return 0;
			size_t seed = 0;
			auto mix = [&seed](unsigned long long v) { seed ^= std::hash<unsigned long long>{}(v) + static_cast<size_t>(0x9e3779b9U) + (seed << 6) + (seed >> 2); };
			mix(h->TokenId); mix(h->OwnerVirtualThreadId); mix(h->OwnerContextId); mix(h->OwnerCoreId); mix(h->OwnerPodId); mix(h->OwnerDomainTag); mix(h->DeviceId); mix(h->Generation);
			return seed;
		}
	};
	struct DmaStreamComputeTokenHandleEqual {
		bool operator()(const std::shared_ptr<DmaStreamComputeTokenHandle>& a, const std::shared_ptr<DmaStreamComputeTokenHandle>& b) const noexcept { return a == b || (a && b && *a == *b); }
	};
	struct DmaStreamComputeIssueAdmissionMetadata final {
		unsigned long long IssuingPc = 0, BundleId = 0;
		unsigned char SlotIndex = 0, LaneIndex = 0;
		unsigned long long IssueCycle = 0, ReplayEpoch = 0;
		bool IsLane6IssuePlacement() const noexcept { return SlotIndex == 6 && LaneIndex == 6; }
		static DmaStreamComputeIssueAdmissionMetadata Lane6(unsigned long long pc = 0, unsigned long long bundle = 0, unsigned long long cycle = 0, unsigned long long epoch = 0) { return {pc, bundle, 6, 6, cycle, epoch}; }
	};
	struct DmaStreamComputeActiveTokenEntry final {
		const DmaStreamComputeTokenHandle Handle;
		const std::shared_ptr<DmaStreamComputeToken> Token;
		const DmaStreamComputeIssueAdmissionMetadata Metadata;
		const unsigned long long DescriptorIdentityHash;
		const unsigned long long NormalizedFootprintHash;

		DmaStreamComputeActiveTokenEntry(DmaStreamComputeTokenHandle handle,
			std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeIssueAdmissionMetadata metadata,
			unsigned long long descriptorIdentityHash, unsigned long long normalizedFootprintHash)
			: Handle(std::move(handle)), Token(std::move(token)), Metadata(metadata),
			  DescriptorIdentityHash(descriptorIdentityHash), NormalizedFootprintHash(normalizedFootprintHash)
		{
			if (!Token) throw std::invalid_argument("token");
		}

		std::shared_ptr<DmaStreamComputeTokenLifecycleEvidence> ExportLifecycleEvidence() const
		{
			return Token->ExportLifecycleEvidence();
		}
	};
	struct DmaStreamComputeTokenStoreOptions final {
		unsigned short ActiveTokenCapacity, PerDomainTokenQuota;
		unsigned long long Generation, FirstTokenId;
		DmaStreamComputeTokenStoreOptions(unsigned short capacity, unsigned short quota, unsigned long long generation = 1, unsigned long long first = 1) : ActiveTokenCapacity(capacity ? capacity : 1), PerDomainTokenQuota(quota ? quota : 1), Generation(generation ? generation : 1), FirstTokenId(first ? first : 1) {}
		static const std::shared_ptr<DmaStreamComputeTokenStoreOptions> Default;
	};
	inline const std::shared_ptr<DmaStreamComputeTokenStoreOptions> DmaStreamComputeTokenStoreOptions::Default = std::make_shared<DmaStreamComputeTokenStoreOptions>(16, 16);
	class DmaStreamComputeIssueAdmissionRequest final {
	public:
		const std::shared_ptr<::YAKSys_Hybrid_CPU::Core::DmaStreamComputeMicroOp> Carrier;
		const std::shared_ptr<DmaStreamComputeOwnerGuardDecision> IssueGuardDecision;
		const DmaStreamComputeIssueAdmissionMetadata Metadata;
		const std::shared_ptr<DmaStreamComputePressurePolicy> PressurePolicy;
		const std::shared_ptr<DmaStreamComputePressureSnapshot> PressureSnapshot;
		const std::shared_ptr<DmaStreamComputeTelemetryCounters> Telemetry;

		DmaStreamComputeIssueAdmissionRequest(std::shared_ptr<::YAKSys_Hybrid_CPU::Core::DmaStreamComputeMicroOp> carrier,
			std::shared_ptr<DmaStreamComputeOwnerGuardDecision> issueGuardDecision,
			DmaStreamComputeIssueAdmissionMetadata metadata, std::shared_ptr<DmaStreamComputePressurePolicy> pressurePolicy,
			std::shared_ptr<DmaStreamComputePressureSnapshot> pressureSnapshot,
			std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);
		static std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> ForLane6(
			std::shared_ptr<::YAKSys_Hybrid_CPU::Core::DmaStreamComputeMicroOp> carrier, DmaStreamComputeIssueAdmissionMetadata metadata,
			std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);
	};

	class DmaStreamComputeIssueAdmissionResult final {
	private:
		DmaStreamComputeIssueAdmissionResult(DmaStreamComputeIssueAdmissionStatus status,
			DmaStreamComputeIssueAdmissionRejectKind rejectKind, DmaStreamComputeValidationFault validationFault,
			DmaStreamComputePressureRejectKind pressureRejectKind,
			std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry,
			std::shared_ptr<DmaStreamComputeFaultRecord> fault, std::wstring message);
	public:
		const DmaStreamComputeIssueAdmissionStatus Status;
		const DmaStreamComputeIssueAdmissionRejectKind RejectKind;
		const DmaStreamComputeValidationFault ValidationFault;
		const DmaStreamComputePressureRejectKind PressureRejectKind;
		const std::shared_ptr<DmaStreamComputeActiveTokenEntry> Entry;
		const std::shared_ptr<DmaStreamComputeToken> Token;
		const std::shared_ptr<DmaStreamComputeFaultRecord> Fault;
		const std::wstring Message;
		const DmaStreamComputeTokenHandle Handle;
		const bool IsAccepted;
		const bool HasAllocatedToken;
		const bool RequiresRetireExceptionPublication;

		static std::shared_ptr<DmaStreamComputeIssueAdmissionResult> Accepted(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry);
		static std::shared_ptr<DmaStreamComputeIssueAdmissionResult> Rejected(DmaStreamComputeIssueAdmissionRejectKind rejectKind,
			DmaStreamComputeValidationFault validationFault, DmaStreamComputePressureRejectKind pressureRejectKind, const std::wstring& message);
		static std::shared_ptr<DmaStreamComputeIssueAdmissionResult> ArchitecturalFault(DmaStreamComputeIssueAdmissionRejectKind rejectKind,
			DmaStreamComputeValidationFault validationFault, std::shared_ptr<DmaStreamComputeFaultRecord> fault, const std::wstring& message);
	};
	struct DmaStreamComputeTokenStatusSnapshot final
	{
		DmaStreamComputeTokenHandle Handle{};
		DmaStreamComputeTokenState State{};
		DmaStreamComputeTokenCancelReason CancelReason{};
		DmaStreamComputeTokenFaultKind FaultKind{};
		int StagedWriteCount = 0;
		bool HasCommitted = false;
		unsigned long long DescriptorIdentityHash = 0;
		unsigned long long NormalizedFootprintHash = 0;
		DmaStreamComputeTokenLifecycleEvidence LifecycleEvidence{};
		unsigned long long EncodedStatusWord = 0;

		DmaStreamComputeTokenStatusSnapshot(DmaStreamComputeTokenHandle handle,
			DmaStreamComputeTokenState state, DmaStreamComputeTokenCancelReason cancelReason,
			DmaStreamComputeTokenFaultKind faultKind, int stagedWriteCount, bool hasCommitted,
			unsigned long long descriptorIdentityHash, unsigned long long normalizedFootprintHash,
			DmaStreamComputeTokenLifecycleEvidence lifecycleEvidence)
			: Handle(std::move(handle)), State(state), CancelReason(cancelReason), FaultKind(faultKind),
			  StagedWriteCount(stagedWriteCount), HasCommitted(hasCommitted),
			  DescriptorIdentityHash(descriptorIdentityHash), NormalizedFootprintHash(normalizedFootprintHash),
			  LifecycleEvidence(std::move(lifecycleEvidence))
		{
			EncodedStatusWord = static_cast<unsigned char>(State)
				| (static_cast<unsigned long long>(static_cast<unsigned char>(CancelReason)) << 8)
				| (static_cast<unsigned long long>(static_cast<unsigned char>(FaultKind)) << 16)
				| (static_cast<unsigned long long>(HasCommitted ? 1 : 0) << 24)
				| (static_cast<unsigned long long>(static_cast<unsigned int>(StagedWriteCount)) << 32);
		}

		static std::shared_ptr<DmaStreamComputeTokenStatusSnapshot> FromEntry(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry)
		{
			if (!entry || !entry->Token) throw std::invalid_argument("entry");
			const auto lastFault = entry->Token->getLastFault();
			const auto faultKind = lastFault && *lastFault ? (*lastFault)->FaultKind : DmaStreamComputeTokenFaultKind::None;
			return std::make_shared<DmaStreamComputeTokenStatusSnapshot>(entry->Handle,
				entry->Token->getState(), entry->Token->getCancelReason(), faultKind,
				entry->Token->getStagedWriteCount(), entry->Token->getHasCommitted(),
				entry->DescriptorIdentityHash, entry->NormalizedFootprintHash,
				*entry->Token->ExportLifecycleEvidence());
		}
	};

	struct DmaStreamComputeStatusQueryResult final
	{
		DmaStreamComputeStatusQueryStatus Status{};
		DmaStreamComputeStatusQueryRejectKind RejectKind{};
		std::shared_ptr<DmaStreamComputeTokenStatusSnapshot> Snapshot;
		std::wstring Message;
		bool IsAccepted = false;
		bool RequiresRetireExceptionPublication = true;

		DmaStreamComputeStatusQueryResult(DmaStreamComputeStatusQueryStatus status,
			DmaStreamComputeStatusQueryRejectKind rejectKind,
			std::shared_ptr<DmaStreamComputeTokenStatusSnapshot> snapshot, std::wstring message)
			: Status(status), RejectKind(rejectKind), Snapshot(std::move(snapshot)),
		  Message(IsNullOrWhiteSpace(message) ? L"DSC_STATUS token query rejected." : std::move(message)),
		  IsAccepted(Status == DmaStreamComputeStatusQueryStatus::Accepted && Snapshot != nullptr),
		  RequiresRetireExceptionPublication(!IsAccepted) {}
		static bool IsNullOrWhiteSpace(const std::wstring &value) noexcept
		{
			for (wchar_t ch : value)
			{
				const bool whitespace = (ch >= L'\t' && ch <= L'\r') || ch == L' '
					|| ch == 0x0085 || ch == 0x00A0 || ch == 0x1680
					|| (ch >= 0x2000 && ch <= 0x200A) || ch == 0x2028 || ch == 0x2029
					|| ch == 0x202F || ch == 0x205F || ch == 0x3000;
				if (!whitespace) return false;
			}
			return true;
		}
		static std::shared_ptr<DmaStreamComputeStatusQueryResult> Accepted(std::shared_ptr<DmaStreamComputeTokenStatusSnapshot> snapshot)
		{
			if (!snapshot) throw std::invalid_argument("snapshot");
			return std::make_shared<DmaStreamComputeStatusQueryResult>(DmaStreamComputeStatusQueryStatus::Accepted,
				DmaStreamComputeStatusQueryRejectKind::None, std::move(snapshot), L"DSC_STATUS captured store-owned token status for retire-owned writeback.");
		}
		static std::shared_ptr<DmaStreamComputeStatusQueryResult> Rejected(DmaStreamComputeStatusQueryRejectKind rejectKind, std::wstring message)
		{
			return std::make_shared<DmaStreamComputeStatusQueryResult>(DmaStreamComputeStatusQueryStatus::Rejected,
				rejectKind, nullptr, std::move(message));
		}
	};

	struct DmaStreamComputeStatusReplayEvidence final
	{
		unsigned short Opcode = 0;
		unsigned long long TokenId = 0;
		DmaStreamComputeStatusQueryStatus Status{};
		DmaStreamComputeStatusQueryRejectKind RejectKind{};
		std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		DmaStreamComputeTokenLifecycleEvidence TokenLifecycleEvidence{};
		DmaStreamComputeLanePlacementEvidence LanePlacementEvidence{};
		unsigned long long EncodedStatusWord = 0;

		bool IsComplete() const noexcept
		{
			return Opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS && Status == DmaStreamComputeStatusQueryStatus::Accepted
				&& RejectKind == DmaStreamComputeStatusQueryRejectKind::None && OwnerBinding
				&& TokenLifecycleEvidence.IsComplete() && LanePlacementEvidence.IsComplete();
		}
		static std::shared_ptr<DmaStreamComputeStatusReplayEvidence> Capture(unsigned long long tokenId,
			std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding,
			std::shared_ptr<DmaStreamComputeStatusQueryResult> result,
			DmaStreamComputeLanePlacementEvidence lanePlacementEvidence)
		{
			if (!ownerBinding) throw std::invalid_argument("ownerBinding");
			if (!result) throw std::invalid_argument("result");
			DmaStreamComputeTokenLifecycleEvidence lifecycleEvidence{};
			unsigned long long statusWord = 0;
			if (result->Snapshot)
			{
				lifecycleEvidence = result->Snapshot->LifecycleEvidence;
				statusWord = result->Snapshot->EncodedStatusWord;
			}
			auto evidence = std::make_shared<DmaStreamComputeStatusReplayEvidence>();
			evidence->Opcode = Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS;
			evidence->TokenId = tokenId;
			evidence->Status = result->Status;
			evidence->RejectKind = result->RejectKind;
			evidence->OwnerBinding = std::move(ownerBinding);
			evidence->TokenLifecycleEvidence = std::move(lifecycleEvidence);
			evidence->LanePlacementEvidence = lanePlacementEvidence;
			evidence->EncodedStatusWord = statusWord;
			return evidence;
		}
	};

	class DmaStreamComputeTokenStore final : public std::enable_shared_from_this<DmaStreamComputeTokenStore>
	{
	private:
		const std::shared_ptr<DmaStreamComputeTokenStoreOptions> Options;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<DmaStreamComputeTokenHandle, DmaStreamComputeActiveTokenEntry> _activeTokens = new();
		std::unordered_map<std::shared_ptr<DmaStreamComputeTokenHandle>, std::shared_ptr<DmaStreamComputeActiveTokenEntry>, DmaStreamComputeTokenHandleHash, DmaStreamComputeTokenHandleEqual> _activeTokens;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, int> _activeTokensByDomain = new();
		std::unordered_map<unsigned long long, int> _activeTokensByDomain = std::unordered_map<unsigned long long, int>();
		unsigned long long _nextTokenId = 0;

	public:
		DmaStreamComputeTokenStore();

		DmaStreamComputeTokenStore(std::shared_ptr<DmaStreamComputeTokenStoreOptions> options);

		std::shared_ptr<DmaStreamComputeTokenStoreOptions> getOptions() const;

		int getActiveTokenCount() const;

		unsigned long long getGeneration() const;

		std::shared_ptr<DmaStreamComputeIssueAdmissionResult> TryAllocateAtIssueAdmission(std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> request);

		bool TryGet(std::shared_ptr<DmaStreamComputeTokenHandle> handle, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, std::optional<std::shared_ptr<DmaStreamComputeActiveTokenEntry>> &entry);

		std::shared_ptr<DmaStreamComputeStatusQueryResult> QueryStatusByTokenId(unsigned long long tokenId, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding);

		bool TryCancel(std::shared_ptr<DmaStreamComputeTokenHandle> handle, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, DmaStreamComputeTokenCancelReason reason);

		std::vector<std::shared_ptr<DmaStreamComputeTokenHandle>> SnapshotActiveHandles();

	private:
		std::shared_ptr<DmaStreamComputeTokenHandle> AllocateHandle(std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding);

		int GetDomainActiveCount(unsigned long long ownerDomainTag);

		static bool TryValidateIssueGuard(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> issueGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> &result);

		static bool TryPassPressure(std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> request, std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> &result);

		static bool RejectPressure(std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> request, DmaStreamComputePressureRejectKind pressureRejectKind, DmaStreamComputeValidationFault validationFault, const std::wstring &message, std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> &result);
	};
}

