#pragma once

#include "../../../../../Runtime/Completion/Records/YAKSys_Hybrid_CPU.Core.CompletionRecord.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "CompletionProjectionService.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CompletionProjectionService final : public std::enable_shared_from_this<CompletionProjectionService>
	{
	public:
		std::shared_ptr<VmxCompletionProjection> ProjectToVmx(std::shared_ptr<CompletionRecord> record);

		VmExitReason ProjectReason(std::shared_ptr<CompletionRecord> record);

		bool CanProjectToVmx(std::shared_ptr<CompletionRecord> record);
	};
}
