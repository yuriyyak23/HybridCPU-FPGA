#include "MemoryBankRouting.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	int MemoryBankRouting::ResolveStandaloneReadOnlyBank(unsigned long long offset, unsigned long long sourceLength)
	{
		if ((sourceLength < 8 || sourceLength > 4096) || offset > sourceLength - 8)
		{
			throw InvalidOperationException(L"Original single-page read-only source footprint required.");
		}
		return ResolveBankId(offset, 4096, 1);
	}

	unsigned long long MemoryBankRouting::getSchedulerVisibleUninitializedUseCount()
	{
		return static_cast<unsigned long long>(_schedulerVisibleUninitializedUseCount.load(std::memory_order_relaxed));
	}

	bool MemoryBankRouting::IsResolvedSchedulerVisibleBankId(int bankId)
	{
		return bankId >= 0;
	}

	void MemoryBankRouting::ResetTelemetryForTesting()
	{
		_schedulerVisibleUninitializedUseCount.store(0, std::memory_order_relaxed);
	}

	int MemoryBankRouting::ResolveSchedulerVisibleBankId(unsigned long long address)
	{
		const auto memory = Processor::Memory;
		if (memory && memory->getNumBanks() > 0 && memory->getBankWidthBytes() > 0)
		{
			return ResolveBankId(address, memory->getBankWidthBytes(), memory->getNumBanks());
		}

		_schedulerVisibleUninitializedUseCount.fetch_add(1, std::memory_order_relaxed);
		return UninitializedSchedulerVisibleBankId;
	}

	int MemoryBankRouting::ResolveBankId(unsigned long long address, int bankWidthBytes, int numBanks)
	{
		int resolvedBankWidthBytes = bankWidthBytes > 0 ? bankWidthBytes : DefaultBankWidthBytes;
		int resolvedNumBanks = numBanks > 0 ? numBanks : DefaultNumBanks;

		return static_cast<int>((address / static_cast<unsigned long long>(resolvedBankWidthBytes)) % static_cast<unsigned long long>(resolvedNumBanks));
	}
}
