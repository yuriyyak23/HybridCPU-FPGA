#pragma once
#include <string>
#include <memory>
#include <utility>
#include <span>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationGoldenArtifactKind : unsigned char
	{
		CompatAbiSnapshot = 0,
		GeneratedProjectionParity = 1,
		AliasMapCompleteness = 2,
		HostEvidenceNonLeak = 3,
		MigrationReplay = 4,
		NestedComposition = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationGoldenArtifact(VirtualizationGoldenArtifactKind Kind, string Name, bool IsHardArchitecturalGate, bool RequiresGeneratedSource);


struct VirtualizationGoldenArtifact final
{
    VirtualizationGoldenArtifactKind Kind{};
    std::wstring Name;
    bool IsHardArchitecturalGate = false;
    bool RequiresGeneratedSource = false;
    VirtualizationGoldenArtifact(VirtualizationGoldenArtifactKind kind, std::wstring name, bool hardGate, bool generatedSource)
        : Kind(kind), Name(std::move(name)), IsHardArchitecturalGate(hardGate), RequiresGeneratedSource(generatedSource) {}
};
}
