#include "IHardwareOccupancyInput.h"

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	HardwareOccupancySnapshot128 HardwareOccupancySnapshot128::getPermissive()
	{
		return HardwareOccupancySnapshot128(SafetyMask128::Zero, 0, 0, 2, 0xFFFF, 0xFFFF, 2, 0xFFFF, 0xFFFF, 2, 0xFFFF, 0xFFFF);
	}

	HardwareOccupancySnapshot128::HardwareOccupancySnapshot128(SafetyMask128 overloadedResources, long long samplingCycle, unsigned long long samplingEpoch, unsigned char memoryIssueBudget, unsigned short memoryBankBudgetAtLeastOneMask, unsigned short memoryBankBudgetAtLeastTwoMask, unsigned char readIssueBudget, unsigned short readBankBudgetAtLeastOneMask, unsigned short readBankBudgetAtLeastTwoMask, unsigned char writeIssueBudget, unsigned short writeBankBudgetAtLeastOneMask, unsigned short writeBankBudgetAtLeastTwoMask)
		: OverloadedResources(overloadedResources), SamplingCycle(samplingCycle), SamplingEpoch(samplingEpoch),
		  MemoryIssueBudget(memoryIssueBudget), MemoryBankBudgetAtLeastOneMask(memoryBankBudgetAtLeastOneMask), MemoryBankBudgetAtLeastTwoMask(memoryBankBudgetAtLeastTwoMask),
		  ReadIssueBudget(readIssueBudget), ReadBankBudgetAtLeastOneMask(readBankBudgetAtLeastOneMask), ReadBankBudgetAtLeastTwoMask(readBankBudgetAtLeastTwoMask),
		  WriteIssueBudget(writeIssueBudget), WriteBankBudgetAtLeastOneMask(writeBankBudgetAtLeastOneMask), WriteBankBudgetAtLeastTwoMask(writeBankBudgetAtLeastTwoMask)
	{
	}

	YAKSys_Hybrid_CPU::Core::SafetyMask128 HardwareOccupancySnapshot128::getOverloadedResources() const
	{
		return OverloadedResources;
	}

	long long HardwareOccupancySnapshot128::getSamplingCycle() const
	{
		return SamplingCycle;
	}

	unsigned long long HardwareOccupancySnapshot128::getSamplingEpoch() const
	{
		return SamplingEpoch;
	}

	unsigned char HardwareOccupancySnapshot128::getMemoryIssueBudget() const
	{
		return MemoryIssueBudget;
	}

	unsigned short HardwareOccupancySnapshot128::getMemoryBankBudgetAtLeastOneMask() const
	{
		return MemoryBankBudgetAtLeastOneMask;
	}

	unsigned short HardwareOccupancySnapshot128::getMemoryBankBudgetAtLeastTwoMask() const
	{
		return MemoryBankBudgetAtLeastTwoMask;
	}

	unsigned char HardwareOccupancySnapshot128::getReadIssueBudget() const
	{
		return ReadIssueBudget;
	}

	unsigned short HardwareOccupancySnapshot128::getReadBankBudgetAtLeastOneMask() const
	{
		return ReadBankBudgetAtLeastOneMask;
	}

	unsigned short HardwareOccupancySnapshot128::getReadBankBudgetAtLeastTwoMask() const
	{
		return ReadBankBudgetAtLeastTwoMask;
	}

	unsigned char HardwareOccupancySnapshot128::getWriteIssueBudget() const
	{
		return WriteIssueBudget;
	}

	unsigned short HardwareOccupancySnapshot128::getWriteBankBudgetAtLeastOneMask() const
	{
		return WriteBankBudgetAtLeastOneMask;
	}

	unsigned short HardwareOccupancySnapshot128::getWriteBankBudgetAtLeastTwoMask() const
	{
		return WriteBankBudgetAtLeastTwoMask;
	}

	unsigned char HardwareOccupancySnapshot128::GetMemoryBudgetForBank(int bankId) const
	{
		return GetBudgetForBank(getMemoryBankBudgetAtLeastOneMask(), getMemoryBankBudgetAtLeastTwoMask(), bankId);
	}

	unsigned char HardwareOccupancySnapshot128::GetReadBudgetForBank(int bankId) const
	{
		return GetBudgetForBank(getReadBankBudgetAtLeastOneMask(), getReadBankBudgetAtLeastTwoMask(), bankId);
	}

	unsigned char HardwareOccupancySnapshot128::GetWriteBudgetForBank(int bankId) const
	{
		return GetBudgetForBank(getWriteBankBudgetAtLeastOneMask(), getWriteBankBudgetAtLeastTwoMask(), bankId);
	}

	unsigned char HardwareOccupancySnapshot128::GetBudgetForBank(unsigned short bankBudgetAtLeastOneMask, unsigned short bankBudgetAtLeastTwoMask, int bankId)
	{
		if (static_cast<unsigned int>(bankId) >= 16)
		{
			return 0;
		}

		unsigned short bankMask = static_cast<unsigned short>(1 << bankId);
		if ((bankBudgetAtLeastTwoMask & bankMask) != 0)
		{
			return 2;
		}

		return (bankBudgetAtLeastOneMask & bankMask) != 0 ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0);
	}

	unsigned char HardwareOccupancySnapshot128::getStoreIssueBudget() const
	{
		return getWriteIssueBudget();
	}

	unsigned short HardwareOccupancySnapshot128::getStoreBankBudgetAtLeastOneMask() const
	{
		return getWriteBankBudgetAtLeastOneMask();
	}

	unsigned short HardwareOccupancySnapshot128::getStoreBankBudgetAtLeastTwoMask() const
	{
		return getWriteBankBudgetAtLeastTwoMask();
	}

	unsigned char HardwareOccupancySnapshot128::GetStoreBudgetForBank(int bankId) const
	{
		return GetWriteBudgetForBank(bankId);
	}
}
