#include "YAKSys_Hybrid_CPU.Core.VirtualizationGoldenArtifactManifest.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::span<const std::shared_ptr<VirtualizationGoldenArtifact>> VirtualizationGoldenArtifactManifest::getArtifacts()
	{
		return ArtifactTable;
	}

	bool VirtualizationGoldenArtifactManifest::Contains(VirtualizationGoldenArtifactKind kind)
	{
		for (auto artifact : getArtifacts())
		{
			if (artifact->Kind == kind)
			{
				return true;
			}
		}

		return false;
	}

	bool VirtualizationGoldenArtifactManifest::TryGetArtifact(VirtualizationGoldenArtifactKind kind, std::shared_ptr<VirtualizationGoldenArtifact> &artifact)
	{
		for (auto candidate : getArtifacts())
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

	bool VirtualizationGoldenArtifactManifest::IsHardArchitecturalGate(VirtualizationGoldenArtifactKind kind)
	{
		std::shared_ptr<VirtualizationGoldenArtifact> artifact;
		return TryGetArtifact(kind, artifact) && artifact->IsHardArchitecturalGate;
	}

	bool VirtualizationGoldenArtifactManifest::RequiresGeneratedSource(VirtualizationGoldenArtifactKind kind)
	{
		std::shared_ptr<VirtualizationGoldenArtifact> artifact;
		return TryGetArtifact(kind, artifact) && artifact->RequiresGeneratedSource;
	}
}
