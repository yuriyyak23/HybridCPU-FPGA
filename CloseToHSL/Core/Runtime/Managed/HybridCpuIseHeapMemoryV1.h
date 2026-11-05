#pragma once

#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../NonRTL/Runtime/HybridCpuIseBootBlobContractsV1.h"
#include <vector>
#include <span>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{

	/// <summary>
	/// Explicit production binding between ManagedRuntime heap storage and the exact ISE
	/// MainMemory instance used by a CPU execution context. It never falls back to the
	/// mutable Processor.MainMemory global.
	/// </summary>
	class HybridCpuIseHeapMemoryV1 final : public HybridCPU::ManagedRuntime::IHybridCpuManagedHeapMemoryV1
	{
	private:
		const unsigned long long _baseAddress;
		const unsigned long long _sizeBytes;

		const std::shared_ptr<Processor::MainMemoryArea> _memory;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public HybridCpuIseHeapMemoryV1(Processor.MainMemoryArea memory, ulong baseAddress, ulong sizeBytes)
		HybridCpuIseHeapMemoryV1(std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long baseAddress, unsigned long long sizeBytes);

		unsigned long long BaseAddress() const override;
		unsigned long long SizeBytes() const override;

		bool Clear(unsigned long long address, int byteCount) override;

		bool Read(unsigned long long address, std::span<unsigned char> destination) const override;

		bool Write(unsigned long long address, std::span<const unsigned char> source) override;

	private:
		bool Range(unsigned long long address, int byteCount) const;
	};

}
