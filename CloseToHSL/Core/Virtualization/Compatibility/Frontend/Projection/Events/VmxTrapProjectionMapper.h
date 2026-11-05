#pragma once

#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../../../Runtime/Events/Traps/NeutralTrapResult.h"
#include "TrapDecision.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class VmxTrapProjectionMapper final : public std::enable_shared_from_this<VmxTrapProjectionMapper>
	{
	private:
		static inline const std::shared_ptr<VmxTrapProjectionMapper> Default = std::make_shared<VmxTrapProjectionMapper>();

	public:
		static std::shared_ptr<VmxTrapProjectionMapper> getDefault();

		std::shared_ptr<TrapDecision> Project(std::shared_ptr<NeutralTrapResult> result);

		VmExitReason ProjectReason(std::shared_ptr<NeutralTrapResult> result);

		VmExitReason ProjectReason(NeutralTrapResultKind kind);

	private:
		static VmExitReason ProjectCompatibilityOperationReason(std::shared_ptr<TrapRequest> request);
	};



}
