#pragma once

#include "DmaStreamComputeValidationResult.h"
#include "DmaStreamComputeToken.h"
#include "../../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "DmaStreamComputeDescriptor.h"
#include "../../Memory/IHardwareOccupancyInput.h"
#include <string>
#include <unordered_set>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputePressureRejectKind : unsigned char
	{
		None = 0,
		Lane6Unavailable = 1,
		DmaCredits = 2,
		SrfCredits = 3,
		MemorySubsystemPressure = 4,
		OutstandingTokenCap = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputePressurePolicy
//	{
//		public DmaStreamComputePressurePolicy(byte requiredDmaCredits, ushort requiredSrfCredits, byte requiredMemoryCredits, ushort outstandingTokenCap)
//		{
//			RequiredDmaCredits = requiredDmaCredits == 0 ? (byte)1 : requiredDmaCredits;
//			RequiredSrfCredits = requiredSrfCredits == 0 ? (ushort)1 : requiredSrfCredits;
//			RequiredMemoryCredits = requiredMemoryCredits == 0 ? (byte)1 : requiredMemoryCredits;
//			OutstandingTokenCap = outstandingTokenCap == 0 ? (ushort)1 : outstandingTokenCap;
//		}
//
//		public byte RequiredDmaCredits
//		{
//			get;
//		}
//
//		public ushort RequiredSrfCredits
//		{
//			get;
//		}
//
//		public byte RequiredMemoryCredits
//		{
//			get;
//		}
//
//		public ushort OutstandingTokenCap
//		{
//			get;
//		}
//
//		public static DmaStreamComputePressurePolicy Default
//		{
//			get;
//		} = new(requiredDmaCredits: 1, requiredSrfCredits: 1, requiredMemoryCredits: 1, outstandingTokenCap: 16);
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputePressureSnapshot
//	{
//		public DmaStreamComputePressureSnapshot(bool lane6Available, byte dmaCreditsAvailable, ushort srfCreditsAvailable, byte memorySubsystemCreditsAvailable, ushort outstandingTokens)
//		{
//			Lane6Available = lane6Available;
//			DmaCreditsAvailable = dmaCreditsAvailable;
//			SrfCreditsAvailable = srfCreditsAvailable;
//			MemorySubsystemCreditsAvailable = memorySubsystemCreditsAvailable;
//			OutstandingTokens = outstandingTokens;
//		}
//
//		public bool Lane6Available
//		{
//			get;
//		}
//
//		public byte DmaCreditsAvailable
//		{
//			get;
//		}
//
//		public ushort SrfCreditsAvailable
//		{
//			get;
//		}
//
//		public byte MemorySubsystemCreditsAvailable
//		{
//			get;
//		}
//
//		public ushort OutstandingTokens
//		{
//			get;
//		}
//
//		public static DmaStreamComputePressureSnapshot Permissive(DmaStreamComputePressurePolicy policy)
//		{
//			return new(lane6Available: true, dmaCreditsAvailable: policy.RequiredDmaCredits, srfCreditsAvailable: policy.RequiredSrfCredits, memorySubsystemCreditsAvailable: policy.RequiredMemoryCredits, outstandingTokens: 0);
//		}
//
//		public static DmaStreamComputePressureSnapshot FromHardwareOccupancy(HardwareOccupancySnapshot128 snapshot, bool lane6Available = true, byte dmaCreditsAvailable = 1, ushort srfCreditsAvailable = 1, ushort outstandingTokens = 0)
//		{
//			return new DmaStreamComputePressureSnapshot(lane6Available, dmaCreditsAvailable, srfCreditsAvailable, snapshot.MemoryIssueBudget, outstandingTokens);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeTelemetrySnapshot
//	{
//		public long DescriptorParseAttempts
//		{
//			get;
//			set;
//		}
//
//		public long DescriptorAccepted
//		{
//			get;
//			set;
//		}
//
//		public long DescriptorRejected
//		{
//			get;
//			set;
//		}
//
//		public long ComputeAccepted
//		{
//			get;
//			set;
//		}
//
//		public long ComputeActive
//		{
//			get;
//			set;
//		}
//
//		public long ComputeStaged
//		{
//			get;
//			set;
//		}
//
//		public long ComputeCommitted
//		{
//			get;
//			set;
//		}
//
//		public long ComputeCanceled
//		{
//			get;
//			set;
//		}
//
//		public long ComputeFaulted
//		{
//			get;
//			set;
//		}
//
//		public long ComputeRejected
//		{
//			get;
//			set;
//		}
//
//		public long DescriptorFaults
//		{
//			get;
//			set;
//		}
//
//		public long OwnerDomainFaults
//		{
//			get;
//			set;
//		}
//
//		public long DeviceFaults
//		{
//			get;
//			set;
//		}
//
//		public long TokenFaults
//		{
//			get;
//			set;
//		}
//
//		public long QuotaRejects
//		{
//			get;
//			set;
//		}
//
//		public long BackpressureRejects
//		{
//			get;
//			set;
//		}
//
//		public long Lane6BackpressureRejects
//		{
//			get;
//			set;
//		}
//
//		public long DmaCreditRejects
//		{
//			get;
//			set;
//		}
//
//		public long SrfCreditRejects
//		{
//			get;
//			set;
//		}
//
//		public long MemorySubsystemPressureRejects
//		{
//			get;
//			set;
//		}
//
//		public long OutstandingTokenCapRejects
//		{
//			get;
//			set;
//		}
//
//		public long UnsupportedCarrierRejects
//		{
//			get;
//			set;
//		}
//
//		public long ReplayEnvelopeReuseHits
//		{
//			get;
//			set;
//		}
//
//		public long ReplayEnvelopeRejects
//		{
//			get;
//			set;
//		}
//
//		public ulong BytesRead
//		{
//			get;
//			set;
//		}
//
//		public ulong BytesStaged
//		{
//			get;
//			set;
//		}
//
//		public ulong ElementOperations
//		{
//			get;
//			set;
//		}
//
//		public ulong CopyOperations
//		{
//			get;
//			set;
//		}
//
//		public ulong AddOperations
//		{
//			get;
//			set;
//		}
//
//		public ulong MulOperations
//		{
//			get;
//			set;
//		}
//
//		public ulong FmaOperations
//		{
//			get;
//			set;
//		}
//
//		public ulong ReduceOperations
//		{
//			get;
//			set;
//		}
//
//		public DmaStreamComputeValidationFault LastValidationFault
//		{
//			get;
//			set;
//		}
//
//		public DmaStreamComputeTokenFaultKind LastTokenFaultKind
//		{
//			get;
//			set;
//		}
//
//		public DmaStreamComputePressureRejectKind LastPressureRejectKind
//		{
//			get;
//			set;
//		}
//
//		public ReplayPhaseInvalidationReason LastReplayInvalidationReason
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<string> LastReplayMismatchField
//		{
//			get;
//			set;
//		}
//
//		public bool LastLane6Available
//		{
//			get;
//			set;
//		}
//
//		public byte LastDmaCreditsAvailable
//		{
//			get;
//			set;
//		}
//
//		public ushort LastSrfCreditsAvailable
//		{
//			get;
//			set;
//		}
//
//		public byte LastMemorySubsystemCreditsAvailable
//		{
//			get;
//			set;
//		}
//
//		public ushort LastOutstandingTokens
//		{
//			get;
//			set;
//		}
//
//		public ushort LastOutstandingTokenCap
//		{
//			get;
//			set;
//		}
//	}

		struct DmaStreamComputePressurePolicy final {
		unsigned char RequiredDmaCredits;
		unsigned short RequiredSrfCredits;
		unsigned char RequiredMemoryCredits;
		unsigned short OutstandingTokenCap;
		DmaStreamComputePressurePolicy(unsigned char dma, unsigned short srf, unsigned char memory, unsigned short cap)
			: RequiredDmaCredits(dma ? dma : 1), RequiredSrfCredits(srf ? srf : 1), RequiredMemoryCredits(memory ? memory : 1), OutstandingTokenCap(cap ? cap : 1) {}
		static const std::shared_ptr<DmaStreamComputePressurePolicy> Default;
	};
	inline const std::shared_ptr<DmaStreamComputePressurePolicy> DmaStreamComputePressurePolicy::Default = std::make_shared<DmaStreamComputePressurePolicy>(1, 1, 1, 16);
	struct DmaStreamComputePressureSnapshot final {
		bool Lane6Available;
		unsigned char DmaCreditsAvailable;
		unsigned short SrfCreditsAvailable;
		unsigned char MemorySubsystemCreditsAvailable;
		unsigned short OutstandingTokens;
		static std::shared_ptr<DmaStreamComputePressureSnapshot> Permissive(const std::shared_ptr<DmaStreamComputePressurePolicy>& policy) {
			return std::make_shared<DmaStreamComputePressureSnapshot>(true, policy->RequiredDmaCredits, policy->RequiredSrfCredits, policy->RequiredMemoryCredits, 0);
		}
		static std::shared_ptr<DmaStreamComputePressureSnapshot> FromHardwareOccupancy(const YAKSys_Hybrid_CPU::Core::Memory::HardwareOccupancySnapshot128& snapshot, bool lane6Available = true, unsigned char dma = 1, unsigned short srf = 1, unsigned short outstanding = 0) {
			return std::make_shared<DmaStreamComputePressureSnapshot>(lane6Available, dma, srf, snapshot.getMemoryIssueBudget(), outstanding);
		}
	};
	struct DmaStreamComputeTelemetrySnapshot final {
		long long DescriptorParseAttempts{};
		long long DescriptorAccepted{};
		long long DescriptorRejected{};
		long long ComputeAccepted{};
		long long ComputeActive{};
		long long ComputeStaged{};
		long long ComputeCommitted{};
		long long ComputeCanceled{};
		long long ComputeFaulted{};
		long long ComputeRejected{};
		long long DescriptorFaults{};
		long long OwnerDomainFaults{};
		long long DeviceFaults{};
		long long TokenFaults{};
		long long QuotaRejects{};
		long long BackpressureRejects{};
		long long Lane6BackpressureRejects{};
		long long DmaCreditRejects{};
		long long SrfCreditRejects{};
		long long MemorySubsystemPressureRejects{};
		long long OutstandingTokenCapRejects{};
		long long UnsupportedCarrierRejects{};
		long long ReplayEnvelopeReuseHits{};
		long long ReplayEnvelopeRejects{};
		unsigned long long BytesRead{};
		unsigned long long BytesStaged{};
		unsigned long long ElementOperations{};
		unsigned long long CopyOperations{};
		unsigned long long AddOperations{};
		unsigned long long MulOperations{};
		unsigned long long FmaOperations{};
		unsigned long long ReduceOperations{};
		DmaStreamComputeValidationFault LastValidationFault{};
		DmaStreamComputeTokenFaultKind LastTokenFaultKind{};
		DmaStreamComputePressureRejectKind LastPressureRejectKind{};
		ReplayPhaseInvalidationReason LastReplayInvalidationReason{};
		std::wstring LastReplayMismatchField{};
		bool LastLane6Available{};
		unsigned char LastDmaCreditsAvailable{};
		unsigned short LastSrfCreditsAvailable{};
		unsigned char LastMemorySubsystemCreditsAvailable{};
		unsigned short LastOutstandingTokens{};
		unsigned short LastOutstandingTokenCap{};
	};
	struct DmaStreamComputeReplayEvidenceComparison;

	class DmaStreamComputeTelemetryCounters final : public std::enable_shared_from_this<DmaStreamComputeTelemetryCounters>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _acceptedTokens = new();
		std::unordered_set<unsigned long long> _acceptedTokens = std::unordered_set<unsigned long long>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _activeTokens = new();
		std::unordered_set<unsigned long long> _activeTokens = std::unordered_set<unsigned long long>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _stagedTokens = new();
		std::unordered_set<unsigned long long> _stagedTokens = std::unordered_set<unsigned long long>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _committedTokens = new();
		std::unordered_set<unsigned long long> _committedTokens = std::unordered_set<unsigned long long>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _canceledTokens = new();
		std::unordered_set<unsigned long long> _canceledTokens = std::unordered_set<unsigned long long>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _faultedTokens = new();
		std::unordered_set<unsigned long long> _faultedTokens = std::unordered_set<unsigned long long>();

		long long _descriptorParseAttempts = 0;
		long long _descriptorAccepted = 0;
		long long _descriptorRejected = 0;
		long long _computeRejected = 0;
		long long _descriptorFaults = 0;
		long long _ownerDomainFaults = 0;
		long long _deviceFaults = 0;
		long long _tokenFaults = 0;
		long long _quotaRejects = 0;
		long long _backpressureRejects = 0;
		long long _lane6BackpressureRejects = 0;
		long long _dmaCreditRejects = 0;
		long long _srfCreditRejects = 0;
		long long _memorySubsystemPressureRejects = 0;
		long long _outstandingTokenCapRejects = 0;
		long long _unsupportedCarrierRejects = 0;
		long long _replayEnvelopeReuseHits = 0;
		long long _replayEnvelopeRejects = 0;
		unsigned long long _bytesRead = 0;
		unsigned long long _bytesStaged = 0;
		unsigned long long _elementOperations = 0;
		unsigned long long _copyOperations = 0;
		unsigned long long _addOperations = 0;
		unsigned long long _mulOperations = 0;
		unsigned long long _fmaOperations = 0;
		unsigned long long _reduceOperations = 0;
		DmaStreamComputeValidationFault _lastValidationFault = static_cast<DmaStreamComputeValidationFault>(0);
		DmaStreamComputeTokenFaultKind _lastTokenFaultKind = static_cast<DmaStreamComputeTokenFaultKind>(0);
		DmaStreamComputePressureRejectKind _lastPressureRejectKind = static_cast<DmaStreamComputePressureRejectKind>(0);
		ReplayPhaseInvalidationReason _lastReplayInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(0);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private string? _lastReplayMismatchField;
		std::wstring _lastReplayMismatchField;
		bool _lastLane6Available = false;
		unsigned char _lastDmaCreditsAvailable = 0;
		unsigned short _lastSrfCreditsAvailable = 0;
		unsigned char _lastMemorySubsystemCreditsAvailable = 0;
		unsigned short _lastOutstandingTokens = 0;
		unsigned short _lastOutstandingTokenCap = 0;

	public:
		std::shared_ptr<DmaStreamComputeTelemetrySnapshot> Snapshot();

		void RecordDescriptorParseAttempt();

		void RecordDescriptorAccepted(std::shared_ptr<DmaStreamComputeDescriptor> descriptor);

		void RecordDescriptorRejected(DmaStreamComputeValidationFault fault);

		void RecordOwnerGuardRejected(std::shared_ptr<DmaStreamComputeOwnerBinding> structuralOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision);

		void RecordPressureSnapshot(std::shared_ptr<DmaStreamComputePressurePolicy> policy, std::shared_ptr<DmaStreamComputePressureSnapshot> snapshot);

		void RecordAdmissionReject(DmaStreamComputePressureRejectKind rejectKind, DmaStreamComputeValidationFault validationFault);

		void RecordTokenAccepted(std::shared_ptr<DmaStreamComputeToken> token);

		void RecordTokenActive(std::shared_ptr<DmaStreamComputeToken> token);

		void RecordTokenStaged(std::shared_ptr<DmaStreamComputeToken> token, unsigned long long byteCount);

		void RecordTokenCanceled(std::shared_ptr<DmaStreamComputeToken> token);

		void RecordTokenCommitted(std::shared_ptr<DmaStreamComputeToken> token);

		void RecordTokenFaulted(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeTokenFaultKind faultKind);

		void RecordTokenAdmissionFault(DmaStreamComputeValidationFault validationFault);

		void RecordRuntimeRead(unsigned long long byteCount);

		void RecordElementOperations(DmaStreamComputeOperationKind operation, unsigned long long elementCount);

		void RecordReplayEvidenceComparison(std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> comparison);

	private:
		void RecordValidationFaultFamily(DmaStreamComputeValidationFault fault);

		void RecordTokenFaultFamily(DmaStreamComputeTokenFaultKind faultKind);
	};

	class DmaStreamComputeAdmissionController final
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeTokenAdmissionResult TryAdmit(DmaStreamComputeValidationResult validationResult, ulong tokenId, DmaStreamComputeTelemetryCounters? telemetry)
		static std::shared_ptr<DmaStreamComputeTokenAdmissionResult> TryAdmit(std::shared_ptr<DmaStreamComputeValidationResult> validationResult, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeTokenAdmissionResult TryAdmit(DmaStreamComputeValidationResult validationResult, ulong tokenId, DmaStreamComputePressurePolicy pressurePolicy, DmaStreamComputePressureSnapshot pressureSnapshot, DmaStreamComputeTelemetryCounters? telemetry)
		static std::shared_ptr<DmaStreamComputeTokenAdmissionResult> TryAdmit(std::shared_ptr<DmaStreamComputeValidationResult> validationResult, unsigned long long tokenId, std::shared_ptr<DmaStreamComputePressurePolicy> pressurePolicy, std::shared_ptr<DmaStreamComputePressureSnapshot> pressureSnapshot, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry);

	private:
		static bool TryPassPressure(std::shared_ptr<DmaStreamComputePressurePolicy> policy, std::shared_ptr<DmaStreamComputePressureSnapshot> snapshot, DmaStreamComputePressureRejectKind &rejectKind, DmaStreamComputeValidationFault &validationFault, std::wstring &message);

		static bool Reject(DmaStreamComputePressureRejectKind reject, DmaStreamComputeValidationFault fault, const std::wstring &failureMessage, DmaStreamComputePressureRejectKind &rejectKind, DmaStreamComputeValidationFault &validationFault, std::wstring &message);
	};
}
