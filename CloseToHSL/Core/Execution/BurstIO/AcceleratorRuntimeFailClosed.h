#pragma once

#include "IBurstBackend.h"
#include <string>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Execution
{
	class AcceleratorRuntimeFailClosed final
	{
	private:
		static inline const std::wstring RegistrationMessage = std::wstring(L"Retained custom-accelerator DMA registration is not implemented in the current runtime. ") + L"The legacy contour must fail closed instead of silently accepting the device as authority.";

		static inline const std::wstring TransferMessage = std::wstring(L"Retained custom-accelerator DMA transfers are not implemented in the current runtime. ") + L"The legacy contour must fail closed instead of returning a completed transfer token.";

	public:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [DoesNotReturn] public static void ThrowRegistrationNotSupported()
		static void ThrowRegistrationNotSupported();

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [DoesNotReturn] public static DMATransferToken ThrowTransferNotSupported()
		static std::shared_ptr<DMATransferToken> ThrowTransferNotSupported();
	};
}
