#pragma once
#include "VmxCompatRetireBoundary.h"

#include "VmxRetireModel.h"
#include "../Projection/VmxCompletion.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCompatRetireBoundary final : public std::enable_shared_from_this<VmxCompatRetireBoundary>
	{
	public:
		std::shared_ptr<VmxCompatRetireResult> ValidatePublication(std::shared_ptr<VmxCompatRetireRequest> request);

	private:
		static bool RequiresCompletionRoute(VmxRetireEffect effect);

		static bool IsTypedFault(VmxCompletionKind completionKind);
	};
}
