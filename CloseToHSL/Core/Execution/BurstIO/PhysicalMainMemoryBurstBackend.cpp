#include "PhysicalMainMemoryBurstBackend.h"
#include "AcceleratorRuntimeFailClosed.h"
#include <limits>

using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Execution
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PhysicalMainMemoryBurstBackend(Processor.MainMemoryArea mainMemory, MemoryCoherencyObserver? coherencyObserver = null)
	PhysicalMainMemoryBurstBackend::PhysicalMainMemoryBurstBackend(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver) : _mainMemory(mainMemory), _coherencyObserver(coherencyObserver)
	{
		ArgumentNullException::ThrowIfNull(mainMemory);
	}

	std::shared_ptr<Processor::MainMemoryArea> PhysicalMainMemoryBurstBackend::getMainMemory() const
	{
		return _mainMemory;
	}

	bool PhysicalMainMemoryBurstBackend::Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer)
	{
		static_cast<void>(deviceID);
		if (buffer.empty())
		{
			return true;
		}
		if (buffer.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
		{
			return false;
		}

		return HasExactMainMemoryRange(address, static_cast<int>(buffer.size())) && _mainMemory->TryReadPhysicalRange(address, buffer);
	}

	bool PhysicalMainMemoryBurstBackend::Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer)
	{
		static_cast<void>(deviceID);
		if (buffer.empty())
		{
			return true;
		}
		if (buffer.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
		{
			return false;
		}

		if (!HasExactMainMemoryRange(address, static_cast<int>(buffer.size())) || !_mainMemory->TryWritePhysicalRange(address, buffer))
		{
			return false;
		}

		if (_coherencyObserver != nullptr)
		{
			_coherencyObserver->NotifyWrite(std::make_shared<MemoryCoherencyWriteNotification>(address, static_cast<unsigned long long>(buffer.size()), 0, MemoryCoherencyWriteSourceKind::PhysicalBurstBackendWrite));
		}
		return true;
	}

	void PhysicalMainMemoryBurstBackend::RegisterAcceleratorDevice(unsigned long long deviceId, AcceleratorDMACapabilities capabilities)
	{
		AcceleratorRuntimeFailClosed::ThrowRegistrationNotSupported();
	}

	std::shared_ptr<DMATransferToken> PhysicalMainMemoryBurstBackend::InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size)
	{
		return AcceleratorRuntimeFailClosed::ThrowTransferNotSupported();
	}

	bool PhysicalMainMemoryBurstBackend::HasExactMainMemoryRange(unsigned long long address, int size)
	{
		if (size <= 0)
		{
			return false;
		}

		unsigned long long memoryLength = static_cast<unsigned long long>(_mainMemory->getLength());
		unsigned long long requestSize = static_cast<unsigned long long>(size);
		return requestSize <= memoryLength && address <= memoryLength - requestSize;
	}
}
