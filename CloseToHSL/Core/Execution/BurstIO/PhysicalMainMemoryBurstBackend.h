#pragma once

#include "IBurstBackend.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Memory/MemoryCoherencyObserver.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Explicit physical burst backend wrapper for future backend-selection tests.
	/// It models physical main-memory burst access and is not installed as the
	/// current lane6 DSC or lane7 L7 execution path.
	/// </summary>
	class PhysicalMainMemoryBurstBackend final : public IBurstBackend
	{
	private:
		const std::shared_ptr<Processor::MainMemoryArea> _mainMemory;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MemoryCoherencyObserver? _coherencyObserver;
		const std::shared_ptr<MemoryCoherencyObserver> _coherencyObserver;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PhysicalMainMemoryBurstBackend(Processor.MainMemoryArea mainMemory, MemoryCoherencyObserver? coherencyObserver = null)
		PhysicalMainMemoryBurstBackend(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver = nullptr);

		std::shared_ptr<Processor::MainMemoryArea> getMainMemory() const;

		bool Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer) override;

		bool Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer) override;

		void RegisterAcceleratorDevice(unsigned long long deviceId, AcceleratorDMACapabilities capabilities) override;

		std::shared_ptr<DMATransferToken> InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size) override;

	private:
		bool HasExactMainMemoryRange(unsigned long long address, int size);
	};
}
