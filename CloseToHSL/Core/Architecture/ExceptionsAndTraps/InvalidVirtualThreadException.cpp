#include "InvalidVirtualThreadException.h"
#include <codecvt>
#include <locale>

using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core
{

	std::wstring InvalidVirtualThreadException::getOperation() const
	{
		return Operation;
	}

	int InvalidVirtualThreadException::getVtId() const
	{
		return VtId;
	}

	int InvalidVirtualThreadException::getVtCount() const
	{
		return VtCount;
	}

	ExecutionFaultCategory InvalidVirtualThreadException::getCategory() const
	{
		return ExecutionFaultCategory::InvalidVirtualThread;
	}

    InvalidVirtualThreadException::InvalidVirtualThreadException(const std::wstring &operation, int vtId, int vtCount)
        : std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(
            ExecutionFaultContract::FormatMessage(ExecutionFaultCategory::InvalidVirtualThread, FormatDetail(operation, vtId, vtCount)))),
          Operation(NormalizeOperation(operation)), VtId(vtId), VtCount(vtCount)
    {
        // Category is exposed by getCategory and the message prefix; Exception.Data needs a native equivalent.
    }

    std::wstring InvalidVirtualThreadException::FormatDetail(const std::wstring &operation, int vtId, int vtCount)
    {
        const std::wstring validRange = vtCount > 0 ? std::format(L"0..{}", vtCount - 1) : L"<none>";
        return std::format(L"{} attempted to access invalid virtual thread {}. Valid VT range is {}.",
            NormalizeOperation(operation), vtId, validRange);
    }
	std::wstring InvalidVirtualThreadException::NormalizeOperation(const std::wstring &operation)
	{
		return StringHelper::isEmptyOrWhiteSpace(operation) ? L"UnknownStateOperation" : operation;
	}

	int VirtualThreadIdResolver::ResolveOrThrow(int vtId, int vtCount, const std::wstring &operation)
	{
		VtId resolvedVtId;
		if (vtCount > 0 && static_cast<unsigned int>(vtId) < static_cast<unsigned int>(vtCount) && VtId::TryCreate(vtId, resolvedVtId))
		{
			return resolvedVtId.getValue();
		}

		throw InvalidVirtualThreadException(operation, vtId, vtCount);
	}
}
