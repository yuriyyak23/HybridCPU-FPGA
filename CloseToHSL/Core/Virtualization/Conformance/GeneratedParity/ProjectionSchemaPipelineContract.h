#pragma once

#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class ProjectionSchemaPipelineViolation : unsigned char
	{
		None = 0,
		MissingSchemaPath = 1,
		MissingGeneratorName = 2,
		MissingCanonicalSourceHash = 3,
		MissingGeneratedArtifactHash = 4,
		MissingEntryCount = 5,
		MissingBuildGeneration = 6,
		MissingConformanceParity = 7,
		MissingAbiFreeze = 8
	};

	struct ProjectionSchemaPipelineArtifact final
	{
		const std::wstring SchemaPath;
		const std::wstring GeneratorName;
		const unsigned long long SourceHash;
		const unsigned long long ArtifactHash;
		const int EntryCount;
		const bool GeneratedAtBuild;
		const bool RequiresConformanceParity;
		const bool RequiresAbiFreeze;

		ProjectionSchemaPipelineArtifact(std::wstring schemaPath, std::wstring generatorName,
			unsigned long long sourceHash, unsigned long long artifactHash, int entryCount,
			bool generatedAtBuild, bool requiresConformanceParity, bool requiresAbiFreeze)
			: SchemaPath(std::move(schemaPath)), GeneratorName(std::move(generatorName)),
			  SourceHash(sourceHash), ArtifactHash(artifactHash), EntryCount(entryCount),
			  GeneratedAtBuild(generatedAtBuild), RequiresConformanceParity(requiresConformanceParity),
			  RequiresAbiFreeze(requiresAbiFreeze) {}
	};
}
