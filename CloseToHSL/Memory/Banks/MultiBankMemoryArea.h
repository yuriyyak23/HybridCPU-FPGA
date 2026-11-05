#pragma once

#include "../../Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <vector>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <mutex>
#include <utility>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::CloseToHSL::Memory::Banks
{
	/// <summary>
	/// Multi-bank memory area implementation for HybridCPU.
	/// Provides parallel memory access through multiple independent memory banks.
	///
	/// Key features:
	/// - Multiple independent memory banks for parallel access
	/// - Bank interleaving based on address
	/// - Simulates parallel bandwidth improvement
	/// - Compatible with existing IOMMU infrastructure
	/// - Phase 2: Bank conflict detection with cycle-accurate timing
	/// - Phase 2: Port management with switching penalties
	/// - Phase 2: Row buffer simulation (hit/miss latencies)
	///
	/// Architecture:
	/// - Each bank is an independent MemoryStream
	/// - Address mapping: bank = (address / bankSize) % bankCount
	/// - Offset within bank: address % bankSize
	///
	/// Design goals (ref2.md):
	/// - Enable concurrent burst transactions to different banks
	/// - Reduce memory access contention
	/// - Support streaming operations with high bandwidth
	/// </summary>
	class MultiBankMemoryArea : public Processor::MainMemoryArea
	{
	private:
		long long SilentSquashCount = 0;

		/// <summary>
		/// Bank state tracking for cycle-accurate timing simulation (Phase 2)
		/// </summary>
	private:
		class BankState
		{
		public:
			bool Busy = false; // Is bank currently busy?
			long long BusyUntilCycle = 0; // Cycle when bank becomes available
			int QueueDepth = 0; // Number of pending requests
			unsigned long long LastAccessAddress = 0; // Last accessed address for row buffer
			int PortId = 0; // Which port is using this bank (-1 = none)
		};

	private:
		std::vector<std::shared_ptr<MemoryStream>> _banks;
		int _bankCount = 0;
		unsigned long long _bankSize = 0; // Size of each bank in bytes
		unsigned long long _totalSize = 0; // Total memory size
		std::vector<BankState> _bankStates; // Phase 2: Per-bank state tracking
		std::vector<std::shared_ptr<std::mutex>> _bankLocks; // Phase 2: Per-bank locks for thread safety

		/// <summary>
		/// Initialize multi-bank memory area.
		/// </summary>
		/// <param name="bankCount">Number of memory banks (typically 2, 4, or 8)</param>
		/// <param name="bankSize">Size of each bank in bytes (e.g., 64MB = 0x4000000UL)</param>
	public:
		MultiBankMemoryArea(int bankCount, unsigned long long bankSize);

		/// <summary>
		/// Get the bank and offset for a given physical address.
		/// Bank selection uses interleaving for better distribution.
		/// </summary>
		/// <param name="physicalAddress">Physical address to map</param>
		/// <returns>Bank index and offset within that bank</returns>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private (MemoryStream bank, ulong offset) GetBank(ulong physicalAddress)
//		{
//			// Bank interleaving: address is mapped to banks cyclically
//			// This ensures sequential addresses are distributed across banks
//			int bankIndex = (int)((physicalAddress / _bankSize) % (ulong)_bankCount);
//			ulong offset = physicalAddress % _bankSize;
//			return (_banks[bankIndex], offset);
//		}

	private:
		std::pair<std::shared_ptr<MemoryStream>, unsigned long long> GetBank(unsigned long long physicalAddress) const;
	public:

		/// <summary>
		/// Override Length to return total memory size across all banks.
		/// </summary>
		long long getLength() const override;

		/// <summary>
		/// Override Capacity to return total capacity across all banks.
		/// </summary>
		int getCapacity() const override;
		void setCapacity(int value) override;

		/// <summary>
		/// Silent Squash counter: incremented when a domain-checked access is rejected (req.md §4).
		/// The operation is silently dropped (no interrupt, no pipeline stall).
		/// </summary>
		long long getSilentSquashCount() const;
	private:
		void setSilentSquashCount(long long value);

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [ThreadStatic] private static bool _lastAccessSilentlySquashed;
		static inline thread_local bool _lastAccessSilentlySquashed = false;

		/// <summary>
		/// Override Read to access appropriate memory bank.
		/// </summary>
	public:
		int Read(std::vector<unsigned char> &buffer, int offset, int count) override;

		/// <summary>
		/// Override Write to access appropriate memory bank.
		/// </summary>
		void Write(std::vector<unsigned char> &buffer, int offset, int count) override;

		// MainMemoryArea's physical byte fast paths address SystemMemory.
		// Banked memory must use its own checked bank path for these APIs too.
		int ReadByte() override;

		void WriteByte(unsigned char value) override;

		/// <summary>
		/// Current domain tag for the active memory access context (req.md §4).
		/// Set before physical read/write by the pipeline/memory subsystem.
		/// 0 = trusted kernel / unrestricted access.
		/// </summary>
	private:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [ThreadStatic] private static ulong _currentDomainTag;
		static inline thread_local unsigned long long _currentDomainTag = 0;

		/// <summary>
		/// Set the domain tag for subsequent physical memory accesses.
		/// Called by the pipeline before executing a stolen MicroOp's memory access.
		/// </summary>
		/// <param name="domainTag">Domain tag from MicroOp.DomainTag (0 = unrestricted)</param>
	public:
		static void SetAccessDomainTag(unsigned long long domainTag);

		static void ResetLastAccessSilentSquashFlag();

		static bool ConsumeLastAccessSilentSquashFlag();

		/// <summary>
		/// Override SetLength to adjust total memory size.
		/// </summary>
		void SetLength(long long value) override;

		/// <summary>
		/// Override Flush to flush all banks.
		/// </summary>
		void Flush() override;

		/// <summary>
		/// Get statistics about bank usage (for debugging/profiling).
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public (int bankCount, ulong bankSize, ulong totalSize) GetBankInfo()
//		{
//			return (_bankCount, _bankSize, _totalSize);
//		}

		/// <summary>
		/// Get the current length of a specific bank.
		/// </summary>
		long long GetBankLength(int bankIndex);

//		#region Phase 2: Microarchitecture Fidelity

		/// <summary>
		/// Per-bank domain capability mask (req.md §2: Capability Tags).
		/// Each bank stores a bitmask of domain tags allowed to access it.
		/// A zero mask means unrestricted access (backward-compatible default).
		/// HLS: maps to a fixed-size register array (one 64-bit register per bank).
		/// </summary>
	private:
		std::vector<unsigned long long> _bankDomainCapabilities;

		/// <summary>
		/// Assign a domain capability mask to a specific bank.
		/// Only threads whose DomainTag overlaps with the bank capability mask are allowed access.
		/// Called by OS/kernel during memory domain setup.
		/// </summary>
		/// <param name="bankId">Target bank index</param>
		/// <param name="domainMask">Bitmask of allowed domain tags (0 = unrestricted)</param>
	public:
		void SetBankDomainCapability(int bankId, unsigned long long domainMask);

		/// <summary>
		/// Check if a domain tag is allowed to access a specific bank (req.md §2).
		/// Performs a single-cycle AND gate check: (bankCapability &amp; domainTag) != 0.
		/// Returns true if access is allowed, false for silent squash.
		/// HLS: synthesizes as a single AND + compare-to-zero gate.
		/// </summary>
		/// <param name="bankId">Bank being accessed</param>
		/// <param name="domainTag">Requester's domain tag (from MicroOp.DomainTag)</param>
		/// <returns>True if access is permitted</returns>
		bool CheckBankDomainAccess(int bankId, unsigned long long domainTag);

		/// <summary>
		/// Check if a memory access to the given address will cause a bank conflict.
		/// Returns the bank ID, conflict status, and estimated stall cycles.
		/// (Phase 2: Requirement 2.1)
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public (int BankId, bool HasConflict, int StallCycles) CheckBankConflict(ulong address, int size, long currentCycle)
//		{
//			int bankId = (int)((address / _bankSize) % (ulong)_bankCount);
//
//			lock (_bankLocks[bankId])
//			{
//				ref BankState state = ref _bankStates[bankId];
//
//				// Check if bank is busy
//				if (state.Busy && currentCycle < state.BusyUntilCycle)
//				{
//					int stallCycles = (int)(state.BusyUntilCycle - currentCycle);
//					// Additional stalls based on queue depth
//					stallCycles += state.QueueDepth * 2;
//					return (bankId, true, stallCycles);
//				}
//
//				return (bankId, false, 0);
//			}
//		}

		/// <summary>
		/// Calculate memory access latency based on address and access pattern.
		/// Includes row buffer simulation (hit = 10 cycles, miss = 30 cycles).
		/// (Phase 2: Requirement 2.1)
		/// </summary>
		int CalculateAccessLatency(unsigned long long address, int size, bool isSequential);

		/// <summary>
		/// Allocate a memory port for accessing a specific bank.
		/// Returns the port ID that was allocated.
		/// (Phase 2: Requirement 2.1)
		/// </summary>
		int AllocatePort(int bankId, long long currentCycle);

		/// <summary>
		/// Release a memory port that was accessing a specific bank.
		/// (Phase 2: Requirement 2.1)
		/// </summary>
		void ReleasePort(int portId, int bankId, long long currentCycle);

		/// <summary>
		/// Mark a bank as busy until the specified cycle.
		/// Updates last access address for row buffer simulation.
		/// (Phase 2: Internal utility)
		/// </summary>
		void MarkBankBusy(int bankId, unsigned long long address, long long completionCycle);

		/// <summary>
		/// Mark a bank as available (no longer busy).
		/// (Phase 2: Internal utility)
		/// </summary>
		void MarkBankAvailable(int bankId, long long currentCycle);

//		#endregion

	protected:
		std::shared_ptr<MultiBankMemoryArea> shared_from_this()
		{
			return std::static_pointer_cast<MultiBankMemoryArea>(Processor::MainMemoryArea::shared_from_this());
		}
	};

	/// <summary>
	/// Bank arbitrator for managing per-cycle bank access conflicts.
	/// Tracks which banks are reserved in the current cycle to detect conflicts.
	/// HLS-compatible: fixed-size bool array, no dynamic allocation.
	/// (Phase 2: Memory Arbitration & Bank Conflict Detection)
	/// </summary>
	class BankArbitrator : public std::enable_shared_from_this<BankArbitrator>
	{
	private:
		std::vector<bool> _busyBanks;
		const int _bankCount;
		const unsigned long long _bankSize;
		int _busyCount = 0;

		/// <summary>
		/// Initialize bank arbitrator with bank configuration
		/// </summary>
	public:
		BankArbitrator(int bankCount, unsigned long long bankSize);

		/// <summary>
		/// Calculate which bank an address belongs to
		/// </summary>
	private:
		int CalculateBank(unsigned long long address);

		/// <summary>
		/// Try to reserve a bank for the current cycle.
		/// Returns false if the bank is already busy (conflict detected).
		/// HLS: single array read + conditional write, O(1).
		/// </summary>
	public:
		bool TryReserveBank(unsigned long long address, int &bankId);

		/// <summary>
		/// Reset arbitrator state at the end of each cycle.
		/// Must be called once per cycle to clear busy bank tracking.
		/// HLS: fixed-iteration array clear.
		/// </summary>
		void ResetCycle();

		/// <summary>
		/// Get count of currently busy banks
		/// </summary>
		int GetBusyBankCount();
	};
}
