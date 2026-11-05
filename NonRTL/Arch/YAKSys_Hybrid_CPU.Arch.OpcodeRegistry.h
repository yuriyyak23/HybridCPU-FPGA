#pragma once

#include "OpcodeInfo.Types.h"
#include "Generated/GeneratedIsaShadowCatalog.g.h"
#include "InstructionClass.h"
#include "SerializationClass.h"
#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <vector>
#include <optional>
#include <format>

using namespace YAKSys_Hybrid_CPU::Arch::Generated;


namespace YAKSys_Hybrid_CPU::Arch
{
	class OpcodeRegistry final
	{
		/// <summary>
		/// Runtime compatibility facade for manifest-derived static opcode metadata.
		/// The independent copy preserves the public mutable-array boundary.
		/// </summary>
	public:
		static inline const std::vector<OpcodeInfo> Opcodes = GeneratedIsaCatalog::Opcodes;


		/// <summary>
		/// Lookup opcode information by opcode value.
		/// Returns null if opcode not found.
		/// </summary>
		static std::optional<OpcodeInfo> GetInfo(unsigned int opCode);

		static bool TryGetMnemonic(unsigned int opCode, std::wstring &mnemonic);

		static std::wstring GetMnemonicOrHex(unsigned int opCode);

		/// <summary>
		/// Returns <see langword="true"/> for canonical native L7-SDC lane7
		/// system-device command carriers.
		/// </summary>
		static bool IsSystemDeviceCommandOpcode(unsigned int opCode);

		/// <summary>
		/// Resolve canonical runtime semantics for a published opcode contour.
		/// Returns false when the opcode is not published by <see cref="Opcodes"/>.
		/// </summary>
		static bool TryGetPublishedSemantics(unsigned int opCode, InstructionClass &instructionClass, SerializationClass &serializationClass);

		/// <summary>
		/// Resolve canonical runtime semantics for a published opcode contour.
		/// Returns false when the opcode is not published by <see cref="Opcodes"/>.
		/// </summary>
		static bool TryGetPublishedSemantics(Processor::CPU_Core::InstructionsEnum opcode, InstructionClass &instructionClass, SerializationClass &serializationClass);

		/// <summary>
		/// Check if opcode is a vector instruction
		/// </summary>
		static bool IsVectorOp(unsigned int opCode);

		/// <summary>
		/// Returns <see langword="true"/> when the opcode requires the canonical
		/// decoder-to-runtime vector payload projection.
		/// This includes published vector instructions plus the retained vector
		/// transfer carriers that are still materialized through vector transport.
		/// </summary>
		static bool RequiresVectorPayloadProjection(unsigned int opCode);

		/// <summary>
		/// Returns <see langword="true"/> when the opcode uses bundle-PC-relative
		/// static control-flow targeting and therefore can project a canonical
		/// target address without consulting register state.
		/// </summary>
		static bool HasStaticRelativeControlFlowTarget(unsigned int opCode);

		/// <summary>
		/// Check if opcode supports masking
		/// </summary>
		static bool SupportsMasking(unsigned int opCode);

		/// <summary>
		/// Returns true only for the Phase 05B policy-bit contour selected for
		/// executable saturating arithmetic. Other opcodes with the raw saturating
		/// bit set must fail closed until separately scoped.
		/// </summary>
		static bool SupportsSaturatingAddPolicy(unsigned int opCode);

		/// <summary>
		/// Returns <see langword="true"/> when the opcode uses the packed flat-architectural
		/// Word1 register ABI for canonical decode / IR operand projection.
		/// This semantic query is intentionally classifier-driven and must not be derived
		/// from StreamLength or legacy scalar heuristics.
		/// </summary>
		static bool UsesPackedArchRegisterWord1(Processor::CPU_Core::InstructionsEnum opcode);

		static bool UsesPackedArchRegisterWord1(unsigned int opcode);

		/// <summary>
		/// Check if opcode is a math or vector instruction.
		/// Table-driven replacement for <c>VLIW_Instruction.IsMathOrVector</c> (V6 B8/B9).
		/// </summary>
		static bool IsMathOrVectorOp(unsigned int opCode);

		/// <summary>
		/// Check if opcode is a control flow instruction.
		/// Table-driven replacement for <c>VLIW_Instruction.IsControlFlow</c> (V6 B7).
		/// </summary>
		static bool IsControlFlowOp(unsigned int opCode);

		/// <summary>
		/// Check if opcode is a vector comparison instruction that generates a predicate mask.
		/// Table-driven replacement for the VCMPEQ–VCMPGE opcode-range check (V6 B9).
		/// </summary>
		static bool IsComparisonOp(unsigned int opCode);

		/// <summary>
		/// Check if opcode is a predicate-mask manipulation instruction (VMAND/VMOR/VMXOR/VMNOT/VPOPC/VMSBF).
		/// Table-driven replacement for the VMAND–VPOPC opcode-range check (V6 B9).
		/// </summary>
		static bool IsMaskManipOp(unsigned int opCode);

		/// <summary>
		/// Check if opcode is a scoped widen/narrow/convert vector contour.
		/// </summary>
		static bool IsWidenNarrowConvertOp(unsigned int opCode);

		/// <summary>
		/// Check if opcode is a scoped prefix-scan vector contour.
		/// </summary>
		static bool IsScanOp(unsigned int opCode);

		/// <summary>
		/// Check if opcode is a fused multiply-add instruction (VFMADD/VFMSUB/VFNMADD/VFNMSUB).
		/// Table-driven replacement for the VFMADD–VFNMSUB opcode-range check (V6 B9).
		/// </summary>
		static bool IsFmaOp(unsigned int opCode);

		/// <summary>
		/// Check if opcode is a vector reduction instruction (VREDSUM–VREDXOR).
		/// Table-driven replacement for the VREDSUM–VREDXOR opcode-range check (V6 B9).
		/// </summary>
		static bool IsReductionOp(unsigned int opCode);


	private:
		static bool IsVectorConfigSystemSingletonOpcode(Processor::CPU_Core::InstructionsEnum opcode);

		static bool IsVectorConfigSystemSingletonOpcode(unsigned int opcode);

		static bool UsesClassifierOnlyPackedArchRegisterWord1(Processor::CPU_Core::InstructionsEnum opcode);

		static bool UsesClassifierOnlyPackedArchRegisterWord1(unsigned int opcode);
	};
}
