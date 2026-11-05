#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include "DataCacheRangeResults.h"
#include "../Architecture/MemoryModel/Addressing/MemoryRangeOverlap.h"
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	enum class MemoryCoherencyWriteSourceKind : unsigned char
	{
		Unknown = 0,
		CpuPhysicalWrite = 1,
		AtomicPhysicalWrite = 2,
		DmaControllerWrite = 3,
		DmaStreamComputeCommit = 4,
		StreamEngineWrite = 5,
		L7AcceleratorCommit = 6,
		PhysicalBurstBackendWrite = 7,
		ModelOrchestration = 8
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryCoherencyWriteNotification(ulong Address, ulong Length, ulong DomainTag, MemoryCoherencyWriteSourceKind SourceKind)
//	{
//		public bool IsWellFormed
//		{
//			get
//			{
//				return MemoryRangeOverlap.IsNonEmptyWellFormed(Address, Length);
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DataCacheRangeInvalidationResult(bool RangeWellFormed, int L1LinesInvalidated, int L2LinesInvalidated, int AssistResidentLinesInvalidated)
//	{
//		public int TotalLinesInvalidated
//		{
//			get
//			{
//				return L1LinesInvalidated + L2LinesInvalidated;
//			}
//		}
//
//		public bool Succeeded
//		{
//			get
//			{
//				return RangeWellFormed;
//			}
//		}
//
//		public static DataCacheRangeInvalidationResult Empty
//		{
//			get;
//		} = new(true, 0, 0, 0);
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DataCacheRangeFlushResult(bool RangeWellFormed, bool IsNoOpProof, bool RequiresFutureDirtyWriteback, int CleanLinesObserved, int L1DirtyLines, int L2DirtyLines)
//	{
//		public int DirtyLinesObserved
//		{
//			get
//			{
//				return L1DirtyLines + L2DirtyLines;
//			}
//		}
//
//		public bool Succeeded
//		{
//			get
//			{
//				return RangeWellFormed && !RequiresFutureDirtyWriteback;
//			}
//		}
//
//		public static DataCacheRangeFlushResult EmptyNoOpProof
//		{
//			get;
//		} = new(true, true, false, 0, 0, 0);
//
//		public DataCacheRangeFlushResult Merge(DataCacheRangeFlushResult other)
//		{
//			return new(RangeWellFormed && other.RangeWellFormed, IsNoOpProof && other.IsNoOpProof, RequiresFutureDirtyWriteback || other.RequiresFutureDirtyWriteback, CleanLinesObserved + other.CleanLinesObserved, L1DirtyLines + other.L1DirtyLines, L2DirtyLines + other.L2DirtyLines);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryCoherencyObserverResult(bool NotificationWellFormed, int DataCacheLinesInvalidated, int AssistResidentLinesInvalidated, int SrfWindowsInvalidated)
//	{
//		public bool Succeeded
//		{
//			get
//			{
//				return NotificationWellFormed;
//			}
//		}
//
//		public static MemoryCoherencyObserverResult Empty
//		{
//			get;
//		} = new(true, 0, 0, 0);
//	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct MemoryCoherencyCacheParticipant
	struct MemoryCoherencyWriteNotification {
		unsigned long long Address, Length, DomainTag;
		MemoryCoherencyWriteSourceKind SourceKind;
		MemoryCoherencyWriteNotification(unsigned long long address, unsigned long long length, unsigned long long domainTag, MemoryCoherencyWriteSourceKind sourceKind) : Address(address), Length(length), DomainTag(domainTag), SourceKind(sourceKind) {}
		bool isWellFormed() const { return MemoryRangeOverlap::IsNonEmptyWellFormed(Address, Length); }
	};
	class MemoryCoherencyCacheParticipant
	{
	private:
		std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> _l1Data;
		std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> _l2Data;

	public:
		MemoryCoherencyCacheParticipant(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data);

		std::shared_ptr<DataCacheRangeInvalidationResult> InvalidateRange(unsigned long long address, unsigned long long length, unsigned long long domainTag = 0) const;

		std::shared_ptr<DataCacheRangeFlushResult> FlushRange(unsigned long long address, unsigned long long length, unsigned long long domainTag = 0) const;

		static std::shared_ptr<DataCacheRangeInvalidationResult> InvalidateArrays(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data, unsigned long long address, unsigned long long length, unsigned long long domainTag = 0);

		static std::shared_ptr<DataCacheRangeFlushResult> FlushArrays(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data, unsigned long long address, unsigned long long length, unsigned long long domainTag = 0);

	private:
		static int InvalidateArray(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> cache, unsigned long long address, unsigned long long length, unsigned long long domainTag, int &assistInvalidated);

		static int CountOverlappingDirtyLines(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> cache, unsigned long long address, unsigned long long length, unsigned long long domainTag, int &cleanLines);

		static bool IsLiveLine(Processor::CPU_Core::Cache_Data_Object line);

		static bool DomainMatches(unsigned long long lineDomainTag, unsigned long long notificationDomainTag);

		MemoryCoherencyCacheParticipant() = default;
	};

	/// <summary>
	/// Explicit non-coherent publication observer for modeled external writes.
	/// Callers must route write notifications through this object; it is not a
	/// coherent DMA/cache hierarchy, snoop fabric, or automatic CPU memory authority.
	/// </summary>
	class MemoryCoherencyObserver final : public std::enable_shared_from_this<MemoryCoherencyObserver>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<MemoryCoherencyCacheParticipant> _cacheParticipants = new();
		std::vector<MemoryCoherencyCacheParticipant> _cacheParticipants = std::vector<MemoryCoherencyCacheParticipant>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<StreamRegisterFile> _streamRegisterFiles = new();
		std::vector<std::shared_ptr<StreamRegisterFile>> _streamRegisterFiles = std::vector<std::shared_ptr<StreamRegisterFile>>();

	public:
		std::shared_ptr<MemoryCoherencyObserver> RegisterDataCache(std::shared_ptr<Processor::CPU_Core> core);

		std::shared_ptr<MemoryCoherencyObserver> RegisterDataCache(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data);

		std::shared_ptr<MemoryCoherencyObserver> RegisterStreamRegisterFile(std::shared_ptr<StreamRegisterFile> streamRegisterFile);

		std::shared_ptr<MemoryCoherencyObserverResult> NotifyWrite(std::shared_ptr<MemoryCoherencyWriteNotification> notification);

		std::shared_ptr<DataCacheRangeFlushResult> FlushBeforeExternalRead(unsigned long long address, unsigned long long length, unsigned long long domainTag = 0);
	};
}
