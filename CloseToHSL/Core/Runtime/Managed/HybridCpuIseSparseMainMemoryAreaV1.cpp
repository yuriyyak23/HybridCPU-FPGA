#include "HybridCpuIseSparseMainMemoryAreaV1.h"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{
	HybridCpuIseSparseMainMemoryAreaV1::HybridCpuIseSparseMainMemoryAreaV1(long long length)
		: _length(length)
	{
		if (length <= 0 || length % PageSize != 0)
			throw std::out_of_range("length");
	}

	long long HybridCpuIseSparseMainMemoryAreaV1::getLength() const
	{
		return _length;
	}

	bool HybridCpuIseSparseMainMemoryAreaV1::TryReadPhysicalRange(
		unsigned long long physicalAddress, std::span<unsigned char> buffer)
	{
		if (!InRange(physicalAddress, buffer.size()))
			return false;

		for (std::size_t offset = 0; offset < buffer.size();)
		{
			const unsigned long long address = physicalAddress + static_cast<unsigned long long>(offset);
			const unsigned long long pageIndex = address / PageSize;
			const std::size_t pageOffset = static_cast<std::size_t>(address % PageSize);
			const std::size_t count = std::min(static_cast<std::size_t>(PageSize) - pageOffset,
				buffer.size() - offset);
			const auto page = _pages.find(pageIndex);
			if (page != _pages.end())
				std::copy_n(page->second.begin() + static_cast<std::ptrdiff_t>(pageOffset), count,
					buffer.begin() + static_cast<std::ptrdiff_t>(offset));
			else
				std::fill_n(buffer.begin() + static_cast<std::ptrdiff_t>(offset), count, 0);
			offset += count;
		}
		return true;
	}

	bool HybridCpuIseSparseMainMemoryAreaV1::TryWritePhysicalRange(
		unsigned long long physicalAddress, std::span<const unsigned char> buffer)
	{
		if (!InRange(physicalAddress, buffer.size()))
			return false;

		for (std::size_t offset = 0; offset < buffer.size();)
		{
			const unsigned long long address = physicalAddress + static_cast<unsigned long long>(offset);
			const unsigned long long pageIndex = address / PageSize;
			const std::size_t pageOffset = static_cast<std::size_t>(address % PageSize);
			const std::size_t count = std::min(static_cast<std::size_t>(PageSize) - pageOffset,
				buffer.size() - offset);
			auto [page, inserted] = _pages.try_emplace(pageIndex, PageSize, 0);
			(void)inserted;
			std::copy_n(buffer.begin() + static_cast<std::ptrdiff_t>(offset), count,
				page->second.begin() + static_cast<std::ptrdiff_t>(pageOffset));
			offset += count;
		}
		return true;
	}

	unsigned long long HybridCpuIseSparseMainMemoryAreaV1::ReadPhysicalWord(unsigned long long physicalAddress)
	{
		std::array<unsigned char, sizeof(unsigned long long)> bytes{};
		if (!TryReadPhysicalRange(physicalAddress, std::span<unsigned char>(bytes)))
			return 0;

		unsigned long long value = 0;
		for (std::size_t index = 0; index < bytes.size(); ++index)
			value |= static_cast<unsigned long long>(bytes[index]) << (index * 8);
		return value;
	}

	bool HybridCpuIseSparseMainMemoryAreaV1::InRange(unsigned long long address, std::size_t count) const
	{
		return address <= static_cast<unsigned long long>(_length) &&
			count <= static_cast<unsigned long long>(_length) - address;
	}
}
