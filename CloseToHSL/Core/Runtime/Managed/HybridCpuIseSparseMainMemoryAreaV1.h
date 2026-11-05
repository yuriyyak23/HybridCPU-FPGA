#pragma once

#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <unordered_map>
#include <vector>
#include <span>
#include <memory>

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{

	/// <summary>Bounded page-sparse physical memory for loader-backed guest evidence runs.</summary>
	class HybridCpuIseSparseMainMemoryAreaV1 final : public Processor::MainMemoryArea
	{
	private:
		static constexpr int PageSize = 4096;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, byte[]> _pages = new();
		std::unordered_map<unsigned long long, std::vector<unsigned char>> _pages = std::unordered_map<unsigned long long, std::vector<unsigned char>>();
		const long long _length;

	public:
		HybridCpuIseSparseMainMemoryAreaV1(long long length = 0x3000'0000);

		long long getLength() const override;

		bool TryReadPhysicalRange(unsigned long long physicalAddress, std::span<unsigned char> buffer) override;

		bool TryWritePhysicalRange(unsigned long long physicalAddress, std::span<const unsigned char> buffer) override;

		unsigned long long ReadPhysicalWord(unsigned long long physicalAddress) override;

	private:
		bool InRange(unsigned long long address, std::size_t count) const;

	protected:
		std::shared_ptr<HybridCpuIseSparseMainMemoryAreaV1> shared_from_this()
		{
			return std::static_pointer_cast<HybridCpuIseSparseMainMemoryAreaV1>(Processor::MainMemoryArea::shared_from_this());
		}
	};

}
