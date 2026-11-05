#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompatibilityWriteNoEmissionDecision : unsigned char
	{
		Allowed = 0,
		UnsupportedWriteResult = 1,
		MissingCompatibilityFence = 2,
		MissingNoEffectPolicy = 3,
		DescriptorMutated = 4,
		GrantsMutated = 5,
		EvidenceMutated = 6,
		CompletionMutated = 7,
		MemoryGenerationMutated = 8,
		LaneStateMutated = 9,
		MigrationStateMutated = 10
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatibilityWriteNoEmissionSnapshot(ulong DescriptorFingerprint, ulong GrantFingerprint, ulong EvidenceFingerprint, ulong CompletionFingerprint, ulong MemoryGenerationFingerprint, ulong LaneStateFingerprint, ulong MigrationStateFingerprint);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatibilityWriteNoEmissionResult(CompatibilityWriteNoEmissionDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == CompatibilityWriteNoEmissionDecision.Allowed;
//			}
//		}
//
//		public static CompatibilityWriteNoEmissionResult Allowed
//		{
//			get;
//		} = new(CompatibilityWriteNoEmissionDecision.Allowed, "Compatibility write has no emission path.");
//
//		public static CompatibilityWriteNoEmissionResult Denied(CompatibilityWriteNoEmissionDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


}
