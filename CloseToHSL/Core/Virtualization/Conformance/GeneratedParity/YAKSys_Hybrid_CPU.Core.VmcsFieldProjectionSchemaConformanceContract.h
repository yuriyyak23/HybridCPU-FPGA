#pragma once

#include "VmcsFieldProjectionSchemaConformanceContract.h"
#include "../../Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.h"
#include <unordered_set>
#include <algorithm>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmcsFieldProjectionSchemaConformanceContract final : public std::enable_shared_from_this<VmcsFieldProjectionSchemaConformanceContract>
	{
	public:
		VmcsFieldProjectionSchemaConformanceViolation Evaluate(std::shared_ptr<VmcsFieldProjectionSchemaArtifact> artifact);

		bool IsSatisfied(std::shared_ptr<VmcsFieldProjectionSchemaArtifact> artifact);

		bool IsCurrentSchemaSatisfied();

	private:
		static bool IsHostCompatibilityAlias(VmcsField field);
	};
}
