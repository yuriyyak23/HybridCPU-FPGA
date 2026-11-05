#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VectorStreamProjectionDecision : unsigned char
	{
		Allowed = 0,
		MissingExecutionExtension = 1,
		MissingVectorStreamDescriptor = 2,
		RuntimeAuthorityMissing = 3,
		CompatibilityProjectionDenied = 4,
		SaveMaskDenied = 5,
		SnapshotStale = 6,
		HostEvidenceDenied = 7
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VectorStreamProjectionResult(VectorStreamProjectionDecision Decision, VectorStreamSaveMask EffectiveMask, string Message)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VectorStreamProjectionDecision.Allowed;
//			}
//		}
//
//		public static VectorStreamProjectionResult Allowed(VectorStreamSaveMask effectiveMask)
//		{
//			return new(VectorStreamProjectionDecision.Allowed, effectiveMask, string.Empty);
//		}
//
//		public static VectorStreamProjectionResult Denied(VectorStreamProjectionDecision decision, string message)
//		{
//			return new(decision, VectorStreamSaveMask.None, message);
//		}
//	}


}
