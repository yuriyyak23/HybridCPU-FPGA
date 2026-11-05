#include "YAKSys_Hybrid_CPU.Core.ProjectionSchemaPipelineContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<ProjectionSchemaPipelineArtifact> ProjectionSchemaPipelineContract::getCurrentVmcsFieldProjection()
	{
		return CurrentVmcsFieldProjection;
	}

	std::shared_ptr<ProjectionSchemaPipelineArtifact> ProjectionSchemaPipelineContract::getCurrentVmxCapsBitProjection()
	{
		return CurrentVmxCapsBitProjection;
	}

	ProjectionSchemaPipelineViolation ProjectionSchemaPipelineContract::Evaluate(std::shared_ptr<ProjectionSchemaPipelineArtifact> artifact)
	{
		if (!artifact)
		{
			return ProjectionSchemaPipelineViolation::MissingSchemaPath;
		}

		if (StringHelper::isEmptyOrWhiteSpace(artifact->SchemaPath))
		{
			return ProjectionSchemaPipelineViolation::MissingSchemaPath;
		}

		if (StringHelper::isEmptyOrWhiteSpace(artifact->GeneratorName))
		{
			return ProjectionSchemaPipelineViolation::MissingGeneratorName;
		}

		if (artifact->SourceHash == 0)
		{
			return ProjectionSchemaPipelineViolation::MissingCanonicalSourceHash;
		}

		if (artifact->ArtifactHash == 0)
		{
			return ProjectionSchemaPipelineViolation::MissingGeneratedArtifactHash;
		}

		if (artifact->EntryCount <= 0)
		{
			return ProjectionSchemaPipelineViolation::MissingEntryCount;
		}

		if (!artifact->GeneratedAtBuild)
		{
			return ProjectionSchemaPipelineViolation::MissingBuildGeneration;
		}

		if (!artifact->RequiresConformanceParity)
		{
			return ProjectionSchemaPipelineViolation::MissingConformanceParity;
		}

		return artifact->RequiresAbiFreeze ? ProjectionSchemaPipelineViolation::None : ProjectionSchemaPipelineViolation::MissingAbiFreeze;
	}

	bool ProjectionSchemaPipelineContract::IsSatisfied(std::shared_ptr<ProjectionSchemaPipelineArtifact> artifact)
	{
		return Evaluate(artifact) == ProjectionSchemaPipelineViolation::None;
	}

	bool ProjectionSchemaPipelineContract::IsCurrentVmcsFieldProjectionSatisfied()
	{
		return IsSatisfied(getCurrentVmcsFieldProjection());
	}

	bool ProjectionSchemaPipelineContract::IsCurrentVmxCapsBitProjectionSatisfied()
	{
		return IsSatisfied(getCurrentVmxCapsBitProjection());
	}
}

