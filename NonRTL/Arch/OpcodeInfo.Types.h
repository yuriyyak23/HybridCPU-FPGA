#pragma once

#include "InstructionClass.h"
#include "SerializationClass.h"
#include <string>
#include <optional>

namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// Unified metadata for instruction opcodes.
	/// Provides architectural information about instructions for decoder/encoder/documentation.
	///
	/// Design goals:
	/// - Single source of truth for opcode properties
	/// - Support for extended instruction attributes (masks, indexed, reduction, etc.)
	/// - HLS-friendly (no dynamic allocation, pure data structure)
	/// - Extensible for future ISA additions without breaking existing code
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct OpcodeInfo
	enum class OpcodeCategory : unsigned char
	{
		Scalar = 0, // Scalar integer/FP operations
		Vector = 1, // Vector/stream operations
		Memory = 2, // Memory load/store
		ControlFlow = 3, // Jumps, calls, returns
		System = 4, // System control, VMX, interrupts
		Comparison = 5, // Comparison and mask generation
		BitManip = 6, // Bit manipulation and shifts
		Atomic = 7, // LR/SC/AMO atomic operations
		Privileged = 8 // ECALL/EBREAK/MRET/SRET/WFI
	};

	/// <summary>
	/// Instruction characteristic flags (bit flags)
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum InstructionFlags : ushort
	enum class InstructionFlags : int
	{
		None = 0,
		Vector = 1 << 0, // Vector/stream instruction
		Maskable = 1 << 1, // Supports predicate masking
		Reduction = 1 << 2, // Can perform reduction
		Indexed = 1 << 3, // Supports indexed addressing
		ModifiesFlags = 1 << 4, // Modifies CPU flags register
		UsesImmediate = 1 << 5, // Uses immediate value
		TwoOperand = 1 << 6, // Two-operand (destructive)
		ThreeOperand = 1 << 7, // Three-operand (non-destructive)
		MemoryRead = 1 << 8, // Reads from memory
		MemoryWrite = 1 << 9, // Writes to memory
		FloatingPoint = 1 << 10, // Floating-point operation
		Atomic = 1 << 11, // Atomic operation
		Privileged = 1 << 12, // Requires privileged mode
		MaskManipulation = 1 << 13, // Predicate-mask manipulation (VMAND/VMOR/VMXOR/VMNOT/VPOPC/VMSBF)
		WidenNarrowConvert = 1 << 14, // Widen/narrow/convert contour (Phase 05 VZEXT)
		Scan = 1 << 15 // Prefix scan contour (Phase 05A VSCAN.SUM)
	};

	constexpr InstructionFlags operator|(InstructionFlags left, InstructionFlags right) noexcept
	{
		return static_cast<InstructionFlags>(static_cast<int>(left) | static_cast<int>(right));
	}

	constexpr InstructionFlags operator&(InstructionFlags left, InstructionFlags right) noexcept
	{
		return static_cast<InstructionFlags>(static_cast<int>(left) & static_cast<int>(right));
	}

	class OpcodeInfo
	{
		/// <summary>
		/// Opcode value (matches InstructionsEnum)
		/// </summary>
	public:
		const unsigned int OpCode;

		/// <summary>
		/// Human-readable mnemonic (e.g., "VADD", "VSUB")
		/// </summary>
		const std::wstring Mnemonic;

		/// <summary>
		/// Category of instruction
		/// </summary>
		const OpcodeCategory Category;

		/// <summary>
		/// Number of source operands (0, 1, 2, or 3)
		/// </summary>
		const unsigned char OperandCount;

		/// <summary>
		/// Instruction characteristics flags
		/// </summary>
		const InstructionFlags Flags;

		/// <summary>
		/// Execution latency in cycles (minimum, for scalar operations)
		/// Vector operations scale by VL / VLMAX
		/// </summary>
		const unsigned char ExecutionLatency;

		/// <summary>
		/// Memory bandwidth requirement (bytes per element)
		/// 0 = no memory access, 1 = single operand, 2 = dual operand
		/// </summary>
		const unsigned char MemoryBandwidth;

		/// <summary>
		/// Canonical runtime routing class for the published opcode contour.
		/// </summary>
		const YAKSys_Hybrid_CPU::Arch::InstructionClass InstructionClass;

		/// <summary>
		/// Canonical runtime serialization class for the published opcode contour.
		/// </summary>
		const YAKSys_Hybrid_CPU::Arch::SerializationClass SerializationClass;

		OpcodeInfo(unsigned int opCode, const std::wstring &mnemonic, OpcodeCategory category, unsigned char operandCount, InstructionFlags flags, unsigned char executionLatency, unsigned char memoryBandwidth, const std::optional<YAKSys_Hybrid_CPU::Arch::InstructionClass> &instructionClass = std::nullopt, const std::optional<YAKSys_Hybrid_CPU::Arch::SerializationClass> &serializationClass = std::nullopt);

		/// <summary>
		/// Check if instruction is a vector/stream operation
		/// </summary>
		bool isVector() const;

		/// <summary>
		/// Check if instruction supports predication (masking)
		/// </summary>
		bool getSupportsMasking() const;

		/// <summary>
		/// Check if instruction can perform reduction operations
		/// </summary>
		bool getSupportsReduction() const;

		/// <summary>
		/// Check if instruction supports indexed (gather/scatter) addressing
		/// </summary>
		bool getSupportsIndexed() const;

		/// <summary>
		/// Check if instruction is a control flow operation
		/// </summary>
		bool isControlFlow() const;

		/// <summary>
		/// Check if instruction is a math or vector operation.
		/// True for Scalar ALU (integer/FP/shift), Vector, Comparison, and BitManip
		/// categories.  This is the table-driven replacement for the legacy
		/// <c>VLIW_Instruction.IsMathOrVector</c> opcode-range comparison (V6 B8/B9).
		/// </summary>
		bool isMathOrVector() const;

		/// <summary>
		/// Check if instruction modifies flags register
		/// </summary>
		bool getModifiesFlags() const;

	private:
		static YAKSys_Hybrid_CPU::Arch::InstructionClass ResolveInstructionClass(OpcodeCategory category);

		static YAKSys_Hybrid_CPU::Arch::SerializationClass ResolveSerializationClass(OpcodeCategory category, InstructionFlags flags);

		OpcodeInfo() : OpCode(0), Mnemonic(L""), Category(static_cast<YAKSys_Hybrid_CPU::Arch::OpcodeCategory>(0)), OperandCount(0), Flags(static_cast<YAKSys_Hybrid_CPU::Arch::InstructionFlags>(0)), ExecutionLatency(0), MemoryBandwidth(0), InstructionClass(static_cast<YAKSys_Hybrid_CPU::Arch::InstructionClass>(0)), SerializationClass(static_cast<YAKSys_Hybrid_CPU::Arch::SerializationClass>(0))
		{
		}
	};

	/// <summary>
	/// Instruction category classification
	/// </summary>

}
