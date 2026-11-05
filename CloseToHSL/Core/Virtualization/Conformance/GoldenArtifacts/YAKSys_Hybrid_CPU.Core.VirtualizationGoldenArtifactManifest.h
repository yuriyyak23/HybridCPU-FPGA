#pragma once

#include "VirtualizationGoldenArtifactManifest.h"
#include <vector>
#include <memory>
#include <span>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationGoldenArtifactManifest final : public std::enable_shared_from_this<VirtualizationGoldenArtifactManifest>
	{
	private:
		static inline const std::vector<std::shared_ptr<VirtualizationGoldenArtifact>> ArtifactTable = {std::make_shared<VirtualizationGoldenArtifact>(VirtualizationGoldenArtifactKind::CompatAbiSnapshot, L"CompatAbiFreezeContract", true, false), std::make_shared<VirtualizationGoldenArtifact>(VirtualizationGoldenArtifactKind::GeneratedProjectionParity, L"GeneratedProjectionParityContract", true, true), std::make_shared<VirtualizationGoldenArtifact>(VirtualizationGoldenArtifactKind::AliasMapCompleteness, L"CompatAliasMap", true, true), std::make_shared<VirtualizationGoldenArtifact>(VirtualizationGoldenArtifactKind::HostEvidenceNonLeak, L"HostEvidenceNonLeakContract", true, false), std::make_shared<VirtualizationGoldenArtifact>(VirtualizationGoldenArtifactKind::MigrationReplay, L"MigrationReplayContract", true, false), std::make_shared<VirtualizationGoldenArtifact>(VirtualizationGoldenArtifactKind::NestedComposition, L"NestedCompositionContract", true, false)};

	public:
		static std::span<const std::shared_ptr<VirtualizationGoldenArtifact>> getArtifacts();

		bool Contains(VirtualizationGoldenArtifactKind kind);

		bool TryGetArtifact(VirtualizationGoldenArtifactKind kind, std::shared_ptr<VirtualizationGoldenArtifact> &artifact);

		bool IsHardArchitecturalGate(VirtualizationGoldenArtifactKind kind);

		bool RequiresGeneratedSource(VirtualizationGoldenArtifactKind kind);
	};
}
