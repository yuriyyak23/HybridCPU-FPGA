#include "StreamExecutionRequest.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Execution
{

	unsigned int StreamExecutionRequest::getOpCode() const
	{
		return OpCode;
	}

	DataTypeEnum StreamExecutionRequest::getDataTypeValue() const
	{
		return DataTypeValue;
	}

	unsigned char StreamExecutionRequest::getPredicateMask() const
	{
		return PredicateMask;
	}

	unsigned long long StreamExecutionRequest::getDestSrc1Pointer() const
	{
		return DestSrc1Pointer;
	}

	unsigned long long StreamExecutionRequest::getSrc2Pointer() const
	{
		return Src2Pointer;
	}

	unsigned short StreamExecutionRequest::getImmediate() const
	{
		return Immediate;
	}

	unsigned int StreamExecutionRequest::getStreamLength() const
	{
		return StreamLength;
	}

	unsigned short StreamExecutionRequest::getStride() const
	{
		return Stride;
	}

	unsigned short StreamExecutionRequest::getRowStride() const
	{
		return RowStride;
	}

	bool StreamExecutionRequest::getIndexed() const
	{
		return Indexed;
	}

	bool StreamExecutionRequest::getIs2D() const
	{
		return Is2D;
	}

	bool StreamExecutionRequest::getTailAgnostic() const
	{
		return TailAgnostic;
	}

	bool StreamExecutionRequest::getMaskAgnostic() const
	{
		return MaskAgnostic;
	}

	bool StreamExecutionRequest::getSaturating() const
	{
		return Saturating;
	}

	bool StreamExecutionRequest::isScalar() const
	{
		return getStreamLength() <= 1;
	}

	bool StreamExecutionRequest::getRequiresRetireVisibleScalarCarrier() const
	{
		return (getStreamLength() == 1 && isScalar()) || getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC);
	}

	bool StreamExecutionRequest::getRequiresPredicateStateCarrier() const
	{
		if (getStreamLength() == 0 || isScalar())
		{
			return false;
		}

		return OpcodeRegistry::IsComparisonOp(getOpCode()) || (OpcodeRegistry::IsMaskManipOp(getOpCode()) && getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC));
	}

	bool StreamExecutionRequest::getRequiresMemoryVisibleCarrier() const
	{
		if (getStreamLength() == 0 || isScalar())
		{
			return false;
		}

		return getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC) && !getRequiresPredicateStateCarrier();
	}

	bool StreamExecutionRequest::isUnsupportedScalarizedVectorHelperSurface() const
	{
		return getStreamLength() == 1 && isScalar() && getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC) && OpcodeRegistry::IsVectorOp(getOpCode());
	}

	bool StreamExecutionRequest::isUnsupportedZeroLengthHelperSurface() const
	{
		return getStreamLength() == 0 && !getRequiresRetireVisibleScalarCarrier();
	}

	bool StreamExecutionRequest::isUnsupportedControlHelperSurface() const
	{
		return getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_SETUP) ||
			getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_START) ||
			getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_WAIT);
	}

	StreamExecutionRequest::StreamExecutionRequest(unsigned int opCode, DataTypeEnum dataTypeValue, unsigned char predicateMask, unsigned long long destSrc1Pointer, unsigned long long src2Pointer, unsigned short immediate, unsigned int streamLength, unsigned short stride, unsigned short rowStride, bool indexed, bool is2D, bool tailAgnostic, bool maskAgnostic, bool saturating)
		: OpCode(opCode), DataTypeValue(dataTypeValue), PredicateMask(predicateMask),
		  DestSrc1Pointer(destSrc1Pointer), Src2Pointer(src2Pointer), Immediate(immediate),
		  StreamLength(streamLength), Stride(stride), RowStride(rowStride), Indexed(indexed),
		  Is2D(is2D), TailAgnostic(tailAgnostic), MaskAgnostic(maskAgnostic), Saturating(saturating)
	{
	}

	StreamExecutionRequest StreamExecutionRequest::CreateValidatedCompatIngress(VLIW_Instruction const instruction)
	{
		// word3[50] and word3[49:48] are compat transport fields only.
		// Live execution projects the architecturally relevant payload once and then
		// ignores the VirtualThreadId hint completely. Retired policy-gap must already
		// be zero at production ingress and therefore fails closed here.
		unsigned long long validatedWord3 = VLIW_Instruction::ValidateWord3ForProductionIngress(instruction.getWord3());

		return StreamExecutionRequest(instruction.getOpCode(), instruction.getDataTypeValue(), instruction.getPredicateMask(), instruction.getDestSrc1Pointer(), instruction.getSrc2Pointer(), instruction.getImmediate(), static_cast<unsigned int>((validatedWord3 >> 16) & 0xFFFF'FFFFULL), static_cast<unsigned short>(validatedWord3 & 0xFFFFULL), static_cast<unsigned short>((validatedWord3 >> 51) & 0x1FFFULL), instruction.getIndexed(), instruction.getIs2D(), instruction.getTailAgnostic(), instruction.getMaskAgnostic(), instruction.getSaturating());
	}

	bool StreamExecutionRequest::TryUnpackArchRegs(unsigned char &rd, unsigned char &rs1, unsigned char &rs2) const
	{
		return VLIW_Instruction::TryUnpackArchRegs(getDestSrc1Pointer(), rd, rs1, rs2);
	}
}
