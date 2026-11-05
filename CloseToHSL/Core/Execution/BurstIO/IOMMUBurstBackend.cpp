#include "IOMMUBurstBackend.h"
#include "../../Virtualization/Compatibility/Adapters/IO/YAKSys_Hybrid_CPU.Memory.IOMMU.h"
#include "AcceleratorRuntimeFailClosed.h"

namespace YAKSys_Hybrid_CPU::Execution
{

	bool IOMMUBurstBackend::Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer)
	{
		return YAKSys_Hybrid_CPU::Memory::IOMMU::ReadBurst(deviceID, address, buffer);
	}

	bool IOMMUBurstBackend::Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer)
	{
		return YAKSys_Hybrid_CPU::Memory::IOMMU::WriteBurst(deviceID, address, buffer);
	}

	void IOMMUBurstBackend::RegisterAcceleratorDevice(unsigned long long deviceId, AcceleratorDMACapabilities capabilities)
	{
		AcceleratorRuntimeFailClosed::ThrowRegistrationNotSupported();
	}

	std::shared_ptr<DMATransferToken> IOMMUBurstBackend::InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size)
	{
		return AcceleratorRuntimeFailClosed::ThrowTransferNotSupported();
	}
}
