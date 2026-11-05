#pragma once

#include "../../Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"
#include "VmxCompatProjectionService.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCompatProjectionService final : public std::enable_shared_from_this<VmxCompatProjectionService>
	{
	private:
		const std::shared_ptr<CompatAliasMap> _aliasMap;

	public:
		VmxCompatProjectionService();

		VmxCompatProjectionService(std::shared_ptr<CompatAliasMap> aliasMap);

		std::shared_ptr<VmxCompatProjectionResult> ValidateProjection(std::shared_ptr<VmxCompatProjectionRequest> request);
	};
}
