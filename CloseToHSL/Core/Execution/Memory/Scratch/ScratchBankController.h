#pragma once

#include <vector>
#include <span>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Banked Scratch Buffer Controller with crossbar interconnect.
	///
	/// Models a 4-bank interleaved SRAM with 2R+1W access per cycle.
	/// Each bank is True Dual-Port BRAM (1R+1W simultaneous).
	/// Crossbar routes element addresses to banks by addr[1:0].
	///
	/// <para><b>Conflict detection:</b>
	/// If two reads or a read+write hit the same bank in one cycle → 1-cycle stall.
	/// Stall model is cycle-accurate for timing simulation.</para>
	///
	/// <para><b>Bank mapping (low-bit interleaving):</b></para>
	/// <list type="bullet">
	///   <item>Address[1:0] = 00 → Bank 0</item>
	///   <item>Address[1:0] = 01 → Bank 1</item>
	///   <item>Address[1:0] = 10 → Bank 2</item>
	///   <item>Address[1:0] = 11 → Bank 3</item>
	/// </list>
	///
	/// <para><b>HLS:</b> 4 × BRAM18 (64 bytes each) + 4×3 crossbar mux (~60 LUT6).</para>
	/// </summary>
	class ScratchBankController
	{
	private:
		unsigned long long BankConflicts = 0;
		unsigned long long TotalAccesses = 0;

		static constexpr int BANK_COUNT = 4;
		static constexpr int ELEMENTS_PER_BANK = 8; // 32 elements / 4 banks
		static constexpr int MAX_ELEMENT_SIZE = 8; // 64-bit elements
		static constexpr int BANK_SIZE = ELEMENTS_PER_BANK * MAX_ELEMENT_SIZE; // 64 bytes

		/// <summary>4 independent SRAM banks.</summary>
		std::vector<std::vector<unsigned char>> _banks;

		/// <summary>True when <see cref="Initialize"/> has been called.</summary>
	public:
		bool isInitialized() const;

		/// <summary>Bank conflicts detected (2+ accesses hit same bank in one cycle).</summary>
		unsigned long long getBankConflicts() const;
	private:
		void setBankConflicts(unsigned long long value);

		/// <summary>Total element accesses.</summary>
	public:
		unsigned long long getTotalAccesses() const;
	private:
		void setTotalAccesses(unsigned long long value);

		/// <summary>Initialize banked storage (call once during core init).</summary>
	public:
		void Initialize();

		/// <summary>
		/// Map element index to (bank, byte offset) using low-bit interleaving.
		/// HLS: wire select, zero combinational logic.
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (int bank, int offset) MapAddress(int elementIndex, int elementSize)
//		{
//			int bank = elementIndex & (BANK_COUNT - 1); // bits [1:0]
//			int bankElement = elementIndex >> 2; // bits [N:2]
//			return (bank, bankElement * elementSize);
//		}

		/// <summary>
		/// Read a single element from banked storage.
		/// </summary>
		void ReadElement(int elementIndex, int elementSize, std::span<unsigned char> dest);

		/// <summary>
		/// Write a single element to banked storage.
		/// </summary>
		void WriteElement(int elementIndex, int elementSize, std::span<const unsigned char> src);

		/// <summary>
		/// Check if three simultaneous accesses (2R + 1W) have bank conflicts.
		/// Returns stall cycles: 0 = no conflict, 1 = one-cycle stall.
		///
		/// HLS: 3 bank-index comparators → single-cycle combinational logic.
		/// </summary>
		int CheckConflict(int readAIndex, int readBIndex, int writeDstIndex);

		/// <summary>
		/// Execute a 2R+1W vector triple access with conflict detection.
		/// Reads elements from bank A and B, writes result to bank Dst.
		/// If conflict detected: data is still transferred (sequentialized),
		/// return value indicates stall penalty.
		/// </summary>
		/// <param name="readAElement">Element index for read operand A.</param>
		/// <param name="readBElement">Element index for read operand B.</param>
		/// <param name="writeDstElement">Element index for write destination.</param>
		/// <param name="elementSize">Element size in bytes.</param>
		/// <param name="destA">Buffer to receive operand A data.</param>
		/// <param name="destB">Buffer to receive operand B data.</param>
		/// <param name="srcDst">Source data to write to destination.</param>
		/// <returns>Number of stall cycles (0 or 1).</returns>
		int ExecuteVectorTripleAccess(int readAElement, int readBElement, int writeDstElement, int elementSize, std::span<unsigned char> destA, std::span<unsigned char> destB, std::span<const unsigned char> srcDst);

		/// <summary>
		/// Bulk load from contiguous external memory into banked scratch.
		/// Elements are distributed across banks by interleaving.
		/// </summary>
		/// <param name="source">Source memory span.</param>
		/// <param name="elementSize">Element size in bytes.</param>
		/// <param name="elementCount">Number of elements to load.</param>
		void LoadFromMemory(std::span<const unsigned char> source, int elementSize, int elementCount);

		/// <summary>
		/// Bulk store from banked scratch to contiguous external memory.
		/// </summary>
		/// <param name="dest">Destination memory span.</param>
		/// <param name="elementSize">Element size in bytes.</param>
		/// <param name="elementCount">Number of elements to store.</param>
		void StoreToMemory(std::span<unsigned char> dest, int elementSize, int elementCount);
	};
}
