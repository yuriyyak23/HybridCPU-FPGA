#pragma once

#include <string>
#include <stdexcept>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Scoreboard entry type for distinguishing DMA from Outstanding Load/Store entries.
	/// HLS: 2-bit enum → 2 flip-flops per entry.
	/// </summary>
	enum class ScoreboardEntryType : unsigned char
	{
		/// <summary>Slot is free (no pending operation).</summary>
		Free = 0,

		/// <summary>DMA transaction pending (legacy behavior).</summary>
		Dma = 1,

		/// <summary>Outstanding Load pending (MSHR tracking).</summary>
		OutstandingLoad = 2,

		/// <summary>Outstanding Store pending (store-buffer tracking).</summary>
		OutstandingStore = 3
	};

	/// <summary>
	/// Micro-operation scheduler for Formally Safe Packing (FSP).
	///
	/// Manages 16 nomination ports (one per core in a Pod), a priority encoder
	/// for slot stealing, and per-VT scoreboards for DMA/MSHR tracking.
	///
	/// HLS design constraints:
	/// - Fixed-size arrays only (no dynamic allocation, no List/Dictionary)
	/// - Deterministic iteration order (ascending core ID = priority encoder)
	/// - All state fits in register file / LUTRAM (no BRAM for control path)
	/// - Single-cycle combinational paths for Nominate / TryStealSlot
	///
	/// Refactoring Pt. 1 integration:
	/// - globalHardwareMask (from MemorySubsystem.GetHardwareOccupancyMask128)
	///   is merged into legality checker conflict checks during PackBundle,
	///   replacing the ad-hoc SuppressLsu boolean bypass.
	///
	/// Refactoring Pt. 3 integration:
	/// - Universal Scoreboard: per-VT type-tagged entries track DMA, Outstanding Loads
	///   and Outstanding Stores. Memory bank IDs in scoreboard allow FSP and pipeline
	///   ID-stage to detect bank-level conflicts, preventing Load/Store injection when
	///   the target bank has in-flight MSHR entries for the same virtual thread.
	/// </summary>
	/// <summary>
	/// Exception thrown when a critical pipeline invariant is violated.
	/// Indicates an unrecoverable hardware fault in the emulator model.
	/// </summary>
	class PipelinePanicException : public std::runtime_error, public std::enable_shared_from_this<PipelinePanicException>
	{
	public:
		PipelinePanicException(const std::wstring &message);
	};
}

