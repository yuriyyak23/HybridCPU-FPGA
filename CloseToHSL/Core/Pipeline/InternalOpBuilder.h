#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "IInternalOpBuilder.h"
#include "MicroOps/Internal/InternalOp.h"
#include <string>
#include <optional>
#include <format>
#include <memory>

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Pipeline
// V5 Phase 2: Decoder Disentangling
// ─────────────────────────────────────────────────────────────────────────────

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
// C# TO C++ CONVERTER TASK: C# to C++ Converter could not confirm whether this is a namespace alias or a type alias:
//using IsaOpcode = YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcode;

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{
	/// <summary>
	/// Default <see cref="IInternalOpBuilder"/> implementation.
	/// <para>
	/// Provides the single authoritative mapping from canonical numeric opcodes to
	/// <see cref="InternalOpKind"/>.  All execution units derive their dispatch selector
	/// from this table — there must be no independent mapping tables elsewhere in the pipeline.
	/// </para>
	/// </summary>
	class InternalOpBuilder final : public IInternalOpBuilder
	{
		/// <inheritdoc />
	public:
		std::shared_ptr<InternalOp> Build(std::shared_ptr<InstructionIR> instruction) override;

		// ─────────────────────────────────────────────────────────────────────
		// Opcode → InternalOpKind table
		// ─────────────────────────────────────────────────────────────────────

		/// <summary>
		/// Maps every canonical ISA v4 opcode to its abstract <see cref="InternalOpKind"/>.
		/// </summary>
		static InternalOpKind MapToKind(unsigned short opcode);

		// ─────────────────────────────────────────────────────────────────────
		// Helpers
		// ─────────────────────────────────────────────────────────────────────

	private:
		static InternalOpDataType ResolveDataType(unsigned short opcode, std::shared_ptr<InstructionIR> instruction);

		static std::optional<unsigned short> ResolveCsrTarget(unsigned short opcode, std::shared_ptr<InstructionIR> instruction);

		static InternalOpFlags DeriveFlags(unsigned short opcode);

		static InternalOpFlags DeriveAtomicOrderingFlags(InternalOpKind kind, std::shared_ptr<InstructionIR> instruction);
	};
}
