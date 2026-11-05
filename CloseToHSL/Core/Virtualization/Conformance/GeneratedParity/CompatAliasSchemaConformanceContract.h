#pragma once

#include "../../Compatibility/Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"
#include "../../Compatibility/Generated/SpecArtifacts/YAKSys_Hybrid_CPU.Core.CompatSpecArtifactSet.h"
#include <memory>
#include <utility>

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
			std::shared_ptr<CompatSpecArtifactSet> specArtifacts, std::shared_ptr<CompatAliasSchemaArtifact> schemaArtifact)
			: AliasMap(std::move(aliasMap)), SpecArtifacts(std::move(specArtifacts)), SchemaArtifact(std::move(schemaArtifact)) {}
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatAliasSchemaConformanceRequest(CompatAliasMap AliasMap, CompatSpecArtifactSet SpecArtifacts, CompatAliasSchemaArtifact SchemaArtifact);


}
