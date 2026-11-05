#pragma once

#include "../Tokens/AcceleratorToken.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <unordered_map>
#include <vector>
#include <limits>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>
#include <span>
#include <algorithm>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { struct AcceleratorMemoryRange; }
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorMemoryRead(AcceleratorMemoryRange Range, ReadOnlyMemory<byte> Data);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorStagedWrite(AcceleratorTokenHandle TokenHandle, ulong Address, ReadOnlyMemory<byte> Data)
//	{
//		public ulong Length
//		{
//			get
//			{
//				return (ulong)Data.Length;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorMemoryPortalReadResult
//	{
//		private AcceleratorMemoryPortalReadResult(bool isAccepted, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, IReadOnlyList<AcceleratorMemoryRead> reads, string message)
//		{
//			IsAccepted = isAccepted;
//			FaultCode = faultCode;
//			GuardDecision = guardDecision;
//			Reads = reads;
//			Message = message;
//		}
//
//		public bool IsAccepted
//		{
//			get;
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !IsAccepted;
//			}
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public IReadOnlyList<AcceleratorMemoryRead> Reads
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
//		public bool CanPublishException
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public ulong BytesRead
//		{
//			get
//			{
//				ulong bytes = 0;
//				for (int index = 0; index < Reads.Count; index++)
//				{
//					bytes += (ulong)Reads[index].Data.Length;
//				}
//
//				return bytes;
//			}
//		}
//
//		public static AcceleratorMemoryPortalReadResult Accepted(IReadOnlyList<AcceleratorMemoryRead> reads, AcceleratorGuardDecision guardDecision, string message)
//		{
//			ArgumentNullException.ThrowIfNull(reads);
//			return new AcceleratorMemoryPortalReadResult(isAccepted: true, AcceleratorTokenFaultCode.None, guardDecision, FreezeReads(reads), message);
//		}
//
//		public static AcceleratorMemoryPortalReadResult Reject(AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, string message)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected L7-SDC memory portal reads require a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorMemoryPortalReadResult(isAccepted: false, faultCode, guardDecision, Array.Empty<AcceleratorMemoryRead>(), message);
//		}
//
//		private static IReadOnlyList<AcceleratorMemoryRead> FreezeReads(IReadOnlyList<AcceleratorMemoryRead> reads)
//		{
//			if (reads.Count == 0)
//			{
//				return Array.Empty<AcceleratorMemoryRead>();
//			}
//
//			AcceleratorMemoryRead[] copy = new AcceleratorMemoryRead[reads.Count];
//			for (int index = 0; index < reads.Count; index++)
//			{
//				AcceleratorMemoryRead read = reads[index];
//				copy[index] = read with { Data = CopyMemory(read.Data) };
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//
//		private static ReadOnlyMemory<byte> CopyMemory(ReadOnlyMemory<byte> data)
//		{
//			byte[] copy = data.ToArray();
//			return copy;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorStagingResult
//	{
//		private AcceleratorStagingResult(bool isAccepted, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, System.Nullable<AcceleratorStagedWrite> stagedWrite, string message)
//		{
//			IsAccepted = isAccepted;
//			FaultCode = faultCode;
//			GuardDecision = guardDecision;
//			StagedWrite = stagedWrite;
//			Message = message;
//		}
//
//		public bool IsAccepted
//		{
//			get;
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !IsAccepted;
//			}
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorStagedWrite> StagedWrite
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
//		public bool CanPublishException
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static AcceleratorStagingResult Accepted(AcceleratorStagedWrite stagedWrite, AcceleratorGuardDecision guardDecision, string message)
//		{
//			ArgumentNullException.ThrowIfNull(stagedWrite);
//			return new AcceleratorStagingResult(isAccepted: true, AcceleratorTokenFaultCode.None, guardDecision, stagedWrite, message);
//		}
//
//		public static AcceleratorStagingResult Reject(AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, string message)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected L7-SDC staging operations require a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorStagingResult(isAccepted: false, faultCode, guardDecision, stagedWrite: nullptr, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorStagingReadResult
//	{
//		private AcceleratorStagingReadResult(bool isAccepted, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, IReadOnlyList<AcceleratorStagedWrite> stagedWrites, string message)
//		{
//			IsAccepted = isAccepted;
//			FaultCode = faultCode;
//			GuardDecision = guardDecision;
//			StagedWrites = stagedWrites;
//			Message = message;
//		}
//
//		public bool IsAccepted
//		{
//			get;
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !IsAccepted;
//			}
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public IReadOnlyList<AcceleratorStagedWrite> StagedWrites
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
//		public bool CanPublishException
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static AcceleratorStagingReadResult Accepted(IReadOnlyList<AcceleratorStagedWrite> stagedWrites, AcceleratorGuardDecision guardDecision, string message)
//		{
//			ArgumentNullException.ThrowIfNull(stagedWrites);
//			return new AcceleratorStagingReadResult(isAccepted: true, AcceleratorTokenFaultCode.None, guardDecision, FreezeStagedWrites(stagedWrites), message);
//		}
//
//		public static AcceleratorStagingReadResult Reject(AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, string message)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected L7-SDC staging read operations require a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorStagingReadResult(isAccepted: false, faultCode, guardDecision, Array.Empty<AcceleratorStagedWrite>(), message);
//		}
//
//		private static IReadOnlyList<AcceleratorStagedWrite> FreezeStagedWrites(IReadOnlyList<AcceleratorStagedWrite> stagedWrites)
//		{
//			if (stagedWrites.Count == 0)
//			{
//				return Array.Empty<AcceleratorStagedWrite>();
//			}
//
//			AcceleratorStagedWrite[] copy = new AcceleratorStagedWrite[stagedWrites.Count];
//			for (int index = 0; index < stagedWrites.Count; index++)
//			{
//				AcceleratorStagedWrite stagedWrite = stagedWrites[index];
//				copy[index] = stagedWrite with { Data = stagedWrite.Data.ToArray() };
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

	// These value types were omitted by the C# converter. Their byte payloads are
	// owned by value so callers cannot mutate data after it has been accepted.
	class AcceleratorMemoryRead final
	{
	public:
		const std::shared_ptr<AcceleratorMemoryRange> Range;
		const std::vector<unsigned char> Data;

		AcceleratorMemoryRead(std::shared_ptr<AcceleratorMemoryRange> range, const std::vector<unsigned char>& data)
			: Range(std::move(range)), Data(data) {}
	};

	class AcceleratorStagedWrite final
	{
	public:
		const std::shared_ptr<AcceleratorTokenHandle> TokenHandle;
		const unsigned long long Address;
		const std::vector<unsigned char> Data;
		const unsigned long long Length;

		AcceleratorStagedWrite(std::shared_ptr<AcceleratorTokenHandle> tokenHandle, unsigned long long address, const std::vector<unsigned char>& data)
			: TokenHandle(std::move(tokenHandle)), Address(address), Data(data), Length(static_cast<unsigned long long>(data.size())) {}
	};

	class AcceleratorMemoryPortalReadResult final
	{
	public:
		const bool IsAccepted;
		const bool IsRejected;
		const AcceleratorTokenFaultCode FaultCode;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const std::vector<std::shared_ptr<AcceleratorMemoryRead>> Reads;
		const std::wstring Message;
		const bool CanPublishArchitecturalMemory = false;
		const bool CanPublishException = false;
		const unsigned long long BytesRead;

		AcceleratorMemoryPortalReadResult(bool accepted, AcceleratorTokenFaultCode faultCode,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision,
			std::vector<std::shared_ptr<AcceleratorMemoryRead>> reads, const std::wstring& message)
			: IsAccepted(accepted), IsRejected(!accepted), FaultCode(faultCode), GuardDecision(std::move(guardDecision)),
			  Reads(std::move(reads)), Message(message), BytesRead(ComputeBytesRead(Reads)) {}

		bool getIsRejected() const { return !IsAccepted; }

		static std::shared_ptr<AcceleratorMemoryPortalReadResult> Accepted(
			const std::vector<std::shared_ptr<AcceleratorMemoryRead>>& reads,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring& message)
		{
			if (reads.empty()) return std::make_shared<AcceleratorMemoryPortalReadResult>(true, AcceleratorTokenFaultCode::None, std::move(guardDecision), std::vector<std::shared_ptr<AcceleratorMemoryRead>>{}, message);
			std::vector<std::shared_ptr<AcceleratorMemoryRead>> frozen;
			frozen.reserve(reads.size());
			for (const auto& read : reads)
			{
				if (!read) throw std::invalid_argument("reads contains a null entry");
				frozen.push_back(std::make_shared<AcceleratorMemoryRead>(read->Range, read->Data));
			}
			return std::make_shared<AcceleratorMemoryPortalReadResult>(true, AcceleratorTokenFaultCode::None, std::move(guardDecision), std::move(frozen), message);
		}

		static std::shared_ptr<AcceleratorMemoryPortalReadResult> Reject(
			AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorGuardDecision> guardDecision,
			const std::wstring& message)
		{
			if (faultCode == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected L7-SDC memory portal reads require a fault code.");
			return std::make_shared<AcceleratorMemoryPortalReadResult>(false, faultCode, std::move(guardDecision), std::vector<std::shared_ptr<AcceleratorMemoryRead>>{}, message);
		}

	private:
		static unsigned long long ComputeBytesRead(const std::vector<std::shared_ptr<AcceleratorMemoryRead>>& reads)
		{
			unsigned long long bytes = 0;
			for (const auto& read : reads) if (read) bytes += static_cast<unsigned long long>(read->Data.size());
			return bytes;
		}
	};

	class AcceleratorStagingResult final
	{
	public:
		const bool IsAccepted;
		const bool IsRejected;
		const AcceleratorTokenFaultCode FaultCode;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const std::shared_ptr<AcceleratorStagedWrite> StagedWrite;
		const std::wstring Message;
		const bool CanPublishArchitecturalMemory = false;
		const bool CanPublishException = false;

		AcceleratorStagingResult(bool accepted, AcceleratorTokenFaultCode faultCode,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision,
			std::shared_ptr<AcceleratorStagedWrite> stagedWrite, const std::wstring& message)
			: IsAccepted(accepted), IsRejected(!accepted), FaultCode(faultCode), GuardDecision(std::move(guardDecision)),
			  StagedWrite(std::move(stagedWrite)), Message(message) {}

		bool getIsRejected() const { return !IsAccepted; }

		static std::shared_ptr<AcceleratorStagingResult> Accepted(
			std::shared_ptr<AcceleratorStagedWrite> stagedWrite,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring& message)
		{
			if (!stagedWrite) throw std::invalid_argument("stagedWrite");
			return std::make_shared<AcceleratorStagingResult>(true, AcceleratorTokenFaultCode::None, std::move(guardDecision), std::move(stagedWrite), message);
		}

		static std::shared_ptr<AcceleratorStagingResult> Reject(
			AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorGuardDecision> guardDecision,
			const std::wstring& message)
		{
			if (faultCode == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected L7-SDC staging operations require a fault code.");
			return std::make_shared<AcceleratorStagingResult>(false, faultCode, std::move(guardDecision), nullptr, message);
		}
	};

	class AcceleratorStagingReadResult final
	{
	public:
		const bool IsAccepted;
		const bool IsRejected;
		const AcceleratorTokenFaultCode FaultCode;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const std::vector<std::shared_ptr<AcceleratorStagedWrite>> StagedWrites;
		const std::wstring Message;
		const bool CanPublishArchitecturalMemory = false;
		const bool CanPublishException = false;

		AcceleratorStagingReadResult(bool accepted, AcceleratorTokenFaultCode faultCode,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision,
			std::vector<std::shared_ptr<AcceleratorStagedWrite>> stagedWrites, const std::wstring& message)
			: IsAccepted(accepted), IsRejected(!accepted), FaultCode(faultCode), GuardDecision(std::move(guardDecision)),
			  StagedWrites(std::move(stagedWrites)), Message(message) {}

		bool getIsRejected() const { return !IsAccepted; }

		static std::shared_ptr<AcceleratorStagingReadResult> Accepted(
			const std::vector<std::shared_ptr<AcceleratorStagedWrite>>& stagedWrites,
			std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring& message)
		{
			std::vector<std::shared_ptr<AcceleratorStagedWrite>> frozen;
			frozen.reserve(stagedWrites.size());
			for (const auto& write : stagedWrites)
			{
				if (!write) throw std::invalid_argument("stagedWrites contains a null entry");
				frozen.push_back(std::make_shared<AcceleratorStagedWrite>(write->TokenHandle, write->Address, write->Data));
			}
			return std::make_shared<AcceleratorStagingReadResult>(true, AcceleratorTokenFaultCode::None, std::move(guardDecision), std::move(frozen), message);
		}

		static std::shared_ptr<AcceleratorStagingReadResult> Reject(
			AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorGuardDecision> guardDecision,
			const std::wstring& message)
		{
			if (faultCode == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected L7-SDC staging read operations require a fault code.");
			return std::make_shared<AcceleratorStagingReadResult>(false, faultCode, std::move(guardDecision), std::vector<std::shared_ptr<AcceleratorStagedWrite>>{}, message);
		}
	};

	class IAcceleratorMemoryPortal
	{
	public:
		virtual std::shared_ptr<AcceleratorMemoryPortalReadResult> ReadSourceRanges(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) = 0;
	};

	class IAcceleratorStagingBuffer
	{
	public:
		virtual std::shared_ptr<AcceleratorStagingResult> StageWrite(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorMemoryRange> destinationRange, std::span<const unsigned char> data, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) = 0;

		virtual std::shared_ptr<AcceleratorStagingReadResult> GetStagedWriteSet(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) = 0;
	};

	class MainMemoryReadOnlyAcceleratorMemoryPortal final : public IAcceleratorMemoryPortal
	{
	private:
		const std::shared_ptr<Processor::MainMemoryArea> _mainMemory;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public MainMemoryReadOnlyAcceleratorMemoryPortal(Processor.MainMemoryArea mainMemory)
		MainMemoryReadOnlyAcceleratorMemoryPortal(std::shared_ptr<Processor::MainMemoryArea> mainMemory);

		std::shared_ptr<AcceleratorMemoryPortalReadResult> ReadSourceRanges(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

	private:
		bool HasExactMainMemoryRange(unsigned long long address, int size);
	};

	class AcceleratorStagingBuffer final : public IAcceleratorStagingBuffer
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, List<AcceleratorStagedWrite>> _writesByHandle = new();
		std::unordered_map<unsigned long long, std::vector<std::shared_ptr<AcceleratorStagedWrite>>> _writesByHandle = std::unordered_map<unsigned long long, std::vector<std::shared_ptr<AcceleratorStagedWrite>>>();

	public:
		int getTotalStagedWriteCount() const;

		std::shared_ptr<AcceleratorStagingResult> StageWrite(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorMemoryRange> destinationRange, std::span<const unsigned char> data, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorStagingReadResult> GetStagedWriteSet(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool IsRangeCoveredByFootprint(AcceleratorMemoryRange range, IReadOnlyList<AcceleratorMemoryRange> footprint)
		static bool IsRangeCoveredByFootprint(std::shared_ptr<AcceleratorMemoryRange> range, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &footprint);
	};

}
