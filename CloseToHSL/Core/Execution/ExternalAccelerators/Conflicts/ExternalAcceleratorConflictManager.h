#pragma once

#include "../Tokens/AcceleratorToken.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../Descriptors/AcceleratorCommandDescriptor.h"
#include <algorithm>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <format>
#include <memory>
#include <stdexcept>
#include <utility>
#include <cwctype>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { struct AcceleratorMemoryRange; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry { class AcceleratorTelemetry; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts { class AcceleratorFootprintReservation; class AcceleratorConflictDecision; }

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts
{

	enum class AcceleratorConflictClass : unsigned char
	{
		None = 0,
		IncompleteFootprintTruth = 1,
		SubmitReservation = 2,
		CpuStoreOverlapsAcceleratorRead = 3,
		CpuStoreOverlapsAcceleratorWrite = 4,
		CpuLoadOverlapsAcceleratorWrite = 5,
		DmaStreamComputeOverlapsAcceleratorWrite = 6,
		AcceleratorWriteOverlapsSrfWarmedWindow = 7,
		AssistIngressOverlapsAcceleratorWrite = 8,
		AcceleratorWriteWriteOverlap = 9,
		FenceOrSerializingBoundaryWhileTokenActive = 10,
		VmDomainMappingTransitionWhileTokenActive = 11,
		CommitFootprintReservationMissing = 12,
		CommitConflictValidationRejected = 13
	};

	inline const wchar_t *AcceleratorConflictClassName(AcceleratorConflictClass value) noexcept
	{
		switch (value)
		{
		case AcceleratorConflictClass::None: return L"None";
		case AcceleratorConflictClass::IncompleteFootprintTruth: return L"IncompleteFootprintTruth";
		case AcceleratorConflictClass::SubmitReservation: return L"SubmitReservation";
		case AcceleratorConflictClass::CpuStoreOverlapsAcceleratorRead: return L"CpuStoreOverlapsAcceleratorRead";
		case AcceleratorConflictClass::CpuStoreOverlapsAcceleratorWrite: return L"CpuStoreOverlapsAcceleratorWrite";
		case AcceleratorConflictClass::CpuLoadOverlapsAcceleratorWrite: return L"CpuLoadOverlapsAcceleratorWrite";
		case AcceleratorConflictClass::DmaStreamComputeOverlapsAcceleratorWrite: return L"DmaStreamComputeOverlapsAcceleratorWrite";
		case AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow: return L"AcceleratorWriteOverlapsSrfWarmedWindow";
		case AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite: return L"AssistIngressOverlapsAcceleratorWrite";
		case AcceleratorConflictClass::AcceleratorWriteWriteOverlap: return L"AcceleratorWriteWriteOverlap";
		case AcceleratorConflictClass::FenceOrSerializingBoundaryWhileTokenActive: return L"FenceOrSerializingBoundaryWhileTokenActive";
		case AcceleratorConflictClass::VmDomainMappingTransitionWhileTokenActive: return L"VmDomainMappingTransitionWhileTokenActive";
		case AcceleratorConflictClass::CommitFootprintReservationMissing: return L"CommitFootprintReservationMissing";
		case AcceleratorConflictClass::CommitConflictValidationRejected: return L"CommitConflictValidationRejected";
		default: return L"Unknown";
		}
	}

	enum class AcceleratorConflictDecisionKind : unsigned char
	{
		Accepted = 0,
		Serialize = 1,
		Rejected = 2,
		Fault = 3
	};

	class AcceleratorConflictFault final
	{
	public:
		const AcceleratorConflictClass ConflictClass;
		const AcceleratorTokenFaultCode TokenFaultCode;
		const std::wstring Message;
		const unsigned long long FaultAddress;
		const bool IsWrite;
		const std::shared_ptr<AcceleratorTokenHandle> ConflictingTokenHandle;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;

		AcceleratorConflictFault(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode tokenFaultCode,
			const std::wstring &message, unsigned long long faultAddress = 0, bool isWrite = false,
			std::shared_ptr<AcceleratorTokenHandle> conflictingTokenHandle = nullptr,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision = nullptr)
			: ConflictClass(conflictClass), TokenFaultCode(tokenFaultCode),
		  Message(std::all_of(message.begin(), message.end(), [](wchar_t ch) { return std::iswspace(ch) != 0; })
			? std::format(L"L7-SDC conflict rejected: {}.", AcceleratorConflictClassName(conflictClass)) : message),
			  FaultAddress(faultAddress), IsWrite(isWrite), ConflictingTokenHandle(std::move(conflictingTokenHandle)),
			  GuardDecision(std::move(guardDecision))
		{
			if (conflictClass == AcceleratorConflictClass::None)
				throw std::invalid_argument("L7-SDC conflict faults require a non-None conflict class.");
			if (tokenFaultCode == AcceleratorTokenFaultCode::None)
				throw std::invalid_argument("L7-SDC conflict faults require a non-None token fault code.");
		}
	};

	class AcceleratorConflictDecision final
	{
	public:
		const AcceleratorConflictDecisionKind Kind;
		const AcceleratorConflictClass ConflictClass;
		const std::shared_ptr<AcceleratorFootprintReservation> Reservation;
		const std::shared_ptr<AcceleratorConflictFault> Fault;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const std::wstring Message;
		const bool IsAccepted;
		const bool RequiresSerialization;
		const bool IsRejected;
		const bool ShouldFaultToken;
		const AcceleratorTokenFaultCode TokenFaultCode;
		const bool CanPublishArchitecturalMemory = false;
		const bool UserVisiblePublicationAllowed = false;

		static std::shared_ptr<AcceleratorConflictDecision> Accepted(AcceleratorConflictClass conflictClass,
			const std::wstring &message, std::shared_ptr<AcceleratorFootprintReservation> reservation = nullptr,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision = nullptr)
		{
			return std::shared_ptr<AcceleratorConflictDecision>(new AcceleratorConflictDecision(
				AcceleratorConflictDecisionKind::Accepted, conflictClass, std::move(reservation), nullptr,
				std::move(guardDecision), message));
		}

		static std::shared_ptr<AcceleratorConflictDecision> Serialize(AcceleratorConflictClass conflictClass,
			std::shared_ptr<AcceleratorFootprintReservation> reservation,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision,
			std::shared_ptr<AcceleratorMemoryRange> conflictRange, const std::wstring &message, bool isWrite = true);

		static std::shared_ptr<AcceleratorConflictDecision> Reject(AcceleratorConflictClass conflictClass,
			AcceleratorTokenFaultCode tokenFaultCode, const std::wstring &message,
			std::shared_ptr<AcceleratorMemoryRange> conflictRange = nullptr, bool isWrite = false,
			std::shared_ptr<AcceleratorTokenHandle> conflictingTokenHandle = nullptr,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision = nullptr)
		{
			auto fault = std::make_shared<AcceleratorConflictFault>(conflictClass, tokenFaultCode, message,
				conflictRange ? conflictRange->Address : 0, isWrite, std::move(conflictingTokenHandle), guardDecision);
			return std::shared_ptr<AcceleratorConflictDecision>(new AcceleratorConflictDecision(
				AcceleratorConflictDecisionKind::Rejected, conflictClass, nullptr, std::move(fault),
				std::move(guardDecision), message));
		}

		static std::shared_ptr<AcceleratorConflictDecision> Faulted(AcceleratorConflictClass conflictClass,
			AcceleratorTokenFaultCode tokenFaultCode, const std::wstring &message,
			std::shared_ptr<AcceleratorMemoryRange> conflictRange = nullptr, bool isWrite = false,
			std::shared_ptr<AcceleratorTokenHandle> conflictingTokenHandle = nullptr,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision = nullptr)
		{
			auto fault = std::make_shared<AcceleratorConflictFault>(conflictClass, tokenFaultCode, message,
				conflictRange ? conflictRange->Address : 0, isWrite, std::move(conflictingTokenHandle), guardDecision);
			return std::shared_ptr<AcceleratorConflictDecision>(new AcceleratorConflictDecision(
				AcceleratorConflictDecisionKind::Fault, conflictClass, nullptr, std::move(fault),
				std::move(guardDecision), message));
		}

	private:
		AcceleratorConflictDecision(AcceleratorConflictDecisionKind kind, AcceleratorConflictClass conflictClass,
			std::shared_ptr<AcceleratorFootprintReservation> reservation,
			std::shared_ptr<AcceleratorConflictFault> fault,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision, std::wstring message)
			: Kind(kind), ConflictClass(conflictClass), Reservation(std::move(reservation)), Fault(std::move(fault)),
			  GuardDecision(std::move(guardDecision)), Message(std::move(message)),
			  IsAccepted(kind == AcceleratorConflictDecisionKind::Accepted),
			  RequiresSerialization(kind == AcceleratorConflictDecisionKind::Serialize),
			  IsRejected(kind == AcceleratorConflictDecisionKind::Rejected || kind == AcceleratorConflictDecisionKind::Fault),
			  ShouldFaultToken(kind == AcceleratorConflictDecisionKind::Fault),
			  TokenFaultCode(Fault ? Fault->TokenFaultCode : AcceleratorTokenFaultCode::None) {}
	};

	class AcceleratorFootprintReservation final
	{
	public:
		const std::shared_ptr<AcceleratorTokenHandle> TokenHandle;
		const unsigned long long TokenId;
		const unsigned long long DescriptorIdentityHash;
		const unsigned long long NormalizedFootprintHash;
		const std::shared_ptr<AcceleratorOwnerBinding> OwnerBinding;
		const std::shared_ptr<AcceleratorGuardDecision> ReservationGuardDecision;
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> SourceRanges;
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> DestinationRanges;
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> ScratchRanges;
		const bool HasCompleteFootprintTruth;

		AcceleratorFootprintReservation(std::shared_ptr<AcceleratorToken> token,
			std::shared_ptr<AcceleratorGuardDecision> reservationGuardDecision)
			: TokenHandle(token ? token->getHandle() : nullptr), TokenId(token ? token->getTokenId() : 0),
			  DescriptorIdentityHash(token && token->getDescriptor() ? token->getDescriptor()->Identity.DescriptorIdentityHash : 0),
			  NormalizedFootprintHash(token && token->getDescriptor() ? token->getDescriptor()->NormalizedFootprint.Hash : 0),
			  OwnerBinding(token && token->getDescriptor() ? token->getDescriptor()->OwnerBinding : nullptr),
			  ReservationGuardDecision(std::move(reservationGuardDecision)),
			  SourceRanges(FreezeRanges(token && token->getDescriptor() ? token->getDescriptor()->NormalizedFootprint.SourceRanges : std::vector<std::shared_ptr<AcceleratorMemoryRange>>{})),
			  DestinationRanges(FreezeRanges(token && token->getDescriptor() ? token->getDescriptor()->NormalizedFootprint.DestinationRanges : std::vector<std::shared_ptr<AcceleratorMemoryRange>>{})),
			  ScratchRanges(FreezeRanges(token && token->getDescriptor() ? token->getDescriptor()->NormalizedFootprint.ScratchRanges : std::vector<std::shared_ptr<AcceleratorMemoryRange>>{})),
			  HasCompleteFootprintTruth(TokenHandle && TokenHandle->IsValid && !SourceRanges.empty() && !DestinationRanges.empty() &&
				AreRangesWellFormed(SourceRanges) && AreRangesWellFormed(DestinationRanges) && AreRangesWellFormed(ScratchRanges))
		{
			if (!token) throw std::invalid_argument("token");
			if (!ReservationGuardDecision || !ReservationGuardDecision->IsAllowed)
				throw std::invalid_argument("L7-SDC footprint reservations require an accepted guard decision.");
		}

		bool IsBoundTo(const std::shared_ptr<AcceleratorToken> &token) const
		{
			return token && TokenHandle && token->getHandle() && *TokenHandle == *token->getHandle() &&
				DescriptorIdentityHash == token->getDescriptor()->Identity.DescriptorIdentityHash &&
				NormalizedFootprintHash == token->getDescriptor()->NormalizedFootprint.Hash &&
				OwnerBinding && token->getDescriptor()->OwnerBinding && *OwnerBinding == *token->getDescriptor()->OwnerBinding;
		}

		bool SourceOverlaps(std::shared_ptr<AcceleratorMemoryRange> range, std::shared_ptr<AcceleratorMemoryRange> &overlap) const
		{
			return FirstOverlap(SourceRanges, range, overlap);
		}

		bool DestinationOverlaps(std::shared_ptr<AcceleratorMemoryRange> range, std::shared_ptr<AcceleratorMemoryRange> &overlap) const
		{
			return FirstOverlap(DestinationRanges, range, overlap);
		}

		static bool FirstOverlap(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges,
			std::shared_ptr<AcceleratorMemoryRange> probe, std::shared_ptr<AcceleratorMemoryRange> &overlap)
		{
			overlap.reset();
			if (!IsRangeWellFormed(probe)) return false;
			for (const auto &range : ranges)
			{
				if (!IsRangeWellFormed(range) || !RangesOverlap(*range, *probe)) continue;
				const auto start = (std::max)(range->Address, probe->Address);
				const auto end = (std::min)(range->Address + range->Length, probe->Address + probe->Length);
				overlap = std::make_shared<AcceleratorMemoryRange>(AcceleratorMemoryRange{start, end - start});
				return true;
			}
			return false;
		}

		static bool RangesOverlap(const AcceleratorMemoryRange &left, const AcceleratorMemoryRange &right)
		{
			if (left.Length == 0 || right.Length == 0 ||
				left.Address > std::numeric_limits<unsigned long long>::max() - left.Length ||
				right.Address > std::numeric_limits<unsigned long long>::max() - right.Length) return false;
			const auto leftEnd = left.Address + left.Length;
			const auto rightEnd = right.Address + right.Length;
			return left.Address < rightEnd && right.Address < leftEnd;
		}

		static bool IsRangeWellFormed(const std::shared_ptr<AcceleratorMemoryRange> &range)
		{
			return range && range->Length != 0 && range->Address <= std::numeric_limits<unsigned long long>::max() - range->Length;
		}

		static bool AreRangesWellFormed(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges)
		{
			return std::all_of(ranges.begin(), ranges.end(), [](const auto &range) { return IsRangeWellFormed(range); });
		}

	private:
		static std::vector<std::shared_ptr<AcceleratorMemoryRange>> FreezeRanges(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges)
		{
			std::vector<std::shared_ptr<AcceleratorMemoryRange>> copy;
			copy.reserve(ranges.size());
			for (const auto &range : ranges)
				copy.push_back(range ? std::make_shared<AcceleratorMemoryRange>(*range) : nullptr);
			return copy;
		}
	};

	inline std::shared_ptr<AcceleratorConflictDecision> AcceleratorConflictDecision::Serialize(
		AcceleratorConflictClass conflictClass,
		std::shared_ptr<AcceleratorFootprintReservation> reservation,
		std::shared_ptr<AcceleratorGuardDecision> guardDecision,
		std::shared_ptr<AcceleratorMemoryRange> conflictRange,
		const std::wstring &message,
		bool isWrite)
	{
		auto fault = std::make_shared<AcceleratorConflictFault>(conflictClass,
			AcceleratorTokenFaultCode::ConflictRejected, message,
			conflictRange ? conflictRange->Address : 0, isWrite,
			reservation ? reservation->TokenHandle : nullptr, guardDecision);
		return std::shared_ptr<AcceleratorConflictDecision>(new AcceleratorConflictDecision(
			AcceleratorConflictDecisionKind::Serialize, conflictClass, std::move(reservation),
			std::move(fault), std::move(guardDecision), message));
	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorConflictFault
//	{
//		public AcceleratorConflictFault(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode tokenFaultCode, string message, ulong faultAddress = 0, bool isWrite = false, AcceleratorTokenHandle conflictingTokenHandle = default, System.Nullable<AcceleratorGuardDecision> guardDecision = nullptr)
//		{
//			if (conflictClass == AcceleratorConflictClass.None)
//			{
//				throw new ArgumentException("L7-SDC conflict faults require a non-None conflict class.", nameof(conflictClass));
//			}
//
//			if (tokenFaultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("L7-SDC conflict faults require a non-None token fault code.", nameof(tokenFaultCode));
//			}
//
//			ConflictClass = conflictClass;
//			TokenFaultCode = tokenFaultCode;
//			Message = string.IsNullOrWhiteSpace(message) ? string.Format("L7-SDC conflict rejected: {0}.", conflictClass) : message;
//			FaultAddress = faultAddress;
//			IsWrite = isWrite;
//			ConflictingTokenHandle = conflictingTokenHandle;
//			GuardDecision = guardDecision;
//		}
//
//		public AcceleratorConflictClass ConflictClass
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode TokenFaultCode
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public ulong FaultAddress
//		{
//			get;
//		}
//
//		public bool IsWrite
//		{
//			get;
//		}
//
//		public AcceleratorTokenHandle ConflictingTokenHandle
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorConflictDecision
//	{
//		private AcceleratorConflictDecision(AcceleratorConflictDecisionKind kind, AcceleratorConflictClass conflictClass, System.Nullable<AcceleratorFootprintReservation> reservation, System.Nullable<AcceleratorConflictFault> fault, System.Nullable<AcceleratorGuardDecision> guardDecision, string message)
//		{
//			Kind = kind;
//			ConflictClass = conflictClass;
//			Reservation = reservation;
//			Fault = fault;
//			GuardDecision = guardDecision;
//			Message = message;
//		}
//
//		public AcceleratorConflictDecisionKind Kind
//		{
//			get;
//		}
//
//		public bool IsAccepted
//		{
//			get
//			{
//				return Kind == AcceleratorConflictDecisionKind.Accepted;
//			}
//		}
//
//		public bool RequiresSerialization
//		{
//			get
//			{
//				return Kind == AcceleratorConflictDecisionKind.Serialize;
//			}
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return Kind is AcceleratorConflictDecisionKind.Rejected or AcceleratorConflictDecisionKind.Fault;
//			}
//		}
//
//		public bool ShouldFaultToken
//		{
//			get
//			{
//				return Kind == AcceleratorConflictDecisionKind.Fault;
//			}
//		}
//
//		public AcceleratorConflictClass ConflictClass
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorFootprintReservation> Reservation
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorConflictFault> Fault
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode TokenFaultCode
//		{
//			get
//			{
//				return (Fault == nullptr ? nullptr : Fault.TokenFaultCode) ?? AcceleratorTokenFaultCode.None;
//			}
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
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
//		public bool UserVisiblePublicationAllowed
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static AcceleratorConflictDecision Accepted(AcceleratorConflictClass conflictClass, string message, System.Nullable<AcceleratorFootprintReservation> reservation = nullptr, System.Nullable<AcceleratorGuardDecision> guardDecision = nullptr)
//		{
//			return new(AcceleratorConflictDecisionKind.Accepted, conflictClass, reservation, fault: nullptr, guardDecision, message);
//		}
//
//		public static AcceleratorConflictDecision Serialize(AcceleratorConflictClass conflictClass, AcceleratorFootprintReservation reservation, AcceleratorGuardDecision guardDecision, AcceleratorMemoryRange conflictRange, string message, bool isWrite = true)
//		{
//			return new(AcceleratorConflictDecisionKind.Serialize, conflictClass, reservation, new AcceleratorConflictFault(conflictClass, AcceleratorTokenFaultCode.ConflictRejected, message, conflictRange.Address, isWrite, reservation.TokenHandle, guardDecision), guardDecision, message);
//		}
//
//		public static AcceleratorConflictDecision Reject(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode tokenFaultCode, string message, AcceleratorMemoryRange conflictRange = default, bool isWrite = false, AcceleratorTokenHandle conflictingTokenHandle = default, System.Nullable<AcceleratorGuardDecision> guardDecision = nullptr)
//		{
//			return new(AcceleratorConflictDecisionKind.Rejected, conflictClass, reservation: nullptr, new AcceleratorConflictFault(conflictClass, tokenFaultCode, message, conflictRange.Address, isWrite, conflictingTokenHandle, guardDecision), guardDecision, message);
//		}
//
//		public static AcceleratorConflictDecision Faulted(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode tokenFaultCode, string message, AcceleratorMemoryRange conflictRange = default, bool isWrite = false, AcceleratorTokenHandle conflictingTokenHandle = default, System.Nullable<AcceleratorGuardDecision> guardDecision = nullptr)
//		{
//			return new(AcceleratorConflictDecisionKind.Fault, conflictClass, reservation: nullptr, new AcceleratorConflictFault(conflictClass, tokenFaultCode, message, conflictRange.Address, isWrite, conflictingTokenHandle, guardDecision), guardDecision, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorFootprintReservation
//	{
//		internal AcceleratorFootprintReservation(AcceleratorToken token, AcceleratorGuardDecision reservationGuardDecision)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			if (!reservationGuardDecision.IsAllowed)
//			{
//				throw new ArgumentException("L7-SDC footprint reservations require an accepted guard decision.", nameof(reservationGuardDecision));
//			}
//
//			TokenHandle = token.Handle;
//			TokenId = token.TokenId;
//			DescriptorIdentityHash = token.Descriptor.Identity.DescriptorIdentityHash;
//			NormalizedFootprintHash = token.Descriptor.NormalizedFootprint.Hash;
//			OwnerBinding = token.Descriptor.OwnerBinding;
//			ReservationGuardDecision = reservationGuardDecision;
//			SourceRanges = FreezeRanges(token.Descriptor.NormalizedFootprint.SourceRanges);
//			DestinationRanges = FreezeRanges(token.Descriptor.NormalizedFootprint.DestinationRanges);
//			ScratchRanges = FreezeRanges(token.Descriptor.NormalizedFootprint.ScratchRanges);
//		}
//
//		public AcceleratorTokenHandle TokenHandle
//		{
//			get;
//		}
//
//		public ulong TokenId
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
//		public AcceleratorOwnerBinding OwnerBinding
//		{
//			get;
//		}
//
//		public AcceleratorGuardDecision ReservationGuardDecision
//		{
//			get;
//		}
//
//		public IReadOnlyList<AcceleratorMemoryRange> SourceRanges
//		{
//			get;
//		}
//
//		public IReadOnlyList<AcceleratorMemoryRange> DestinationRanges
//		{
//			get;
//		}
//
//		public IReadOnlyList<AcceleratorMemoryRange> ScratchRanges
//		{
//			get;
//		}
//
//		public bool HasCompleteFootprintTruth
//		{
//			get
//			{
//				return TokenHandle.IsValid && SourceRanges.Count != 0 && DestinationRanges.Count != 0 && AreRangesWellFormed(SourceRanges) && AreRangesWellFormed(DestinationRanges) && AreRangesWellFormed(ScratchRanges);
//			}
//		}
//
//		public bool IsBoundTo(AcceleratorToken token)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			return TokenHandle.Equals(token.Handle) && DescriptorIdentityHash == token.Descriptor.Identity.DescriptorIdentityHash && NormalizedFootprintHash == token.Descriptor.NormalizedFootprint.Hash && OwnerBinding.Equals(token.Descriptor.OwnerBinding);
//		}
//
//		public bool SourceOverlaps(AcceleratorMemoryRange range, out AcceleratorMemoryRange overlap)
//		{
//			return FirstOverlap(SourceRanges, range, out overlap);
//		}
//
//		public bool DestinationOverlaps(AcceleratorMemoryRange range, out AcceleratorMemoryRange overlap)
//		{
//			return FirstOverlap(DestinationRanges, range, out overlap);
//		}
//
//		internal static bool FirstOverlap(IReadOnlyList<AcceleratorMemoryRange> ranges, AcceleratorMemoryRange probe, out AcceleratorMemoryRange overlap)
//		{
//			overlap = default;
//			if (!IsRangeWellFormed(probe))
//			{
//				return false;
//			}
//
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				AcceleratorMemoryRange range = ranges[index];
//				if (!IsRangeWellFormed(range) || !RangesOverlap(range, probe))
//				{
//					continue;
//				}
//
//				ulong start = Math.Max(range.Address, probe.Address);
//				ulong end = Math.Min(range.Address + range.Length, probe.Address + probe.Length);
//				overlap = new AcceleratorMemoryRange(start, end - start);
//				return true;
//			}
//
//			return false;
//		}
//
//		internal static bool RangesOverlap(AcceleratorMemoryRange left, AcceleratorMemoryRange right)
//		{
//			if (!IsRangeWellFormed(left) || !IsRangeWellFormed(right))
//			{
//				return false;
//			}
//
//			ulong leftEnd = left.Address + left.Length;
//			ulong rightEnd = right.Address + right.Length;
//			return left.Address < rightEnd && right.Address < leftEnd;
//		}
//
//		internal static bool IsRangeWellFormed(AcceleratorMemoryRange range)
//		{
//			return range.Length != 0 && range.Address <= ulong.MaxValue - range.Length;
//		}
//
//		internal static bool AreRangesWellFormed(IReadOnlyList<AcceleratorMemoryRange> ranges)
//		{
//			if (ranges == nullptr)
//			{
//				return false;
//			}
//
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				if (!IsRangeWellFormed(ranges[index]))
//				{
//					return false;
//				}
//			}
//
//			return true;
//		}
//
//		private static IReadOnlyList<AcceleratorMemoryRange> FreezeRanges(System.Nullable<IReadOnlyList<AcceleratorMemoryRange>> ranges)
//		{
//			if (ranges == nullptr || ranges.Count == 0)
//			{
//				return Array.Empty<AcceleratorMemoryRange>();
//			}
//
//			AcceleratorMemoryRange[] copy = new AcceleratorMemoryRange[ranges.Count];
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				copy[index] = ranges[index];
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

	class AcceleratorActiveFootprintTable final : public std::enable_shared_from_this<AcceleratorActiveFootprintTable>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, AcceleratorFootprintReservation> _reservations = new();
		std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorFootprintReservation>> _reservations = std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorFootprintReservation>>();

	public:
		int getCount() const;

		const std::vector<std::shared_ptr<AcceleratorFootprintReservation>> getActiveReservations() const;

		bool TryGet(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorFootprintReservation>> &reservation);

		std::shared_ptr<AcceleratorConflictDecision> TryReserve(std::shared_ptr<AcceleratorFootprintReservation> reservation);

		bool Release(std::shared_ptr<AcceleratorTokenHandle> handle);

		std::shared_ptr<AcceleratorConflictDecision> FirstActiveDestinationOverlap(std::shared_ptr<AcceleratorMemoryRange> range, AcceleratorConflictClass conflictClass, const std::wstring &message, std::function<std::optional<std::shared_ptr<AcceleratorGuardDecision>>(std::shared_ptr<AcceleratorFootprintReservation>)> guardSelector);

	private:
		static bool FirstWriteWriteOverlap(std::shared_ptr<AcceleratorFootprintReservation> active, std::shared_ptr<AcceleratorFootprintReservation> incoming, std::shared_ptr<AcceleratorMemoryRange> &overlap);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AcceleratorMemoryRange FirstRangeOrDefault(IReadOnlyList<AcceleratorMemoryRange> ranges)
		static std::shared_ptr<AcceleratorMemoryRange> FirstRangeOrDefault(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges);
	};

	class ExternalAcceleratorConflictManager final : public std::enable_shared_from_this<ExternalAcceleratorConflictManager>
	{
	private:
		const std::shared_ptr<AcceleratorActiveFootprintTable> _activeFootprints = std::make_shared<AcceleratorActiveFootprintTable>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<AcceleratorMemoryRange> _srfWarmWindows = new();
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> _srfWarmWindows = std::vector<std::shared_ptr<AcceleratorMemoryRange>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<AcceleratorMemoryRange> _assistIngressWindows = new();
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> _assistIngressWindows = std::vector<std::shared_ptr<AcceleratorMemoryRange>>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExternalAcceleratorConflictManager(AcceleratorTelemetry? telemetry = null)
		ExternalAcceleratorConflictManager(std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

		std::shared_ptr<AcceleratorActiveFootprintTable> getActiveFootprints() const;

		int getActiveReservationCount() const;

		int getSrfWarmWindowCount() const;

		int getAssistIngressWindowCount() const;

		std::shared_ptr<AcceleratorConflictDecision> TryReserveOnSubmit(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> NotifyCpuLoad(std::shared_ptr<AcceleratorMemoryRange> loadRange, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> NotifyCpuStore(std::shared_ptr<AcceleratorMemoryRange> storeRange, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorConflictDecision NotifyDmaStreamComputeAdmission(IReadOnlyList<AcceleratorMemoryRange> readRanges, IReadOnlyList<AcceleratorMemoryRange> writeRanges, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence)
		std::shared_ptr<AcceleratorConflictDecision> NotifyDmaStreamComputeAdmission(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &readRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &writeRanges, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> NotifySrfWarmWindow(std::shared_ptr<AcceleratorMemoryRange> warmedWindow, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> NotifyAssistIngressWindow(std::shared_ptr<AcceleratorMemoryRange> ingressWindow, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> ValidateBeforeCommit(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> ReleaseTokenFootprint(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> InvalidateSrfCacheOnCommit(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> NotifySerializingBoundary(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorConflictDecision> NotifyVmDomainOrMappingTransition(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

	private:
		static std::shared_ptr<AcceleratorConflictDecision> RejectIncompleteRange(AcceleratorConflictClass conflictClass, std::shared_ptr<AcceleratorMemoryRange> range, bool isWrite);

		std::shared_ptr<AcceleratorConflictDecision> RecordConflictDecision(std::shared_ptr<AcceleratorConflictDecision> decision);

		std::shared_ptr<AcceleratorGuardDecision> ValidateTokenAndDescriptorGuard(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, AcceleratorGuardSurface surface);

		static std::shared_ptr<AcceleratorGuardDecision> ValidateReservationGuard(std::shared_ptr<AcceleratorFootprintReservation> reservation, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		static std::shared_ptr<AcceleratorConflictDecision> RejectDrift(std::shared_ptr<AcceleratorFootprintReservation> reservation, std::shared_ptr<AcceleratorGuardDecision> guardDecision, std::shared_ptr<AcceleratorMemoryRange> faultRange);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool AreRangesWellFormed(IReadOnlyList<AcceleratorMemoryRange>? ranges)
		static bool AreRangesWellFormed(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool FirstWindowOverlap(IReadOnlyList<AcceleratorMemoryRange> leftRanges, IReadOnlyList<AcceleratorMemoryRange> rightRanges, out AcceleratorMemoryRange overlap)
		static bool FirstWindowOverlap(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &leftRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &rightRanges, std::shared_ptr<AcceleratorMemoryRange> &overlap);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AcceleratorMemoryRange FirstRangeOrDefault(IReadOnlyList<AcceleratorMemoryRange> ranges)
		static std::shared_ptr<AcceleratorMemoryRange> FirstRangeOrDefault(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void RemoveOverlappingWindows(List<AcceleratorMemoryRange> windows, IReadOnlyList<AcceleratorMemoryRange> committedRanges)
		static void RemoveOverlappingWindows(std::vector<std::shared_ptr<AcceleratorMemoryRange>> &windows, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &committedRanges);
	};

}

