#include "DecodeProjectionFaultException.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DecodeProjectionFaultException::DecodeProjectionFaultException(const std::wstring &message) : InvalidOperationException(message), InnerException(nullptr)
	{
	}

	DecodeProjectionFaultException::DecodeProjectionFaultException(const std::wstring &message, std::runtime_error innerException)
		: InvalidOperationException(message), InnerException(std::make_exception_ptr(std::move(innerException)))
	{
	}
}
