#pragma once

#include "HybridCpuIseBootBlobContractsV1.h"
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;

namespace HybridCPU_ISE::NonRTL::Runtime
{
	struct HybridCpuIseConsoleOutputV1 final
	{
		unsigned long long Operation;
		std::wstring Text;
		std::wstring Utf16LittleEndianHex;

		HybridCpuIseConsoleOutputV1(unsigned long long operation, std::wstring text, std::wstring utf16LittleEndianHex)
			: Operation(operation), Text(std::move(text)), Utf16LittleEndianHex(std::move(utf16LittleEndianHex))
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuIseConsoleOutputV1(ulong Operation, string Text, string Utf16LittleEndianHex);

	/// <summary>Synchronous diagnostic console sink; snapshots guest bytes and owns no guest roots or GUI state.</summary>
	class HybridCpuIseConsoleProviderV1 final : public IHybridCpuHostServiceProviderV1
	{
	private:
		const std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> _read;
		const int _maximumBytes;
		const int _maximumEvents;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<HybridCpuIseConsoleOutputV1> _output = [];
		std::vector<std::shared_ptr<HybridCpuIseConsoleOutputV1>> _output = {};
		int _bytes = 0;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public HybridCpuIseConsoleProviderV1(System.Func<ulong, int, byte[]?> read, int maximumBytes = 8 * 1024 * 1024, int maximumEvents = 4096)
		HybridCpuIseConsoleProviderV1(std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> read, int maximumBytes = 8 * 1024 * 1024, int maximumEvents = 4096);

		const std::vector<std::shared_ptr<HybridCpuIseConsoleOutputV1>> getOutput() const;

		std::shared_ptr<HybridCpuHostProviderResultV1> Invoke(std::shared_ptr<HybridCpuExternalServiceRequestV1> request);
	};

}
