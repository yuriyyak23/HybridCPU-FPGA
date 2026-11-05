#include "UnsupportedExecutionSurfaceException.h"

namespace YAKSys_Hybrid_CPU::Core
{

    UnsupportedExecutionSurfaceException::UnsupportedExecutionSurfaceException(const std::wstring& surfaceName, int slotIndex, unsigned int opCode, unsigned long long bundlePc)
        : UnsupportedExecutionSurfaceException(surfaceName, slotIndex, opCode, bundlePc, nullptr) {}

    UnsupportedExecutionSurfaceException::UnsupportedExecutionSurfaceException(const std::wstring& surfaceName, int slotIndex, unsigned int opCode, unsigned long long bundlePc, std::exception_ptr innerException)
        : std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(
            ExecutionFaultContract::FormatMessage(ExecutionFaultCategory::UnsupportedExecutionSurface,
                std::format(L"{} slot {} (opcode 0x{:X}) at PC 0x{:X} is legal decode but has no supported mainline runtime execution surface. Reject it before execute until an explicit retire/apply contour lands.", surfaceName, slotIndex, opCode, bundlePc)))),
          SurfaceName(surfaceName), SlotIndex(slotIndex), OpCode(opCode), BundlePc(bundlePc)
    {
        this->innerException = std::move(innerException);
        ExecutionFaultContract::Stamp(*this, getCategory());
    }
	std::wstring UnsupportedExecutionSurfaceException::getSurfaceName() const
	{
		return SurfaceName;
	}

	int UnsupportedExecutionSurfaceException::getSlotIndex() const
	{
		return SlotIndex;
	}

	unsigned int UnsupportedExecutionSurfaceException::getOpCode() const
	{
		return OpCode;
	}

	unsigned long long UnsupportedExecutionSurfaceException::getBundlePc() const
	{
		return BundlePc;
	}

	ExecutionFaultCategory UnsupportedExecutionSurfaceException::getCategory() const
	{
		return ExecutionFaultCategory::UnsupportedExecutionSurface;
	}

	std::shared_ptr<UnsupportedExecutionSurfaceException> UnsupportedExecutionSurfaceException::CreateForAtomic(int slotIndex, unsigned int opCode, unsigned long long bundlePc)
	{
		return std::make_shared<UnsupportedExecutionSurfaceException>(L"Atomic", slotIndex, opCode, bundlePc);
	}

	std::shared_ptr<UnsupportedExecutionSurfaceException> UnsupportedExecutionSurfaceException::CreateForMemory(int slotIndex, unsigned int opCode, unsigned long long bundlePc)
	{
		return std::make_shared<UnsupportedExecutionSurfaceException>(L"Memory", slotIndex, opCode, bundlePc);
	}

	std::shared_ptr<UnsupportedExecutionSurfaceException> UnsupportedExecutionSurfaceException::CreateForControlFlow(int slotIndex, unsigned int opCode, unsigned long long bundlePc)
	{
		return std::make_shared<UnsupportedExecutionSurfaceException>(L"ControlFlow", slotIndex, opCode, bundlePc);
	}

	std::shared_ptr<UnsupportedExecutionSurfaceException> UnsupportedExecutionSurfaceException::CreateForStreamControl(int slotIndex, unsigned int opCode, unsigned long long bundlePc)
	{
		return std::make_shared<UnsupportedExecutionSurfaceException>(L"StreamControl", slotIndex, opCode, bundlePc);
	}
}
