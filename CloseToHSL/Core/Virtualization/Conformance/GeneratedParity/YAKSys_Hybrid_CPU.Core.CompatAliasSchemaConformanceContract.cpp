#include "YAKSys_Hybrid_CPU.Core.CompatAliasSchemaConformanceContract.h"
#include "../../Compatibility/Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"
#include "../../Compatibility/Generated/SpecArtifacts/YAKSys_Hybrid_CPU.Core.CompatSpecArtifactSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CompatAliasSchemaConformanceViolation CompatAliasSchemaConformanceContract::Evaluate(std::shared_ptr<CompatAliasSchemaConformanceRequest> request)
	{
		if (request->SpecArtifacts == nullptr)
		{
			return CompatAliasSchemaConformanceViolation::MissingSpecArtifactSet;
		}

		if (request->AliasMap == nullptr)
		{
			return CompatAliasSchemaConformanceViolation::MissingAliasMap;
		}

		if (!request->SpecArtifacts->Contains(CompatSpecArtifactKind::CompatAliasMap))
		{
			return CompatAliasSchemaConformanceViolation::MissingCompatAliasArtifact;
		}

		if (!request->SpecArtifacts->RequiresGeneratedParity(CompatSpecArtifactKind::CompatAliasMap))
		{
			return CompatAliasSchemaConformanceViolation::GeneratedParityNotRequired;
		}

		if (!request->SpecArtifacts->RequiresAbiFreeze(CompatSpecArtifactKind::CompatAliasMap))
		{
			return CompatAliasSchemaConformanceViolation::AbiFreezeNotRequired;
		}

		if (!request->SchemaArtifact->IsCanonical || request->SchemaArtifact->SourceHash == 0 || request->SchemaArtifact->ArtifactHash == 0 || request->SchemaArtifact->EntryCount == 0)
		{
			return CompatAliasSchemaConformanceViolation::MissingCanonicalSchema;
		}

		if (!request->AliasMap->MatchesCanonicalSchema(request->SchemaArtifact))
		{
			return CompatAliasSchemaConformanceViolation::SchemaMismatch;
		}

		for (auto entry : CompatAliasMap::getEntries())
		{
			if (StringHelper::isEmptyOrWhiteSpace(entry->SourceName) || StringHelper::isEmptyOrWhiteSpace(entry->TargetName))
			{
				return CompatAliasSchemaConformanceViolation::EmptyAliasName;
			}

			if (!entry->IsFrozenAbi)
			{
				return CompatAliasSchemaConformanceViolation::UnfrozenAliasEntry;
			}
		}

		return CompatAliasSchemaConformanceViolation::None;
	}

	bool CompatAliasSchemaConformanceContract::IsSatisfied(std::shared_ptr<CompatAliasSchemaConformanceRequest> request)
	{
		return Evaluate(request) == CompatAliasSchemaConformanceViolation::None;
	}

	bool CompatAliasSchemaConformanceContract::IsCurrentSchemaSatisfied()
	{
		return IsSatisfied(std::make_shared<CompatAliasSchemaConformanceRequest>(std::make_shared<CompatAliasMap>(), std::make_shared<CompatSpecArtifactSet>(), CompatAliasMap::getCanonicalSchema()));
	}
}
