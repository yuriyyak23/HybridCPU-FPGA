#pragma once

#include "../../Compatibility/Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"
#include "../../Compatibility/Generated/SpecArtifacts/YAKSys_Hybrid_CPU.Core.CompatSpecArtifactSet.h"
#include <memory>
#include <utility>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	enum class CompatAliasSchemaConformanceViolation : unsigned char
	{
		None = 0,
		MissingSpecArtifactSet = 1,
		MissingAliasMap = 2,
		MissingCompatAliasArtifact = 3,
		GeneratedParityNotRequired = 4,
		AbiFreezeNotRequired = 5,
		MissingCanonicalSchema = 6,
		SchemaMismatch = 7,
		EmptyAliasName = 8,
		UnfrozenAliasEntry = 9
	};

	struct CompatAliasSchemaConformanceRequest final
	{
		const std::shared_ptr<CompatAliasMap> AliasMap;
		const std::shared_ptr<CompatSpecArtifactSet> SpecArtifacts;
		const std::shared_ptr<CompatAliasSchemaArtifact> SchemaArtifact;

		CompatAliasSchemaConformanceRequest(std::shared_ptr<CompatAliasMap> aliasMap,
			std::shared_ptr<CompatSpecArtifactSet> specArtifacts,
			std::shared_ptr<CompatAliasSchemaArtifact> schemaArtifact)
			: AliasMap(std::move(aliasMap)), SpecArtifacts(std::move(specArtifacts)),
			  SchemaArtifact(std::move(schemaArtifact)) {}
	};

	class CompatAliasSchemaConformanceContract final : public std::enable_shared_from_this<CompatAliasSchemaConformanceContract>
	{
	public:
		CompatAliasSchemaConformanceViolation Evaluate(std::shared_ptr<CompatAliasSchemaConformanceRequest> request);

		bool IsSatisfied(std::shared_ptr<CompatAliasSchemaConformanceRequest> request);

		bool IsCurrentSchemaSatisfied();
	};
}
