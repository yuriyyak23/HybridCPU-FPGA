#pragma once

#include <string>
#include <memory>
#include "../../../../../tangible_string_helper.h"

#include "NestedProjectionService.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedProjectionService final : public std::enable_shared_from_this<NestedProjectionService>
	{
	public:
		std::shared_ptr<NestedProjectionResult> Validate(std::shared_ptr<NestedProjectionRequest> request);

		bool CanProject(std::shared_ptr<NestedProjectionRequest> request);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool IsExplicitlyAllowed(bool allowed, string? reason)
		static bool IsExplicitlyAllowed(bool allowed, const std::wstring &reason);
	};
}
