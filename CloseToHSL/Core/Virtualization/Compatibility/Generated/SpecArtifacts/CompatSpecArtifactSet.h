#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompatSpecArtifactKind : unsigned char
	{
		FrozenOpcodeAliases = 0,
		FrozenCsrAliases = 1,
		VmcsFieldAliases = 2,
		CapabilityProjection = 3,
		CompletionProjection = 4,
		CompatAliasMap = 5
	};

	enum class CompatSpecArtifactLineageKind : unsigned char
	{
		FrozenAbiVocabulary = 0,
		ExecutableGeneratedSource = 1,
		ProjectionContractOnly = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatSpecArtifact(CompatSpecArtifactKind Kind, string Name, string LineageSource, bool IsGenerated, bool IsFrozenAbi, CompatSpecArtifactLineageKind LineageKind);


}
