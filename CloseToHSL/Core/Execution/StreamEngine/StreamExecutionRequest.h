#pragma once

#include "../../../../Arch/DataTypeEnum.h"
#include "../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"

using namespace HybridCPU_ISE::Arch;

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Validated ingress projection for live stream execution.
	/// Carries only runtime-relevant facts and intentionally discards compat-only transport hints.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct StreamExecutionRequest
	class StreamExecutionRequest
	{
	private:
		const unsigned int OpCode;
		const DataTypeEnum DataTypeValue;
		const unsigned char PredicateMask;
		const unsigned long long DestSrc1Pointer;
		const unsigned long long Src2Pointer;
		const unsigned short Immediate;
		const unsigned int StreamLength;
		const unsigned short Stride;
		const unsigned short RowStride;
		const bool Indexed;
		const bool Is2D;
		const bool TailAgnostic;
		const bool MaskAgnostic;
		const bool Saturating;

	public:
		static constexpr unsigned char NoArchReg = VLIW_Instruction::NoArchReg;

		unsigned int getOpCode() const;

		DataTypeEnum getDataTypeValue() const;

		unsigned char getPredicateMask() const;

		unsigned long long getDestSrc1Pointer() const;

		unsigned long long getSrc2Pointer() const;

		unsigned short getImmediate() const;

		unsigned int getStreamLength() const;

		unsigned short getStride() const;

		unsigned short getRowStride() const;

		bool getIndexed() const;

		bool getIs2D() const;

		bool getTailAgnostic() const;

		bool getMaskAgnostic() const;

		bool getSaturating() const;

		bool isScalar() const;

		bool getRequiresRetireVisibleScalarCarrier() const;

		bool getRequiresPredicateStateCarrier() const;

		bool getRequiresMemoryVisibleCarrier() const;

		bool isUnsupportedScalarizedVectorHelperSurface() const;

		bool isUnsupportedZeroLengthHelperSurface() const;

		bool isUnsupportedControlHelperSurface() const;

	private:
		StreamExecutionRequest(unsigned int opCode, DataTypeEnum dataTypeValue, unsigned char predicateMask, unsigned long long destSrc1Pointer, unsigned long long src2Pointer, unsigned short immediate, unsigned int streamLength, unsigned short stride, unsigned short rowStride, bool indexed, bool is2D, bool tailAgnostic, bool maskAgnostic, bool saturating);

	public:
		static StreamExecutionRequest CreateValidatedCompatIngress(VLIW_Instruction const instruction);

		bool TryUnpackArchRegs(unsigned char &rd, unsigned char &rs1, unsigned char &rs2) const;

		StreamExecutionRequest() = default;
	};
}
