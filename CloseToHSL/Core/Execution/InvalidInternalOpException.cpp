#include "InvalidInternalOpException.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Execution
{

	InternalOpKind InvalidInternalOpException::getKind() const
	{
		return Kind;
	}

	YAKSys_Hybrid_CPU::Core::ExecutionFaultCategory InvalidInternalOpException::getCategory() const
	{
		return ExecutionFaultCategory::InvalidInternalOp;
	}

    InvalidInternalOpException::InvalidInternalOpException(InternalOpKind kind) : InvalidInternalOpException(kind, nullptr) {}

    InvalidInternalOpException::InvalidInternalOpException(InternalOpKind kind, std::exception_ptr innerException)
        : std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(
            ExecutionFaultContract::FormatMessage(ExecutionFaultCategory::InvalidInternalOp,
                std::format(L"No execution unit registered for InternalOpKind.{}. Ensure every InternalOpKind value has a corresponding IExecutionUnit in the ExecutionEngine registry.", static_cast<int>(kind))))), Kind(kind)
    {
        this->innerException = std::move(innerException);
        ExecutionFaultContract::Stamp(*this, getCategory());
    }
}