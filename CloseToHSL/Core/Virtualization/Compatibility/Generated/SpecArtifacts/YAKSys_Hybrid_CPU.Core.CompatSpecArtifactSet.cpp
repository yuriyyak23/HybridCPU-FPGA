#include "YAKSys_Hybrid_CPU.Core.CompatSpecArtifactSet.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<std::shared_ptr<CompatSpecArtifact>>& CompatSpecArtifactSet::getArtifacts()
	{
		return ArtifactTable;
	}

	bool CompatSpecArtifactSet::Contains(CompatSpecArtifactKind kind)
	{
		for (const auto& artifact : getArtifacts())
		{
			if (artifact->Kind == kind)
			{
				return true;
			}
		}
		return false;
	}

	bool CompatSpecArtifactSet::TryGetArtifact(CompatSpecArtifactKind kind, std::shared_ptr<CompatSpecArtifact>& artifact)
	{
		for (const auto& candidate : getArtifacts())
		{
			if (candidate->Kind == kind)
			{
				artifact = candidate;
				return true;
			}
		}
		artifact = nullptr;
		return false;
	}

	bool CompatSpecArtifactSet::HasExecutableLineage(CompatSpecArtifactKind kind)
	{
		std::shared_ptr<CompatSpecArtifact> artifact;
		return TryGetArtifact(kind, artifact) && artifact->LineageKind == CompatSpecArtifactLineageKind::ExecutableGeneratedSource;
	}

	bool CompatSpecArtifactSet::RequiresGeneratedParity(CompatSpecArtifactKind kind)
	{
		std::shared_ptr<CompatSpecArtifact> artifact;
		return TryGetArtifact(kind, artifact) && artifact->IsGenerated && artifact->LineageKind == CompatSpecArtifactLineageKind::ExecutableGeneratedSource;
	}

	bool CompatSpecArtifactSet::RequiresAbiFreeze(CompatSpecArtifactKind kind)
	{
		std::shared_ptr<CompatSpecArtifact> artifact;
		return TryGetArtifact(kind, artifact) && artifact->IsFrozenAbi;
	}
}