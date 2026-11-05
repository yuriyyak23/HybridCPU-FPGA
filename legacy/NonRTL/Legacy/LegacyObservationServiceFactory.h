#pragma once

#include "../../../Machine/HybridCPU_ISE.IseObservationService.h"
#include <memory>
#include <mutex>

namespace HybridCPU_ISE::Legacy
{
	class LegacyObservationServiceFactory final
	{
	public:
		static std::shared_ptr<IseObservationService> CreateLegacyGlobalCompat(std::shared_ptr<std::recursive_mutex> syncLock);

		static std::shared_ptr<IseObservationService> CreateLegacyGlobalStrict(std::shared_ptr<std::recursive_mutex> syncLock);

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use CreateLegacyGlobalCompat(...) or CreateLegacyGlobalStrict(...) to make legacy-global observation selection explicit.")] public static IseObservationService CreateProcessorBacked(Object syncLock)
		static std::shared_ptr<IseObservationService> CreateProcessorBacked(std::shared_ptr<std::recursive_mutex> syncLock);
	};
}
