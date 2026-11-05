#pragma once

#include <vector>
#include <span>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Immutable RF-10.10 publication payload for one completed canonical
	/// VLOAD/VSTORE source read. The bounded retire window remains the only owner
	/// allowed to publish the packed bytes to the destination surface.
	/// </summary>
	class VectorTransferRetireEffect final : public std::enable_shared_from_this<VectorTransferRetireEffect>
	{
	private:
		const unsigned int Opcode;
		const unsigned long long SourceAddress;
		const unsigned long long DestinationAddress;
		const unsigned long long ElementCount;
		const int ElementSize;
		const unsigned short Stride;

		const std::vector<unsigned char> _packedData;

	public:
		VectorTransferRetireEffect(unsigned int opcode, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride, std::span<const unsigned char> packedData);

		unsigned int getOpcode() const;
		unsigned long long getSourceAddress() const;
		unsigned long long getDestinationAddress() const;
		unsigned long long getElementCount() const;
		int getElementSize() const;
		unsigned short getStride() const;
		std::vector<unsigned char> getPackedData() const;

		std::vector<unsigned char> CopyPackedData();
	};

}
