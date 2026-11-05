#include "YAKSys_Hybrid_CPU.Core.CompatAliasGeneratorPipelineContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<CompatAliasGeneratorPipelineArtifact> CompatAliasGeneratorPipelineContract::getCurrent()
	{
		return Current;
	}

	CompatAliasGeneratorPipelineViolation CompatAliasGeneratorPipelineContract::Evaluate(std::shared_ptr<CompatAliasGeneratorPipelineArtifact> artifact, std::shared_ptr<CompatSpecArtifactSet> specArtifacts, std::shared_ptr<CompatAliasSchemaConformanceContract> schemaContract)
	{
		if (StringHelper::isEmptyOrWhiteSpace(artifact->SchemaPath))
		{
			return CompatAliasGeneratorPipelineViolation::MissingSchemaPath;
		}

		if (StringHelper::isEmptyOrWhiteSpace(artifact->GeneratorName))
		{
			return CompatAliasGeneratorPipelineViolation::MissingGeneratorName;
		}

		if (!artifact->CanonicalSchema->IsCanonical || artifact->CanonicalSchema->SourceHash == 0)
		{
			return CompatAliasGeneratorPipelineViolation::MissingCanonicalSchema;
		}

		if (!artifact->GeneratedAtBuild || artifact->CanonicalSchema->ArtifactHash == 0)
		{
			return CompatAliasGeneratorPipelineViolation::MissingGeneratedArtifact;
		}

		if (specArtifacts == nullptr || !specArtifacts->RequiresGeneratedParity(CompatSpecArtifactKind::CompatAliasMap) || !artifact->RequiresConformanceParity)
		{
			return CompatAliasGeneratorPipelineViolation::MissingGeneratedParityRequirement;
		}

		if (!specArtifacts->RequiresAbiFreeze(CompatSpecArtifactKind::CompatAliasMap))
		{
			return CompatAliasGeneratorPipelineViolation::MissingAbiFreezeRequirement;
		}

		schemaContract = (schemaContract != nullptr) ? schemaContract : std::make_shared<CompatAliasSchemaConformanceContract>();
		return schemaContract->IsSatisfied(std::make_shared<CompatAliasSchemaConformanceRequest>(std::make_shared<CompatAliasMap>(), specArtifacts, artifact->CanonicalSchema)) ? CompatAliasGeneratorPipelineViolation::None : CompatAliasGeneratorPipelineViolation::SchemaConformanceFailed;
	}

	bool CompatAliasGeneratorPipelineContract::IsSatisfied(std::shared_ptr<CompatAliasGeneratorPipelineArtifact> artifact)
	{
		return Evaluate(artifact, std::make_shared<CompatSpecArtifactSet>(), std::make_shared<CompatAliasSchemaConformanceContract>()) == CompatAliasGeneratorPipelineViolation::None;
	}

	bool CompatAliasGeneratorPipelineContract::IsCurrentPipelineSatisfied()
	{
		return IsSatisfied(getCurrent());
	}
}
