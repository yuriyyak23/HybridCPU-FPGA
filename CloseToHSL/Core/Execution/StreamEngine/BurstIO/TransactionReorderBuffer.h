#pragma once
#include <span>

#include <vector>

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Transaction Reorder Buffer for AXI4 burst requests.
	///
	/// Allows out-of-order completion of memory transactions while
	/// maintaining in-order retirement (commit) semantics:
	/// <list type="bullet">
	///   <item>Requests are allocated in program order.</item>
	///   <item>Responses may arrive in any order (based on bank availability).</item>
	///   <item>TRB retires (commits) entries strictly in allocation order.</item>
	/// </list>
	///
	/// <para><b>Key insight:</b> the VLIW core is In-Order, but the memory
	/// subsystem can be Out-of-Order without affecting architectural state.
	/// The TRB is the boundary between In-Order (core) and OoO (memory).</para>
	///
	/// <para><b>HLS:</b> fixed-size entry array with valid/complete bits.
	/// 16 entries × ~256 bits = 4096 bits → 2 BRAM18 or LUTRAM.
	/// <c>FindNextIssuable</c> maps to a 16-input priority encoder
	/// with bank_free masking (2 LUT levels).</para>
	/// </summary>
	class TransactionReorderBuffer
	{
	private:
		unsigned long long TotalTransactions = 0;
		unsigned long long ReorderedCompletions = 0;
		unsigned long long BankConflictStalls = 0;

		/// <summary>Maximum outstanding transactions (HLS: maps to BRAM).</summary>
	public:
		static constexpr int TRB_DEPTH = 16;

		/// <summary>
		/// TRB entry: tracks one outstanding burst transaction.
		/// HLS: fixed-size struct, no heap allocation.
		/// </summary>
	public:
		class TrbEntry
		{
			/// <summary>Unique transaction ID (monotonically increasing).</summary>
		public:
			unsigned long long TransactionId = 0;

			/// <summary>Source thread ID that initiated this transaction.</summary>
			int ThreadId = 0;

			/// <summary>Target address for the burst.</summary>
			unsigned long long Address = 0;

			/// <summary>Size in bytes.</summary>
			int Size = 0;

			/// <summary>True if read, false if write.</summary>
			bool IsRead = false;

			/// <summary>Which memory bank this targets (for conflict detection).</summary>
			int TargetBank = 0;

			/// <summary>Cycle when request was issued.</summary>
			unsigned long long IssueCycle = 0;

			/// <summary>Cycle when response was received.</summary>
			unsigned long long CompleteCycle = 0;

			/// <summary>Entry is occupied.</summary>
			bool Valid = false;

			/// <summary>Transaction has completed (data available).</summary>
			bool Complete = false;

			/// <summary>Scratch buffer slot where result data is stored.</summary>
			int CompletionSlot = 0;
		};

	private:
		std::vector<TrbEntry> _entries;
		unsigned long long _nextTransactionId = 0;
		int _head = 0; // Oldest entry (in-order commit pointer)
		int _tail = 0; // Newest entry (allocation pointer)
		int _count = 0;

		/// <summary>Total transactions allocated through this TRB.</summary>
	public:
		unsigned long long getTotalTransactions() const;
	private:
		void setTotalTransactions(unsigned long long value);

		/// <summary>Completions that arrived out-of-order (entry != head).</summary>
	public:
		unsigned long long getReorderedCompletions() const;
	private:
		void setReorderedCompletions(unsigned long long value);

		/// <summary>Times <see cref="FindNextIssuable"/> found all entries bank-blocked.</summary>
	public:
		unsigned long long getBankConflictStalls() const;
	private:
		void setBankConflictStalls(unsigned long long value);

		/// <summary>
		/// Create a new TRB with fixed-depth entry storage.
		/// </summary>
	public:
		TransactionReorderBuffer(int _ = 0);

		/// <summary>
		/// Allocate a TRB entry for a new burst transaction.
		/// Returns entry index, or -1 if TRB is full (backpressure).
		/// </summary>
		/// <param name="threadId">Thread that initiated the request.</param>
		/// <param name="address">Target burst address.</param>
		/// <param name="size">Burst size in bytes.</param>
		/// <param name="isRead">True for read, false for write.</param>
		/// <param name="targetBank">Destination memory bank.</param>
		/// <param name="currentCycle">Current simulation cycle.</param>
		/// <returns>Entry index (0–15), or -1 if full.</returns>
		int Allocate(int threadId, unsigned long long address, int size, bool isRead, int targetBank, unsigned long long currentCycle);

		/// <summary>
		/// Mark a transaction as complete (called when memory response arrives).
		/// Out-of-order: any valid entry can complete regardless of position.
		/// </summary>
		/// <param name="entryIndex">TRB slot to mark complete.</param>
		/// <param name="currentCycle">Cycle when completion arrived.</param>
		/// <param name="completionSlot">Scratch buffer slot holding result data.</param>
		void Complete(int entryIndex, unsigned long long currentCycle, int completionSlot = 0);

		/// <summary>
		/// Try to retire the oldest completed transaction (in-order commit).
		/// Only the head entry can retire, preserving program order.
		/// </summary>
		/// <param name="retired">Retired entry data (valid only when returning true).</param>
		/// <returns>True if head entry was complete and retired.</returns>
		bool TryRetire(TrbEntry &retired);

		/// <summary>
		/// Find next issuable transaction whose target bank is not busy.
		/// Scans from head towards tail; returns first entry that is
		/// valid, not yet complete, and whose bank is free.
		/// This is the "reorder" logic: skip bank-blocked entries.
		///
		/// HLS: priority encoder over (valid &amp; !complete &amp; bank_free) bits → 2 LUT levels.
		/// </summary>
		/// <param name="bankBusy">Per-bank busy flags (indexed by bank ID).</param>
		/// <returns>Entry index of next issuable transaction, or -1 if all blocked.</returns>
		int FindNextIssuable(std::span<const bool> bankBusy);
        int FindNextIssuable(const std::vector<bool>& bankBusy);
    private:
        template<class Flags> int FindNextIssuableImpl(const Flags& bankBusy);
    public:

		/// <summary>True if TRB has space for new transactions.</summary>
		bool getHasSpace() const;

		/// <summary>Number of currently allocated (not yet retired) entries.</summary>
		int getOutstandingCount() const;

		/// <summary>
		/// Read a TRB entry by index (for diagnostics).
		/// </summary>
		TrbEntry GetEntry(int index) const;
	};
}
