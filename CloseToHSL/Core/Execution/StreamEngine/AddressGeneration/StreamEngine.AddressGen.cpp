#include "StreamEngine.AddressGen.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::Execution
{

	unsigned long long AddressGen::Gen1D(unsigned long long baseAddr, unsigned long long elementIndex, unsigned short stride)
	{
		return baseAddr + (elementIndex * stride);
	}

	unsigned long long AddressGen::Gen2D(unsigned long long baseAddr, unsigned long long elementIndex, unsigned int rowLength, unsigned short rowStride, unsigned short colStride)
	{
		if (rowLength == 0)
		{
			return baseAddr; // Safety: avoid division by zero
		}

		unsigned long long row = elementIndex / rowLength;
		unsigned long long col = elementIndex % rowLength;

		return baseAddr + (row * rowStride) + (col * colStride);
	}

	bool AddressGen::Crosses4KBBoundary(unsigned long long startAddr, unsigned long long length)
	{
		unsigned long long endAddr = startAddr + length - 1;
		unsigned long long startPage = startAddr >> 12; // Divide by 4096
		unsigned long long endPage = endAddr >> 12;
		return startPage != endPage;
	}

	unsigned long long AddressGen::BytesTo4KBBoundary(unsigned long long addr)
	{
		unsigned long long offset = addr & 0xFFF; // Offset within 4KB page
		return 4096 - offset;
	}

	unsigned long long AddressGen::ComputeBurstLength(unsigned long long startAddr, int elementSize, unsigned long long maxElements)
	{
		if (elementSize <= 0)
		{
			return 0;
		}

		unsigned long long bytesAvailable = BytesTo4KBBoundary(startAddr);
		unsigned long long elementsInPage = bytesAvailable / static_cast<unsigned long long>(elementSize);

		// Return minimum of: elements in page, max elements, AXI4 limit (256)
		unsigned long long result = elementsInPage;
		if (result > maxElements)
		{
			result = maxElements;
		}
		if (result > 256)
		{
			result = 256; // AXI4 burst limit
		}

		return result;
	}

	bool AddressGen::IsWideningOpcode(unsigned int opCode)
	{
		Processor::CPU_Core::InstructionsEnum op = static_cast<Processor::CPU_Core::InstructionsEnum>(opCode);
		// Hardware-level decision: VDOT_FP8 widens from 1-byte to 4-byte,
		// VDOT.WIDE widens FP16/BF16 inputs from 2-byte elements to one FP32 scalar.
		return op == Processor::CPU_Core::InstructionsEnum::VDOT_FP8 || op == Processor::CPU_Core::InstructionsEnum::VDOT_WIDE;
	}

	unsigned int AddressGen::GetEffectiveElementSize(unsigned int opCode, PortType port, unsigned char baseElementSize)
	{
		// Ternary operator compiles to pure MUX in HLS:
		// If widening opcode AND destination port, use 4 bytes, else use base size
		return (IsWideningOpcode(opCode) && port == PortType::Destination) ? 4u : static_cast<unsigned int>(baseElementSize);
	}
}
