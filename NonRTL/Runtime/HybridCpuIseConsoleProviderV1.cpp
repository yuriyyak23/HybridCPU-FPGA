#include "HybridCpuIseConsoleProviderV1.h"

#include <algorithm>
#include <utility>

using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;

namespace HybridCPU_ISE::NonRTL::Runtime
{
	HybridCpuIseConsoleProviderV1::HybridCpuIseConsoleProviderV1(
		std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> read,
		int maximumBytes,
		int maximumEvents)
		: _read(read ? std::move(read) : throw std::invalid_argument("read")),
		  _maximumBytes(maximumBytes),
		  _maximumEvents(maximumEvents)
	{
		if (maximumBytes <= 0 || maximumEvents <= 0)
		{
			throw std::out_of_range("maximumBytes");
		}
	}

	const std::vector<std::shared_ptr<HybridCpuIseConsoleOutputV1>>
	HybridCpuIseConsoleProviderV1::getOutput() const
	{
		return _output;
	}

	std::shared_ptr<HybridCpuHostProviderResultV1>
	HybridCpuIseConsoleProviderV1::Invoke(std::shared_ptr<HybridCpuExternalServiceRequestV1> request)
	{
		if (!request)
		{
			return std::make_shared<HybridCpuHostProviderResultV1>(
				HybridCpuExternalServiceStatusV1::InvalidRequest, 0, 0, L"Console request is missing.");
		}
		if (request->Service != HybridCpuHostServiceV1::Console ||
			(request->Operation != HybridCpuConsoleServiceContractV1::WriteUtf16Operation &&
				request->Operation != HybridCpuConsoleServiceContractV1::SetTitleUtf16Operation))
		{
			return std::make_shared<HybridCpuHostProviderResultV1>(
				HybridCpuExternalServiceStatusV1::MissingSymbol, 0, 2,
				L"Diagnostic console operation is not implemented.");
		}
		if (request->PrivilegeMode != HybridCpuPrivilegeModeV1::User || !request->Arguments ||
			!request->Arguments->empty() ||
			request->BufferLength > static_cast<unsigned long long>(HybridCpuConsoleServiceContractV1::MaximumCodeUnits) * 2 ||
			(request->BufferLength & 1) != 0 ||
			(request->BufferLength == 0
				? request->BufferAddress != 0 || request->BufferAccess != HybridCpuHostBufferAccessV1::None
				: request->BufferAddress == 0 || request->BufferAccess != HybridCpuHostBufferAccessV1::Read) ||
			request->BufferAddress > (std::numeric_limits<unsigned long long>::max)() - request->BufferLength)
		{
			return std::make_shared<HybridCpuHostProviderResultV1>(
				HybridCpuExternalServiceStatusV1::InvalidRequest, 0, 0, L"Invalid console UTF-16 envelope.");
		}

		const int length = static_cast<int>(request->BufferLength);
		if (_output.size() >= static_cast<std::size_t>(_maximumEvents) || length > _maximumBytes - _bytes)
		{
			return std::make_shared<HybridCpuHostProviderResultV1>(
				HybridCpuExternalServiceStatusV1::ProviderFailure, 0, 12,
				L"Diagnostic console capacity exhausted.");
		}

		std::optional<std::vector<unsigned char>> bytes = length == 0
			? std::optional<std::vector<unsigned char>>(std::vector<unsigned char>{})
			: _read(request->BufferAddress, length);
		if (!bytes || bytes->size() != static_cast<std::size_t>(length))
		{
			return std::make_shared<HybridCpuHostProviderResultV1>(
				HybridCpuExternalServiceStatusV1::ProviderFailure, 0, 5,
				L"Console buffer snapshot failed.");
		}

		std::wstring text(static_cast<std::size_t>(length / 2), L'\0');
		for (int index = 0; index < length / 2; ++index)
		{
			const unsigned short codeUnit = static_cast<unsigned short>((*bytes)[static_cast<std::size_t>(index) * 2]) |
				static_cast<unsigned short>(static_cast<unsigned short>((*bytes)[static_cast<std::size_t>(index) * 2 + 1]) << 8);
			text[static_cast<std::size_t>(index)] = static_cast<wchar_t>(codeUnit);
		}

		static constexpr wchar_t HexDigits[] = L"0123456789ABCDEF";
		std::wstring hex;
		hex.reserve(bytes->size() * 2);
		for (const unsigned char byte : *bytes)
		{
			hex.push_back(HexDigits[byte >> 4]);
			hex.push_back(HexDigits[byte & 0x0f]);
		}
		_output.push_back(std::make_shared<HybridCpuIseConsoleOutputV1>(
			request->Operation, std::move(text), std::move(hex)));
		_bytes += length;
		return std::make_shared<HybridCpuHostProviderResultV1>(
			HybridCpuExternalServiceStatusV1::Success, 0, 0, L"");
	}
}
