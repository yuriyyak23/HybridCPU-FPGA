#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompatAliasGeneratorPipelineViolation : unsigned char
	{
		None = 0,
		MissingSchemaPath = 1,
		MissingGeneratorName = 2,
		MissingCanonicalSchema = 3,
		MissingGeneratedArtifact = 4,
		MissingGeneratedParityRequirement = 5,
		MissingAbiFreezeRequirement = 6,
		SchemaConformanceFailed = 7
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatAliasGeneratorPipelineArtifact(string SchemaPath, string GeneratorName, CompatAliasSchemaArtifact CanonicalSchema, bool GeneratedAtBuild, bool RequiresConformanceParity);


}
