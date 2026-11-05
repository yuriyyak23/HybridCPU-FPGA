#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainValidationFailureReason : unsigned char
	{
		None = 0,
		MissingExecutionDomain = 1,
		MissingMemoryDomain = 2,
		MissingIoDomain = 3,
		MissingCapabilityGrant = 4,
		ProjectionCannotMutateState = 5,
		UnsupportedCompatibilityAlias = 6,
		MissingBundleLegalityDescriptor = 7,
		MissingSchedulingBudgetDescriptor = 8,
		RuntimeAuthorityMissing = 9,
		CompatibilityProjectionDenied = 10,
		SchedulingLaneRejected = 11,
		SchedulingBudgetExceeded = 12,
		MissingTrapPolicyDescriptor = 13,
		TrapPolicyClassDenied = 14,
		MissingEventQueueDescriptor = 15,
		EventQueueRejected = 16,
		MissingCompletionRouteDescriptor = 17,
		CompletionSourceDenied = 18,
		InvalidOperationAuthorityClass = 19
	};


}
