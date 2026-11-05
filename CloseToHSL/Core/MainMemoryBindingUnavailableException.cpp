#include "MainMemoryBindingUnavailableException.h"

namespace YAKSys_Hybrid_CPU::Core
{

	MainMemoryBindingUnavailableException::MainMemoryBindingUnavailableException(const std::wstring &bindingSurface, const std::wstring &operation)
		: InvalidOperationException(std::format(L"{} requires an explicit MainMemory binding for {}. ", Normalize(bindingSurface), Normalize(operation)) +
			L"Implicit fallback to Processor.MainMemory is disabled; the runtime must fail closed instead of aliasing a mutable global memory surface."),
		  BindingSurface(Normalize(bindingSurface)), Operation(Normalize(operation))
	{
	}

	std::wstring MainMemoryBindingUnavailableException::getBindingSurface() const
	{
		return BindingSurface;
	}

	std::wstring MainMemoryBindingUnavailableException::getOperation() const
	{
		return Operation;
	}

	std::wstring MainMemoryBindingUnavailableException::Normalize(const std::wstring &value)
	{
		return StringHelper::isEmptyOrWhiteSpace(value) ? L"<unknown>" : value;
	}
}
