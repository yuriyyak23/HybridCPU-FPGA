#pragma once

#include "../Execution/ExecutionFaultContract.h"
#include <string>
#include <stdexcept>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Thrown when a decoded instruction is legal but cannot be routed to a supported
	/// runtime execution surface in the current mainline pipeline configuration.
	/// </summary>
	class UnsupportedExecutionSurfaceException final : public std::runtime_error, public ExecutionFaultMetadata
	{
	private:
		const std::wstring SurfaceName;
		const int SlotIndex;
		const unsigned int OpCode;
		const unsigned long long BundlePc;

	public:
		UnsupportedExecutionSurfaceException(const std::wstring &surfaceName, int slotIndex, unsigned int opCode, unsigned long long bundlePc);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public UnsupportedExecutionSurfaceException(string surfaceName, int slotIndex, uint opCode, ulong bundlePc, Exception? innerException) : base(ExecutionFaultContract.FormatMessage(ExecutionFaultCategory.UnsupportedExecutionSurface, string.Format("{0} slot {1} (opcode 0x{2:X}) at PC 0x{3:X} is legal decode but has no supported mainline runtime execution surface. ", surfaceName, slotIndex, opCode, bundlePc) + "Reject it before execute until an explicit retire/apply contour lands."), innerException)
		UnsupportedExecutionSurfaceException(const std::wstring &surfaceName, int slotIndex, unsigned int opCode, unsigned long long bundlePc, std::exception_ptr innerException);

		std::wstring getSurfaceName() const;
		int getSlotIndex() const;
		unsigned int getOpCode() const;
		unsigned long long getBundlePc() const;
		ExecutionFaultCategory getCategory() const;

		static std::shared_ptr<UnsupportedExecutionSurfaceException> CreateForAtomic(int slotIndex, unsigned int opCode, unsigned long long bundlePc);

		static std::shared_ptr<UnsupportedExecutionSurfaceException> CreateForMemory(int slotIndex, unsigned int opCode, unsigned long long bundlePc);

		static std::shared_ptr<UnsupportedExecutionSurfaceException> CreateForControlFlow(int slotIndex, unsigned int opCode, unsigned long long bundlePc);

		static std::shared_ptr<UnsupportedExecutionSurfaceException> CreateForStreamControl(int slotIndex, unsigned int opCode, unsigned long long bundlePc);
	};
}
