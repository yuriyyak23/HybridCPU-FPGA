#pragma once

#include "../../Core/Pipeline/Assist/Runtime/AssistRuntime.h"
#include "../../../NonRTL/Memory/Registers/StreamRegisterFile.Telemetry.h"
#include <vector>
#include <tuple>
#include <span>
#include <cmath>
#include <limits>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;




namespace YAKSys_Hybrid_CPU::Memory
{
	/// <summary>
	/// Stream Register File for HybridCPU.
	/// Provides a local buffer pool for prefetching stream data.
	/// </summary>
	class StreamRegisterFile : public std::enable_shared_from_this<StreamRegisterFile>
	{
	public:
		enum class RegisterState : unsigned char;

		bool CanAllocateAssistRegister(unsigned long long sourceAddr, unsigned char elementSize, AssistStreamRegisterPartitionPolicy policy, AssistStreamRegisterRejectKind &rejectKind);

		bool TryAllocateAssistRegister(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount, AssistStreamRegisterPartitionPolicy policy, int &registerIndex, AssistStreamRegisterRejectKind &rejectKind);

		void MarkAssistLoading(int regIndex);

		int CountAssistOwnedRegisters();

		int CountAssistOwnedRegisters(RegisterState state);

	private:
		bool TryFindAssistReuseRegister(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount, int &registerIndex);

		bool TrySelectAssistRegisterSlot(AssistStreamRegisterPartitionPolicy policy, int &registerIndex, AssistStreamRegisterRejectKind &rejectKind);

		int FindEmptyRegisterSlot();

		int FindAssistVictimRegister();


		/// <summary>
		/// Maximum vector length (elements per register)
		/// </summary>
		static constexpr int VLMAX = 32;

		/// <summary>
		/// Maximum element size (8 bytes for FLOAT64/INT64)
		/// </summary>
		static constexpr int MAX_ELEMENT_SIZE = 8;

		/// <summary>
		/// Size of each stream register in bytes
		/// </summary>
		static constexpr int REGISTER_SIZE = VLMAX * MAX_ELEMENT_SIZE;

		/// <summary>
		/// Number of stream registers
		/// </summary>
		const int registerCount;

		/// <summary>
		/// Stream register state
		/// </summary>
	public:
		enum class RegisterState : unsigned char
		{
			Invalid = 0,
			Loading = 1,
			Valid = 2,
			Dirty = 3
		};

		/// <summary>
		/// Stream register entry
		/// </summary>
	private:
		class RegisterEntry
		{
		public:
			std::vector<unsigned char> Data;
			RegisterState State = static_cast<RegisterState>(0);
			unsigned long long SourceAddress = 0;
			unsigned int ValidBytes = 0;
			unsigned long long LastAccessTime = 0;
			unsigned char ElementSize = 0;
			unsigned int ElementCount = 0;
			bool AssistOwned = false;
		};

	private:
		std::vector<RegisterEntry> registers;
		unsigned long long accessCounter = 0;
		unsigned long long totalHits = 0;
		unsigned long long totalMisses = 0;
		unsigned long long totalEvictions = 0;

		/// <summary>
		/// Counts actual SRF-served reads that bypassed the live memory backend.
		/// </summary>
		unsigned long long _l1BypassHits = 0;

		/// <summary>
		/// Initialize Stream Register File
		/// </summary>
	public:
		StreamRegisterFile(int numRegisters = 8);

		/// <summary>
		/// Allocate a stream register for a memory region.
		/// Returns a cache hit only when the existing valid register already
		/// contains the full requested packed window.
		/// </summary>
		int AllocateRegister(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount);

		/// <summary>
		/// Find victim register for eviction (LRU policy)
		/// </summary>
	private:
		int FindVictimRegister();

		/// <summary>
		/// Load data into a stream register from a full memory image.
		/// Compatibility path for existing tests.
		/// </summary>
	public:
		bool LoadRegister(int regIndex, std::vector<unsigned char> &memory);

		/// <summary>
		/// Load already-materialized bytes into a stream register.
		/// </summary>
		bool LoadRegister(int regIndex, std::span<const unsigned char> sourceData);

		/// <summary>
		/// Read data from a stream register.
		/// Successful reads count as real SRF bypasses.
		/// </summary>
		bool ReadRegister(int regIndex, std::span<unsigned char> destination, unsigned int byteCount);

		/// <summary>
		/// Try to serve an exact packed contiguous chunk from SRF.
		/// </summary>
		bool TryReadPrefetchedChunk(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount, std::span<unsigned char> destination);

		/// <summary>
		/// Write data to a stream register (marks as dirty)
		/// </summary>
		bool WriteRegister(int regIndex, std::span<const unsigned char> source, unsigned int byteCount);

		/// <summary>
		/// Get register state
		/// </summary>
		RegisterState GetRegisterState(int regIndex);

		/// <summary>
		/// Invalidate a register
		/// </summary>
		void InvalidateRegister(int regIndex);

		/// <summary>
		/// Invalidate all registers
		/// </summary>
		void InvalidateAll();

		/// <summary>
		/// Mark a register as Loading.
		/// </summary>
		void MarkLoading(int regIndex);

		/// <summary>
		/// Invalidate any SRF window that overlaps the written address range.
		/// </summary>
		void InvalidateOverlappingRange(unsigned long long address, unsigned int byteCount);

		/// <summary>
		/// Invalidate any SRF window that overlaps the written address range and
		/// return the number of windows invalidated.
		/// </summary>
		int InvalidateOverlappingRangeAndCount(unsigned long long address, unsigned long long byteCount);

		/// <summary>
		/// Get performance statistics
		/// </summary>
		std::tuple<unsigned long long, unsigned long long, unsigned long long, double, unsigned long long> GetStatistics() const
		{
			const unsigned long long total = totalHits + totalMisses;
			const double hitRate = total > 0 ? static_cast<double>(totalHits) / total : 0.0;
			return {totalHits, totalMisses, totalEvictions, hitRate, _l1BypassHits};
		}

	private:
		static unsigned int ComputeRequestedByteCount(unsigned char elementSize, unsigned int elementCount);

		static unsigned int GetTrackedByteCount(RegisterEntry const reg);



		unsigned long long _foregroundWarmAttempts = 0;
		unsigned long long _foregroundWarmSuccesses = 0;
		unsigned long long _foregroundWarmReuseHits = 0;
		unsigned long long _foregroundBypassHits = 0;
		unsigned long long _assistWarmAttempts = 0;
		unsigned long long _assistWarmSuccesses = 0;
		unsigned long long _assistWarmReuseHits = 0;
		unsigned long long _assistBypassHits = 0;
		unsigned long long _translationRejects = 0;
		unsigned long long _backendRejects = 0;
		unsigned long long _assistResidentBudgetRejects = 0;
		unsigned long long _assistLoadingBudgetRejects = 0;
		unsigned long long _assistNoVictimRejects = 0;

	public:
		void RecordWarmAttempt(bool assistOwned);

		void RecordWarmSuccess(bool assistOwned);

		void RecordWarmReuse(bool assistOwned);

	private:
		void RecordBypassHit(bool assistOwned);

	public:
		void RecordWarmTranslationReject();

		void RecordWarmBackendReject();

		void RecordAssistScheduleReject(AssistStreamRegisterRejectKind rejectKind);

		StreamIngressWarmTelemetry GetIngressWarmTelemetry();
	};
}
