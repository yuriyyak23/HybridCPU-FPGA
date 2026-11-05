#pragma once

#include "../../../NonRTL/Arch/InstructionClass.h"

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Legality Layer
// V5 Phase 2: Decoder Disentangling
// ─────────────────────────────────────────────────────────────────────────────

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core::Legality
{
	/// <summary>
	/// Read-only view of structural resource availability used during legality checking.
	/// <para>
	/// Consumed exclusively by <see cref="ILegalityChecker"/>.
	/// NOT accessible from the decoder or the instruction IR builder.
	/// </para>
	/// </summary>
	class IResourceState
	{
		/// <summary>
		/// Returns <see langword="true"/> when at least one execution unit of the given
		/// <paramref name="instructionClass"/> is available to accept work this cycle.
		/// </summary>
	public:
		virtual bool IsAvailable(InstructionClass instructionClass) = 0;

		/// <summary>
		/// Returns the number of idle execution units of the given <paramref name="instructionClass"/>.
		/// Returns 0 when the class is fully occupied.
		/// </summary>
		virtual int AvailableCount(InstructionClass instructionClass) = 0;
	};
}
