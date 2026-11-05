#pragma once
#include "VmcsV2DescriptorProjection.h"

#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{
	class VmcsV2DescriptorProjection final : public std::enable_shared_from_this<VmcsV2DescriptorProjection>
	{
	public:
		std::shared_ptr<VmcsV2DescriptorProjectionResult> ValidateProjection(std::shared_ptr<VmcsV2DescriptorProjectionRequest> request);
	};
}
