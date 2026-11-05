#pragma once

#include "../../Compatibility/Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"
#include "YAKSys_Hybrid_CPU.Core.CompatAliasSchemaConformanceContract.h"
#include "../../Compatibility/Generated/SpecArtifacts/YAKSys_Hybrid_CPU.Core.CompatSpecArtifactSet.h"
#include "CompatAliasGeneratorPipelineContract.h"
#include <string>
#include <utility>
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	struct CompatAliasGeneratorPipelineArtifact final
	{
		const std::wstring SchemaPath;
		const std::wstring GeneratorName;
		const std::shared_ptr<CompatAliasSchemaArtifact> CanonicalSchema;
		const bool GeneratedAtBuild;
		const bool RequiresConformanceParity;
		CompatAliasGeneratorPipelineArtifact(std::wstring schemaPath, std::wstring generatorName,
			std::shared_ptr<CompatAliasSchemaArtifact> canonicalSchema, bool generatedAtBuild, bool requiresConformanceParity)
			: SchemaPath(std::move(schemaPath)), GeneratorName(std::move(generatorName)), CanonicalSchema(std::move(canonicalSchema)),
			  GeneratedAtBuild(generatedAtBuild), RequiresConformanceParity(requiresConformanceParity) {}
	};

	class CompatAliasGeneratorPipelineContract final : public std::enable_shared_from_this<CompatAliasGeneratorPipelineContract>
	{
	private:
		static inline const std::shared_ptr<CompatAliasGeneratorPipelineArtifact> Current = std::make_shared<CompatAliasGeneratorPipelineArtifact>(L"docs/VMXRefactoring/schemas/compat-alias-schema.v1.json", L"CompatAliasMapGenerator", CompatAliasMap::getCanonicalSchema(), true, true);

	public:
		static std::shared_ptr<CompatAliasGeneratorPipelineArtifact> getCurrent();

		CompatAliasGeneratorPipelineViolation Evaluate(std::shared_ptr<CompatAliasGeneratorPipelineArtifact> artifact, std::shared_ptr<CompatSpecArtifactSet> specArtifacts, std::shared_ptr<CompatAliasSchemaConformanceContract> schemaContract);

		bool IsSatisfied(std::shared_ptr<CompatAliasGeneratorPipelineArtifact> artifact);

		bool IsCurrentPipelineSatisfied();
	};
}
