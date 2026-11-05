#pragma once
#include <span>

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Decoder/IDecoderFrontend.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../Contracts/CompilerTransport/VliwBundleAnnotations.h"
#include "../../../Decoder/DecodedInstructionBundle.h"
#include <string>
#include <optional>
#include <format>
#include <memory>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	/// <summary>
	/// HybridCPU ISA v4 native VLIW decoder public façade.
	/// It preserves <see cref="IDecoderFrontend"/> and legacy
	/// <see cref="InstructionIR"/> projection compatibility while delegating
	/// decode-owned static legality to the declarative canonical pipeline.
	/// </summary>
	class VliwDecoderV4 final : public IDecoderFrontend
	{
		/// <summary>
		/// Decode a single instruction from a VLIW bundle slot.
		/// </summary>
		/// <param name="instruction">Raw VLIW instruction from the bundle slot.</param>
		/// <param name="slotIndex">Physical slot index (0–7) within the bundle.</param>
		/// <returns>Canonical <see cref="InstructionIR"/> compatibility projection.</returns>
		/// <exception cref="InvalidOpcodeException">
		/// Thrown when a decode-owned static encoding is illegal. The internal
		/// <see cref="DecodeFailure"/> remains available only inside the
		/// declarative pipeline.
		/// </exception>
	public:
		std::shared_ptr<InstructionIR> Decode(VLIW_Instruction const instruction, int slotIndex) override;

		/// <summary>
		/// Decode one frontend bundle into the canonical Phase 03 bundle contract.
		/// </summary>
		/// <param name="bundle">Span of raw VLIW instructions (one per slot).</param>
		/// <param name="bundleAnnotations">Frozen source-sideband annotations, when present.</param>
		/// <param name="bundleAddress">PC address of the first byte of the decoded bundle.</param>
		/// <param name="bundleSerial">Bundle serial used by downstream pipeline tracking.</param>
		/// <returns>
		/// Canonical <see cref="DecodedInstructionBundle"/> with one logical slot per
		/// physical bundle slot and legacy <see cref="InstructionIR"/> adapters.
		/// </returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DecodedInstructionBundle DecodeInstructionBundle(System.ReadOnlySpan<VLIW_Instruction> bundle, VliwBundleAnnotations? bundleAnnotations, ulong bundleAddress, ulong bundleSerial = 0, System.Nullable<CanonicalDecodeContext> decodeContext = null)
		std::shared_ptr<DecodedInstructionBundle> DecodeInstructionBundle(std::span<const VLIW_Instruction> bundle, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, unsigned long long bundleAddress, unsigned long long bundleSerial = 0, std::optional<std::shared_ptr<CanonicalDecodeContext>> decodeContext = std::nullopt) override;

		/// <summary>
		/// Decode one frontend bundle using neutral/default slot metadata when no
		/// sideband bundle annotations are available.
		/// </summary>
		std::shared_ptr<DecodedInstructionBundle> DecodeInstructionBundle(std::span<const VLIW_Instruction> bundle, unsigned long long bundleAddress, unsigned long long bundleSerial = 0) override;

		/// <summary>
		/// Returns <see langword="true"/> if <paramref name="opcode"/> is listed as
		/// prohibited in <see cref="IsaV4Surface.ProhibitedOpcodes"/>.
		/// </summary>
		static bool IsProhibited(unsigned short opcode);

		/// <summary>
		/// Returns <see langword="true"/> if the string <paramref name="opcodeName"/> is
		/// listed in <see cref="IsaV4Surface.ProhibitedOpcodes"/>.
		/// </summary>
		static bool IsProhibited(const std::wstring &opcodeName);

	private:
		static bool IsProhibited(unsigned short opcode, const std::wstring &opcodeName);

		static bool TryParseOpcodeIdentifier(const std::wstring &opcodeIdentifier, unsigned short &opcode);
	};
}
