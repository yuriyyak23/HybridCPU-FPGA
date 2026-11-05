#pragma once

#include <memory>
#include <utility>
#include "SecureComputeVmReadVisibilityPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureComputeCompatibilityProjectionRequest(ulong FieldId, ulong AliasBit, bool HasNeutralOwner, bool HasReadOnlySource, bool SecureVisibilityAllowed, bool MigrationClassified, bool ConformanceProven);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureComputeCompatibilityProjectionResult(SecureComputeVmReadVisibilityDecision Decision, bool ValueAvailable, ulong Value)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == SecureComputeVmReadVisibilityDecision.AllowedReadOnlyProjection && ValueAvailable;
//			}
//		}
//	}


	struct SecureComputeCompatibilityProjectionRequest final
	{
		const unsigned long long FieldId;
		const unsigned long long AliasBit;
		const bool HasNeutralOwner;
		const bool HasReadOnlySource;
		const bool SecureVisibilityAllowed;
		const bool MigrationClassified;
		const bool ConformanceProven;

		SecureComputeCompatibilityProjectionRequest(unsigned long long fieldId, unsigned long long aliasBit,
			bool hasNeutralOwner, bool hasReadOnlySource, bool secureVisibilityAllowed,
			bool migrationClassified, bool conformanceProven)
			: FieldId(fieldId), AliasBit(aliasBit), HasNeutralOwner(hasNeutralOwner),
			  HasReadOnlySource(hasReadOnlySource), SecureVisibilityAllowed(secureVisibilityAllowed),
			  MigrationClassified(migrationClassified), ConformanceProven(conformanceProven)
		{
		}
	};

	struct SecureComputeCompatibilityProjectionResult final
	{
		const SecureComputeVmReadVisibilityDecision Decision;
		const bool ValueAvailable;
		const unsigned long long Value;
		const bool IsAllowed;

		SecureComputeCompatibilityProjectionResult(SecureComputeVmReadVisibilityDecision decision,
			bool valueAvailable, unsigned long long value)
			: Decision(decision), ValueAvailable(valueAvailable), Value(value),
			  IsAllowed(decision == SecureComputeVmReadVisibilityDecision::AllowedReadOnlyProjection && valueAvailable)
		{
		}
	};
}
