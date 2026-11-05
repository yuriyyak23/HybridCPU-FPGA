#include "InstructionEncoder.h"
#include "YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Arch/DataType.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Arch
{

	inline unsigned char InstructionEncoder::RequireArchReg(unsigned short regId, const std::wstring &paramName)
	{
		if (regId > ArchRegId::MaxValue)
		{
			throw ArgumentOutOfRangeException(paramName, regId, std::wstring(L"Architectural register id must be in [0, ") + std::to_wstring(ArchRegId::MaxValue) + L"].");
		}

		return static_cast<unsigned char>(regId);
	}

	inline unsigned char InstructionEncoder::RequireOptionalArchReg(unsigned short regId, const std::wstring &paramName)
	{
		if (regId == VLIW_Instruction::NoReg || regId == VLIW_Instruction::NoArchReg)
		{
			return VLIW_Instruction::NoArchReg;
		}

		return RequireArchReg(regId, paramName);
	}

	inline unsigned int InstructionEncoder::RequireUInt32(unsigned long long value, const std::wstring &paramName)
	{
		if (value > std::numeric_limits<unsigned int>::max())
		{
			throw ArgumentOutOfRangeException(paramName, value, L"VLIW carrier field is 32 bits; silent truncation is forbidden.");
		}

		return static_cast<unsigned int>(value);
	}

	inline unsigned short InstructionEncoder::RequireUInt16(unsigned int value, const std::wstring &paramName)
	{
		if (value > std::numeric_limits<unsigned short>::max())
		{
			throw ArgumentOutOfRangeException(paramName, value, L"VLIW carrier field is 16 bits; silent truncation is forbidden.");
		}

		return static_cast<unsigned short>(value);
	}

	inline VLIW_Instruction InstructionEncoder::EncodeScalar(unsigned int opCode, DataTypeEnum dataType, unsigned short reg1, unsigned short reg2, unsigned short reg3, unsigned char predicateMask, unsigned short immediate, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setImmediate(immediate);
		inst.setStreamLength(1); // Scalar = single element
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		inst.setWord1(VLIW_Instruction::PackArchRegs(RequireArchReg(reg1, L"reg1"), RequireArchReg(reg2, L"reg2"), RequireArchReg(reg3, L"reg3")));

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeScalarImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, short immediate, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return EncodeScalar(opCode, dataType, destReg, srcReg, 0, predicateMask, unchecked((ushort)immediate), tailAgnostic, maskAgnostic);
		return EncodeScalar(opCode, dataType, destReg, srcReg, 0, predicateMask, static_cast<unsigned short>(immediate), tailAgnostic, maskAgnostic);
	}

	inline VLIW_Instruction InstructionEncoder::EncodeScalarRotateImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned short immediate6, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		if (immediate6 > 0x3F)
		{
			throw ArgumentOutOfRangeException(L"immediate6", immediate6, L"Scalar rotate-immediate payload must fit imm6 [0, 63].");
		}

		return EncodeScalar(opCode, dataType, destReg, srcReg, 0, predicateMask, immediate6, tailAgnostic, maskAgnostic);
	}

	inline VLIW_Instruction InstructionEncoder::EncodeScalarBitfieldImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned short immediate6, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		if (immediate6 > 0x3F)
		{
			throw ArgumentOutOfRangeException(L"immediate6", immediate6, L"Scalar bitfield-immediate payload must fit imm6 [0, 63].");
		}

		return EncodeScalar(opCode, dataType, destReg, srcReg, 0, predicateMask, immediate6, tailAgnostic, maskAgnostic);
	}

	inline VLIW_Instruction InstructionEncoder::EncodeScalarAddressGenerationImmediate(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned short immediate6, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		if (immediate6 > 0x3F)
		{
			throw ArgumentOutOfRangeException(L"immediate6", immediate6, L"Scalar address-generation immediate payload must fit imm6 [0, 63].");
		}

		return EncodeScalar(opCode, dataType, destReg, srcReg, 0, predicateMask, immediate6, tailAgnostic, maskAgnostic);
	}

	inline VLIW_Instruction InstructionEncoder::EncodeScalarUnary(unsigned int opCode, DataTypeEnum dataType, unsigned short destReg, unsigned short srcReg, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		return EncodeScalar(opCode, dataType, destReg, srcReg, 0, predicateMask, 0, tailAgnostic, maskAgnostic);
	}

	inline VLIW_Instruction InstructionEncoder::EncodeVector1D(unsigned int opCode, DataTypeEnum dataType, unsigned long long destSrc1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned short stride, unsigned char predicateMask, unsigned short immediate, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setImmediate(immediate);
		inst.setDestSrc1Pointer(destSrc1Ptr);
		inst.setSrc2Pointer(src2Ptr);
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setStride(stride);
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		// Clear special addressing flags
		inst.setIndexed(false);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeVector2D(unsigned int opCode, DataTypeEnum dataType, unsigned long long destSrc1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned short colStride, unsigned short rowStride, unsigned int rowLength, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setDestSrc1Pointer(destSrc1Ptr);
		inst.setSrc2Pointer(src2Ptr);
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setStride(colStride);
		inst.setRowStride(rowStride);
		inst.setImmediate(RequireUInt16(rowLength, L"rowLength")); // Store row length in immediate field
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		// Set 2D addressing flag
		inst.setIs2D(true);
		inst.setIndexed(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeVectorIndexed(unsigned int opCode, DataTypeEnum dataType, unsigned long long destSrc1Ptr, unsigned long long descriptorAddr, unsigned long long streamLength, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setDestSrc1Pointer(destSrc1Ptr);
		inst.setSrc2Pointer(descriptorAddr); // Points to descriptor in memory
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		// Set indexed addressing flag
		inst.setIndexed(true);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeVectorComparison(unsigned int opCode, DataTypeEnum dataType, unsigned long long src1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned char destPredicateReg, unsigned short stride, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask); // Input mask (for masked comparisons)
		inst.setDestSrc1Pointer(src1Ptr);
		inst.setSrc2Pointer(src2Ptr);
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setStride(stride);
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		// Store destination predicate register in immediate field
		inst.setImmediate(destPredicateReg);

		inst.setIndexed(false);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeVectorReduction(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned short destReg, unsigned long long streamLength, unsigned short stride, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setDestSrc1Pointer(srcPtr);
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setStride(stride);
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		// Set reduction flag
		inst.setReduction(true);

		// Store destination register in immediate field
		inst.setImmediate(destReg);

		inst.setIndexed(false);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeControlFlow(unsigned int opCode, unsigned short reg1, unsigned short reg2, unsigned short reg3, short relativeOffset)
	{
		if (!OpcodeRegistry::IsControlFlowOp(opCode))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Opcode 0x{0:X} is not a published control-flow opcode.", opCode), nameof(opCode));
			throw std::invalid_argument(std::format("Opcode 0x{0:0>X} is not a published control-flow opcode.", opCode));
		}

		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setWord1(VLIW_Instruction::PackArchRegs(RequireOptionalArchReg(reg1, L"reg1"), RequireOptionalArchReg(reg2, L"reg2"), RequireOptionalArchReg(reg3, L"reg3")));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: inst.Immediate = unchecked((ushort)relativeOffset);
		inst.setImmediate(static_cast<unsigned short>(relativeOffset));
		inst.setSrc2Pointer(0);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeSystem(unsigned int opCode, unsigned short reg1, unsigned long long param1, unsigned long long param2)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setWord1(VLIW_Instruction::PackArchRegs(RequireArchReg(reg1, L"reg1"), VLIW_Instruction::NoArchReg, VLIW_Instruction::NoArchReg));
		inst.setSrc2Pointer(param1);
		inst.setStreamLength(RequireUInt32(param2, L"param2"));

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeDotProduct(unsigned int opCode, DataTypeEnum dataType, unsigned long long destPtr, unsigned long long src1Ptr, unsigned long long src2Ptr, unsigned long long streamLength, unsigned short stride, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		if (destPtr != src1Ptr)
		{
			throw InvalidOperationException(std::wstring(L"EncodeDotProduct rejected a separate destination pointer for the current VDOT* ABI. ") + L"Runtime dot-product execution is limited to the 1D scalar-footprint contour where destination aliases source1; " + L"separate destination/accumulator transport must fail closed until a complete executable ABI is implemented.");
		}

		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setDestSrc1Pointer(src1Ptr); // First source
		inst.setSrc2Pointer(src2Ptr); // Second source
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setStride(stride);
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		// Set reduction flag (dot product reduces to scalar)
		inst.setReduction(true);

		inst.setIndexed(false);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodePredicativeMovement(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned long long destPtr, unsigned long long streamLength, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask); // Mask determines which elements to pack/unpack
		inst.setDestSrc1Pointer(srcPtr);
		inst.setSrc2Pointer(destPtr);
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		inst.setIndexed(false);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodePermutation(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned long long indexDescriptorAddr, unsigned long long destPtr, unsigned long long streamLength, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setDestSrc1Pointer(srcPtr); // Source data vector
		inst.setSrc2Pointer(indexDescriptorAddr); // Descriptor containing index vector
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		// Set indexed addressing flag (gather/permute use index vector)
		inst.setIndexed(true);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeSlide(unsigned int opCode, DataTypeEnum dataType, unsigned long long srcPtr, unsigned long long destPtr, unsigned long long streamLength, unsigned short slideOffset, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setDestSrc1Pointer(srcPtr);
		inst.setSrc2Pointer(destPtr);
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setImmediate(slideOffset); // Slide offset in immediate field
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		inst.setIndexed(false);
		inst.setIs2D(false);

		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeFMA(unsigned int opCode, DataTypeEnum dataType, unsigned long long accDestPtr, unsigned long long triOpDescAddr, unsigned long long streamLength, unsigned short destStride, unsigned char predicateMask, bool tailAgnostic, bool maskAgnostic)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(opCode);
		inst.setDataType(static_cast<unsigned char>(dataType));
		inst.setPredicateMask(predicateMask);
		inst.setDestSrc1Pointer(accDestPtr);
		inst.setSrc2Pointer(triOpDescAddr);
		inst.setStreamLength(RequireUInt32(streamLength, L"streamLength"));
		inst.setStride(destStride);
		inst.setTailAgnostic(tailAgnostic);
		inst.setMaskAgnostic(maskAgnostic);

		inst.setIndexed(false);
		inst.setIs2D(false);

		return inst;
	}

	inline bool InstructionEncoder::ValidateInstruction(VLIW_Instruction const inst)
	{
		// Check opcode is not zero (Nope is allowed)
		if (inst.getOpCode() == 0 && inst.getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::Nope))
		{
			return false;
		}

		// Check data type is valid
		if (!Arch::DataTypeUtils::IsValid(inst.getDataTypeValue()))
		{
			return false;
		}

		// Check predicate register index is in range (0-15)
		if (inst.getPredicateMask() > 15)
		{
			return false;
		}

		// Check flags are consistent
		if (inst.getIndexed() && inst.getIs2D())
		{
			return false; // Cannot be both indexed and 2D
		}

		return true;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeCSRRead(unsigned short csrAddr, unsigned short destReg)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		// Lower the assembler-style CSRR helper to canonical CSRRS rd, csr, x0.
		inst.setOpCode(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRS));
		inst.setDataType(static_cast<unsigned char>(DataTypeEnum::UINT32));
		inst.setStreamLength(1);
		inst.setImmediate(csrAddr);
		inst.setWord1(VLIW_Instruction::PackArchRegs(RequireArchReg(destReg, L"destReg"), 0, VLIW_Instruction::NoArchReg));
		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeCSRWrite(unsigned short csrAddr, unsigned short srcReg)
	{
		VLIW_Instruction inst = VLIW_Instruction();
		// Lower the assembler-style CSRW helper to canonical CSRRW x0, csr, rs1.
		inst.setOpCode(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRW));
		inst.setDataType(static_cast<unsigned char>(DataTypeEnum::UINT32));
		inst.setStreamLength(1);
		inst.setImmediate(csrAddr);
		inst.setWord1(VLIW_Instruction::PackArchRegs(VLIW_Instruction::NoArchReg, RequireArchReg(srcReg, L"srcReg"), VLIW_Instruction::NoArchReg));
		return inst;
	}

	inline VLIW_Instruction InstructionEncoder::EncodeClearExceptionCounters()
	{
		VLIW_Instruction inst = VLIW_Instruction();
		inst.setOpCode(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSR_CLEAR));
		inst.setDataType(static_cast<unsigned char>(DataTypeEnum::UINT32));
		inst.setStreamLength(1);
		return inst;
	}
}
