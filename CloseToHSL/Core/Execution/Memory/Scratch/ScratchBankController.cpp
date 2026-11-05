#include "ScratchBankController.h"
#include <algorithm>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{

	bool ScratchBankController::isInitialized() const
	{
		return !_banks.empty();
	}

	unsigned long long ScratchBankController::getBankConflicts() const
	{
		return BankConflicts;
	}

	void ScratchBankController::setBankConflicts(unsigned long long value)
	{
		BankConflicts = value;
	}

	unsigned long long ScratchBankController::getTotalAccesses() const
	{
		return TotalAccesses;
	}

	void ScratchBankController::setTotalAccesses(unsigned long long value)
	{
		TotalAccesses = value;
	}

	void ScratchBankController::Initialize()
	{
		_banks = std::vector<std::vector<unsigned char>>(BANK_COUNT);
		for (int i = 0; i < BANK_COUNT; i++)
		{
			_banks[i] = std::vector<unsigned char>(BANK_SIZE);
		}
		setBankConflicts(0);
		setTotalAccesses(0);
	}

    void ScratchBankController::ReadElement(int elementIndex, int elementSize, std::span<unsigned char> dest)
    {
        if (elementIndex < 0 || elementSize < 0) throw std::out_of_range("Invalid scratch element.");
        const auto bank = static_cast<std::size_t>(elementIndex & (BANK_COUNT - 1));
        const auto offset = static_cast<std::size_t>(elementIndex >> 2) * static_cast<std::size_t>(elementSize);
        const auto size = static_cast<std::size_t>(elementSize);
        if (bank >= _banks.size() || offset > _banks[bank].size() || size > _banks[bank].size() - offset || dest.size() < size) throw std::out_of_range("Scratch read out of range.");
        std::copy_n(_banks[bank].begin() + offset, size, dest.begin());
        setTotalAccesses(getTotalAccesses() + 1);
    }
    void ScratchBankController::WriteElement(int elementIndex, int elementSize, std::span<const unsigned char> src)
    {
        if (elementIndex < 0 || elementSize < 0) throw std::out_of_range("Invalid scratch element.");
        const auto bank = static_cast<std::size_t>(elementIndex & (BANK_COUNT - 1));
        const auto offset = static_cast<std::size_t>(elementIndex >> 2) * static_cast<std::size_t>(elementSize);
        const auto size = static_cast<std::size_t>(elementSize);
        if (bank >= _banks.size() || offset > _banks[bank].size() || size > _banks[bank].size() - offset || src.size() > size) throw std::out_of_range("Scratch write out of range.");
        std::copy(src.begin(), src.end(), _banks[bank].begin() + offset);
        setTotalAccesses(getTotalAccesses() + 1);
    }
	int ScratchBankController::CheckConflict(int readAIndex, int readBIndex, int writeDstIndex)
	{
		int bankA = readAIndex & (BANK_COUNT - 1);
		int bankB = readBIndex & (BANK_COUNT - 1);
		int bankDst = writeDstIndex & (BANK_COUNT - 1);

		bool conflict = (bankA == bankB) || (bankA == bankDst) || (bankB == bankDst);
		if (conflict)
		{
			setBankConflicts(getBankConflicts() + 1);
			return 1;
		}
		return 0;
	}

	int ScratchBankController::ExecuteVectorTripleAccess(int readAElement, int readBElement, int writeDstElement, int elementSize, std::span<unsigned char> destA, std::span<unsigned char> destB, std::span<const unsigned char> srcDst)
	{
		int stallCycles = CheckConflict(readAElement, readBElement, writeDstElement);

		ReadElement(readAElement, elementSize, destA);
		ReadElement(readBElement, elementSize, destB);
		WriteElement(writeDstElement, elementSize, srcDst);

		return stallCycles;
	}

	void ScratchBankController::LoadFromMemory(std::span<const unsigned char> source, int elementSize, int elementCount)
	{
		for (int i = 0; i < elementCount && (i + 1) * elementSize <= source.size(); i++)
		{
			std::span<const unsigned char> element = source.subspan(i * elementSize, elementSize);
			WriteElement(i, elementSize, element);
		}
	}

	void ScratchBankController::StoreToMemory(std::span<unsigned char> dest, int elementSize, int elementCount)
	{
		for (int i = 0; i < elementCount && (i + 1) * elementSize <= dest.size(); i++)
		{
			std::span<unsigned char> element = dest.subspan(i * elementSize, elementSize);
			ReadElement(i, elementSize, element);
		}
	}
}
