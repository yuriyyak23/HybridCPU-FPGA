#include "YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::span<const std::shared_ptr<CompatAliasMapEntry>> CompatAliasMap::getEntries()
	{
		return EntryTable;
	}

	std::shared_ptr<CompatAliasSchemaArtifact> CompatAliasMap::getCanonicalSchema()
	{
		return CanonicalSchema;
	}

	bool CompatAliasMap::MatchesCanonicalSchema(std::shared_ptr<CompatAliasSchemaArtifact> artifact)
	{
		return artifact->IsCanonical && artifact->SchemaVersion == getCanonicalSchema()->SchemaVersion && artifact->SourceHash == getCanonicalSchema()->SourceHash && artifact->ArtifactHash == getCanonicalSchema()->ArtifactHash && artifact->EntryCount == EntryTable.size();
	}

	bool CompatAliasMap::ContainsSource(CompatAliasSourceKind sourceKind, const std::wstring &sourceName)
	{
		std::shared_ptr<CompatAliasMapEntry> ignored;
		return TryGetEntry(sourceKind, sourceName, ignored);
	}

	bool CompatAliasMap::TryGetEntry(CompatAliasSourceKind sourceKind, const std::wstring &sourceName, std::shared_ptr<CompatAliasMapEntry> &entry)
	{
		for (auto candidate : getEntries())
		{
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
			if (candidate->SourceKind == sourceKind && candidate->SourceName == sourceName)
			{
				entry = candidate;
				return true;
			}
		}

		entry = nullptr;
		return false;
	}

	bool CompatAliasMap::IsFrozenAbiSource(CompatAliasSourceKind sourceKind, const std::wstring &sourceName)
	{
		std::shared_ptr<CompatAliasMapEntry> entry;
		return TryGetEntry(sourceKind, sourceName, entry) && entry->IsFrozenAbi;
	}
}
