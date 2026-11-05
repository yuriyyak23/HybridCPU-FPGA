#pragma once

#include "../../Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.h"
#include "../../Compatibility/Generated/CapabilityProjection/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"
#include "ProjectionSchemaPipelineContract.h"
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class ProjectionSchemaPipelineContract final : public std::enable_shared_from_this<ProjectionSchemaPipelineContract>
	{
	private:
		static inline const std::shared_ptr<ProjectionSchemaPipelineArtifact> CurrentVmcsFieldProjection = std::make_shared<ProjectionSchemaPipelineArtifact>(L"docs/VMXRefactoring/schemas/vmcs-field-projection-schema.v1.json", L"VmcsFieldProjectionGenerator", VmcsFieldProjectionSchema::getCanonicalArtifact()->SourceHash, VmcsFieldProjectionSchema::getCanonicalArtifact()->ArtifactHash, VmcsFieldProjectionSchema::getCanonicalArtifact()->EntryCount, true, true, true);
		static inline const std::shared_ptr<ProjectionSchemaPipelineArtifact> CurrentVmxCapsBitProjection = std::make_shared<ProjectionSchemaPipelineArtifact>(L"docs/VMXRefactoring/schemas/vmxcaps-capability-bit-schema.v1.json", L"VmxCapsCapabilityBitGenerator", CapabilityDescriptorSetSchema::getVmxCompatibilityBitSchema()->SourceHash, CapabilityDescriptorSetSchema::getVmxCompatibilityBitSchema()->ArtifactHash, CapabilityDescriptorSetSchema::getVmxCompatibilityBitSchema()->EntryCount, true, true, true);

	public:
		static std::shared_ptr<ProjectionSchemaPipelineArtifact> getCurrentVmcsFieldProjection();

		static std::shared_ptr<ProjectionSchemaPipelineArtifact> getCurrentVmxCapsBitProjection();

		ProjectionSchemaPipelineViolation Evaluate(std::shared_ptr<ProjectionSchemaPipelineArtifact> artifact);

		bool IsSatisfied(std::shared_ptr<ProjectionSchemaPipelineArtifact> artifact);

		bool IsCurrentVmcsFieldProjectionSatisfied();

		bool IsCurrentVmxCapsBitProjectionSatisfied();
	};
}

