#include "VectorTransferRetireEffect.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VectorTransferRetireEffect::VectorTransferRetireEffect(unsigned int opcode, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride, std::span<const unsigned char> packedData)
		: Opcode(opcode), SourceAddress(sourceAddress), DestinationAddress(destinationAddress),
		  ElementCount(elementCount), ElementSize(elementSize), Stride(stride),
		  _packedData(packedData.begin(), packedData.end())
	{
	}
	unsigned int VectorTransferRetireEffect::getOpcode() const
	{
		return Opcode;
	}

	unsigned long long VectorTransferRetireEffect::getSourceAddress() const
	{
		return SourceAddress;
	}

	unsigned long long VectorTransferRetireEffect::getDestinationAddress() const
	{
		return DestinationAddress;
	}

	unsigned long long VectorTransferRetireEffect::getElementCount() const
	{
		return ElementCount;
	}

	int VectorTransferRetireEffect::getElementSize() const
	{
		return ElementSize;
	}

	unsigned short VectorTransferRetireEffect::getStride() const
	{
		return Stride;
	}

	std::vector<unsigned char> VectorTransferRetireEffect::getPackedData() const
	{
		return _packedData;
	}

	std::vector<unsigned char> VectorTransferRetireEffect::CopyPackedData()
	{
		return _packedData;
	}
}
