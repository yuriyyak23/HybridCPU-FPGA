#include "IBurstBackend.h"

namespace YAKSys_Hybrid_CPU::Execution
{

	unsigned long long DMATransferToken::getDeviceId() const
	{
		return DeviceId;
	}

	void DMATransferToken::setDeviceId(unsigned long long value)
	{
		DeviceId = value;
	}

	unsigned long long DMATransferToken::getSourceAddress() const
	{
		return SourceAddress;
	}

	void DMATransferToken::setSourceAddress(unsigned long long value)
	{
		SourceAddress = value;
	}

	unsigned long long DMATransferToken::getDestinationAddress() const
	{
		return DestinationAddress;
	}

	void DMATransferToken::setDestinationAddress(unsigned long long value)
	{
		DestinationAddress = value;
	}

	int DMATransferToken::getSize() const
	{
		return Size;
	}

	void DMATransferToken::setSize(int value)
	{
		Size = value;
	}

	bool DMATransferToken::isComplete() const
	{
		return IsComplete;
	}

	void DMATransferToken::setComplete(bool value)
	{
		IsComplete = value;
	}

	int DMATransferToken::getCyclesRemaining() const
	{
		return CyclesRemaining;
	}

	void DMATransferToken::setCyclesRemaining(int value)
	{
		CyclesRemaining = value;
	}
}
