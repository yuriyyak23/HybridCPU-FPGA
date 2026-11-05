#pragma once

#include "../../Arch/DataTypeEnum.h"
#include "../../CloseToHSL/Core/Architecture/Registers/Architectural/RegisterIdentity.h"
#include "Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <string>
#include <limits>
#include <stdexcept>
#include <format>
#include "tangible_exception_helper.h"

using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// Centralized instruction encoder for VLIW instruction generation.
	///
	/// Design goals:
	/// - Single source of truth for instruction encoding
	/// - Eliminates duplication in compiler and emulator code
	/// - Provides type-safe instruction building
	/// - Supports extended addressing modes (Indexed, 2D) and new instruction flags
	/// - HLS-friendly: no dynamic allocation, clear data flow
	///
	/// Architecture contract:
	/// - VLIW instruction is 256 bits (32 bytes, 4×64-bit words)
	/// - Word 0: OpCode, DataType, PredicateMask, Flags, Immediate
	/// - Word 1: Destination/Source1 pointer or register IDs
	/// - Word 2: Source2 pointer or descriptor address
	/// - Word 3: StreamLength, Stride, RowStride
	/// </summary>
	class InstructionEncoder final
	{
	private:
		inline static unsigned char RequireArchReg(unsigned short regId, const std::wstring &paramName);

		inline static unsigned char RequireOptionalArchReg(unsigned short regId, const std::wstring &paramName);

		inline static unsigned int RequireUInt32(unsigned long long value, const std::wstring &paramName);

		inline static unsigned short RequireUInt16(unsigned int value, const std::wstring &paramName);

		/// <summary>
		/// Encode a scalar register operation using flat architectural register ids.
		/// StreamLength = 1, operands packed as register IDs in Word1.
		/// </summary>
	public:
		inline static VLIW_Instruction EncodeScalar(unsigned int opCode, DataTypeEnum dataType, unsigned short reg1, unsigned short reg2, unsigned short reg3, unsigned char predicateMask = 0, unsigned short immediate = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a scalar immediate operation using the canonical packed architectural
		/// register ABI: Word1 = (rd, rs1, x0), Immediate = signed payload.
		/// </summary>
		inline static VLIW_Instruction EncodeScalarImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, short immediate, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a scalar rotate-immediate operation with the Phase 02 canonical
		/// ABI: Word1 = (rd, rs1, x0), Immediate = unsigned imm6 payload.
		/// </summary>
		inline static VLIW_Instruction EncodeScalarRotateImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned short immediate6, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a scalar bitfield-immediate operation with the Phase 02 canonical
		/// ABI: Word1 = (rd, rs1, x0), Immediate = unsigned imm6 bit index.
		/// </summary>
		inline static VLIW_Instruction EncodeScalarBitfieldImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned short immediate6, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode SLLI.UW with the Phase 03 canonical .UW immediate ABI:
		/// Word1 = (rd, rs1, x0), Immediate = unsigned imm6 shift amount.
		/// </summary>
		inline static VLIW_Instruction EncodeScalarAddressGenerationImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned short immediate6, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a scalar unary register operation using the canonical packed
		/// architectural register ABI: Word1 = (rd, rs1, x0), Immediate = 0.
		/// </summary>
		inline static VLIW_Instruction EncodeScalarUnary(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a vector memory-to-memory operation with 1D strided addressing.
		/// </summary>
		inline static VLIW_Instruction EncodeVector1D(unsigned int opCode, DataTypeEnum dataType, unsigned long long destSrc1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned short stride = 0, unsigned char predicateMask = 0, unsigned short immediate = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a vector operation with 2D addressing pattern.
		/// </summary>
		inline static VLIW_Instruction EncodeVector2D(unsigned int opCode, DataTypeEnum dataType, unsigned long long destSrc1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned short colStride, unsigned short rowStride, unsigned int rowLength, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a vector operation with indexed (gather/scatter) addressing.
		/// Word2 contains descriptor address pointing to Indexed2SrcDesc structure.
		/// </summary>
		inline static VLIW_Instruction EncodeVectorIndexed(unsigned int opCode, DataTypeEnum dataType, unsigned long long destSrc1Ptr, unsigned long long descriptorAddr, unsigned long long streamLength, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a comparison instruction that generates a predicate mask.
		/// Result is written to predicate register specified in destination field.
		/// </summary>
		inline static VLIW_Instruction EncodeVectorComparison(unsigned int opCode, DataTypeEnum dataType, unsigned long long src1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned char destPredicateReg, unsigned short stride = 0, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a reduction operation (sum, min, max, etc.)
		/// Result is a scalar value written to specified register.
		/// </summary>
		inline static VLIW_Instruction EncodeVectorReduction(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned short destReg, unsigned long long streamLength, unsigned short stride = 0, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a canonical control-flow instruction.
		/// Branch/JAL targets use the signed PC-relative immediate field; JALR uses
		/// register base plus the same immediate. Absolute target transport is not
		/// encoded through Src2Pointer.
		/// </summary>
		inline static VLIW_Instruction EncodeControlFlow(unsigned int opCode, unsigned short reg1, unsigned short reg2, unsigned short reg3, short relativeOffset);

		/// <summary>
		/// Encode a system instruction (CPU control, VMX, etc.)
		/// </summary>
		inline static VLIW_Instruction EncodeSystem(unsigned int opCode, unsigned short reg1, unsigned long long param1, unsigned long long param2);

		/// <summary>
		/// Encode a dot-product instruction (VDOT_VV, VDOTU_VV, VDOTF_VV).
		/// Result is a scalar written to element 0 of destination vector.
		/// </summary>
		inline static VLIW_Instruction EncodeDotProduct(unsigned int opCode, DataTypeEnum dataType, unsigned long long destPtr, unsigned long long src1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned short stride = 0, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a predicative movement instruction (VCOMPRESS_VM, VEXPAND_VM).
		/// These operations pack/unpack elements based on predicate mask.
		/// </summary>
		inline static VLIW_Instruction EncodePredicativeMovement(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned long long destPtr, unsigned long long streamLength, unsigned char predicateMask, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a permutation/gather instruction (VPERMUTE_VV, VRGATHER_VV).
		/// Uses indexed addressing where index vector determines element ordering.
		/// Word2 contains descriptor address pointing to index vector.
		/// </summary>
		inline static VLIW_Instruction EncodePermutation(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned long long indexDescriptorAddr, unsigned long long destPtr, unsigned long long streamLength, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a slide instruction (VSLIDEUP_VI, VSLIDEDOWN_VI).
		/// Slides vector elements up or down by immediate offset.
		/// </summary>
		inline static VLIW_Instruction EncodeSlide(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned long long destPtr, unsigned long long streamLength, unsigned short slideOffset, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Encode a FMA (Fused Multiply-Add) instruction using TriOpDesc descriptor.
		/// Word1 = accumulator/destination base address.
		/// Word2 = TriOpDesc descriptor address in memory (holds SrcA, SrcB, StrideA, StrideB).
		/// </summary>
		/// <param name="opCode">FMA opcode (VFMADD_VV, VFMSUB_VV, VFNMADD_VV, VFNMSUB_VV)</param>
		/// <param name="dataType">Element data type</param>
		/// <param name="accDestPtr">Accumulator/destination base address</param>
		/// <param name="triOpDescAddr">Address of TriOpDesc descriptor in memory</param>
		/// <param name="streamLength">Number of elements to process</param>
		/// <param name="destStride">Byte stride for accumulator/destination (0 = packed)</param>
		/// <param name="predicateMask">Predicate register index (0 = all active)</param>
		/// <param name="tailAgnostic">Per-instruction tail-agnostic policy</param>
		/// <param name="maskAgnostic">Per-instruction mask-agnostic policy</param>
		inline static VLIW_Instruction EncodeFMA(unsigned int opCode, DataTypeEnum dataType, unsigned long long accDestPtr, unsigned long long triOpDescAddr, unsigned long long streamLength, unsigned short destStride = 0, unsigned char predicateMask = 0, bool tailAgnostic = false, bool maskAgnostic = false);

		/// <summary>
		/// Helper: Validate instruction encoding before execution.
		/// Returns true if instruction is well-formed.
		/// </summary>
		inline static bool ValidateInstruction(VLIW_Instruction const inst);

		/// <summary>
		/// Encode a CSR (Control and Status Register) read operation.
		/// Reads a CSR value into a scalar register.
		/// CSR address space: see Appendix A.2 (0x000=VL, 0x010=RoundingMode, 0x020=OverflowCount, etc.)
		/// </summary>
		/// <param name="csrAddr">CSR address (encoded in Immediate field)</param>
		/// <param name="destReg">Destination register for read value</param>
		inline static VLIW_Instruction EncodeCSRRead(unsigned short csrAddr, unsigned short destReg);

		/// <summary>
		/// Encode a CSR (Control and Status Register) write operation.
		/// Writes a value from the source register (packed in Word1) to the addressed CSR.
		/// CSR address space: see Appendix A.2.
		/// </summary>
		/// <param name="csrAddr">CSR address (encoded in Immediate field)</param>
		/// <param name="srcReg">Source register whose value will be written to the CSR</param>
		inline static VLIW_Instruction EncodeCSRWrite(unsigned short csrAddr, unsigned short srcReg);

		/// <summary>
		/// Encode a vector exception status clear operation.
		/// Clears all exception counters without affecting modes or flags.
		/// </summary>
		inline static VLIW_Instruction EncodeClearExceptionCounters();
	};
}
