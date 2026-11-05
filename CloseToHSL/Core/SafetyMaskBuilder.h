#pragma once

#include "Pipeline/Certificates/SafetyMask128.h"
#include "../../NonRTL/Arch/InstructionClass.h"
#include "../../NonRTL/Arch/SerializationClass.h"
#include "../../NonRTL/Arch/OpcodeInfo.Types.h"
#include "Pipeline/MicroOps/Internal/InternalOp.h"
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Builds a <see cref="SafetyMask128"/> from an instruction's canonical classification.
	/// Used to populate safety certificates attached to pipeline operations.
	///
	/// G37: All build paths are runtime-computable — legality is determined without
	///      mandatory compiler hints.  The opcode table and InternalOp category are
	///      the sole authoritative sources.
	///
	/// G38: SafetyMask128 is fully decoupled from opcode integer ranges and from
	///      broken identity fields.  Masks encode only the instruction-class / 
	///      serialization-class pair, or the InternalOpCategory / serialization pair
	///      when built from an <see cref="InternalOp"/>.
	/// </summary>
	class SafetyMaskBuilder final
	{
		// ─── Bit position assignments ─────────────────────────────────────────────
		// Low 64 bits: per-class base bits (one bit per InstructionClass value)
		// High 64 bits: per-serialization-class bits (one bit per SerializationClass value)

		/// <summary>
		/// Builds a <see cref="SafetyMask128"/> that encodes the canonical instruction class
		/// and serialization class in the low and high words respectively.
		/// The mask is a minimal certificate that uniquely identifies the instruction class
		/// pair and is used by the safety verifier during FSP scheduling.
		/// </summary>
	public:
		static SafetyMask128 BuildFromClass(InstructionClass instrClass, SerializationClass serialClass);

		// ─────────────────────────────────────────────────────────────────────
		// G37: Runtime-computable path — no compiler hints required.
		// ─────────────────────────────────────────────────────────────────────

		/// <summary>
		/// Builds a <see cref="SafetyMask128"/> from a raw opcode by querying the
		/// canonical <see cref="OpcodeRegistry"/> table at runtime.
		///
		/// No compiler hint is required.  If the opcode is not registered the
		/// returned mask is <see cref="SafetyMask128.Zero"/> — callers must treat
		/// a zero mask as unclassified / maximally conservative.
		///
		/// G37: Runtime legality computation without mandatory compiler hints.
		/// G38: Decoupled from opcode integer ranges; uses OpcodeRegistry table.
		/// </summary>
		static SafetyMask128 BuildFromOpcode(unsigned int opcode);

		// ─────────────────────────────────────────────────────────────────────
		// G38: InternalOp-based path — fully decoupled from opcode ranges.
		// ─────────────────────────────────────────────────────────────────────

		/// <summary>
		/// Builds a <see cref="SafetyMask128"/> from a canonical <see cref="InternalOp"/>.
		///
		/// The mask is derived solely from <see cref="InternalOp.Category"/> and the
		/// corresponding serialisation class — no opcode integer ranges are used.
		///
		/// G38: Fully decoupled from opcode ranges and identity fields.
		/// </summary>
		static SafetyMask128 BuildFromInternalOp(std::shared_ptr<InternalOp> op);

		// ─────────────────────────────────────────────────────────────────────
		// Private helpers
		// ─────────────────────────────────────────────────────────────────────

		/// <summary>
		/// Derives the canonical <see cref="InstructionClass"/> from an
		/// <see cref="OpcodeInfo"/> record.  Category flags are used directly;
		/// no opcode integer comparison is performed.
		/// </summary>
	private:
		static InstructionClass ClassifyOpcodeInfo(OpcodeInfo info);

		/// <summary>
		/// Converts an <see cref="InternalOpCategory"/> to its corresponding
		/// <see cref="InstructionClass"/>.
		/// </summary>
		static InstructionClass InternalOpCategoryToInstructionClass(InternalOpCategory category);

		/// <summary>
		/// Returns the canonical <see cref="SerializationClass"/> for a given
		/// <see cref="InstructionClass"/>.
		/// </summary>
		static SerializationClass SerializationClassForInstruction(InstructionClass instrClass);

		/// <summary>
		/// Returns the canonical <see cref="SerializationClass"/> for an
		/// <see cref="InternalOp"/>, taking into account ordering flags on atomics.
		///
		/// G38: Derived from op semantics, not from opcode integer ranges.
		/// </summary>
		static SerializationClass SerializationClassForInternalOp(std::shared_ptr<InternalOp> op);
	};
}
