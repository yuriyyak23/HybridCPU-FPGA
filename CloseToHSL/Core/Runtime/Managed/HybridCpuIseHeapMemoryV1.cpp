#include "HybridCpuIseHeapMemoryV1.h"

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public HybridCpuIseHeapMemoryV1(Processor.MainMemoryArea memory, ulong baseAddress, ulong sizeBytes)
	HybridCpuIseHeapMemoryV1::HybridCpuIseHeapMemoryV1(std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long baseAddress, unsigned long long sizeBytes)
		: _baseAddress(baseAddress), _sizeBytes(sizeBytes), _memory((memory != nullptr) ? memory : throw std::invalid_argument("memory"))
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: if (baseAddress == 0 || sizeBytes == 0 || baseAddress > ulong.MaxValue - sizeBytes || baseAddress + sizeBytes > checked((ulong)memory.Length))
		if (baseAddress == 0 || sizeBytes == 0 || baseAddress > (std::numeric_limits<unsigned long long>::max)() - sizeBytes || baseAddress + sizeBytes > static_cast<unsigned long long>(memory->getLength()))
		{
			throw std::out_of_range("Managed heap range must be fully materializable in the explicitly bound ISE memory instance.");
		}
	}

	unsigned long long HybridCpuIseHeapMemoryV1::BaseAddress() const
	{
		return _baseAddress;
	}

	unsigned long long HybridCpuIseHeapMemoryV1::SizeBytes() const
	{
		return _sizeBytes;
	}

	bool HybridCpuIseHeapMemoryV1::Clear(unsigned long long address, int byteCount)
	{
		if (!Range(address, byteCount))
		{
			return false;
		}
		std::vector<unsigned char> zeroes(static_cast<std::size_t>((std::min)(byteCount, 64 * 1024)));
		int remaining = byteCount;
		unsigned long long cursor = address;
		while (remaining != 0)
		{
			int count = static_cast<int>((std::min)(static_cast<std::size_t>(remaining), zeroes.size()));
			if (!_memory->TryWritePhysicalRange(cursor, std::span<const unsigned char>(zeroes.data(), static_cast<std::size_t>(count))))
			{
				return false;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: cursor = checked(cursor + (ulong)count);
			cursor = cursor + static_cast<unsigned long long>(count);
			remaining -= count;
		}
		return true;
	}

	bool HybridCpuIseHeapMemoryV1::Read(unsigned long long address, std::span<unsigned char> destination) const
	{
		return destination.size() <= static_cast<std::size_t>((std::numeric_limits<int>::max)()) && Range(address, static_cast<int>(destination.size())) && _memory->TryReadPhysicalRange(address, destination);
	}

	bool HybridCpuIseHeapMemoryV1::Write(unsigned long long address, std::span<const unsigned char> source)
	{
		return source.size() <= static_cast<std::size_t>((std::numeric_limits<int>::max)()) && Range(address, static_cast<int>(source.size())) && _memory->TryWritePhysicalRange(address, source);
	}

	bool HybridCpuIseHeapMemoryV1::Range(unsigned long long address, int byteCount) const
	{
		if (byteCount < 0 || address < BaseAddress())
		{
			return false;
		}
		unsigned long long relative = address - BaseAddress();
		return relative <= SizeBytes() && static_cast<unsigned long long>(byteCount) <= SizeBytes() - relative;
	}
}
