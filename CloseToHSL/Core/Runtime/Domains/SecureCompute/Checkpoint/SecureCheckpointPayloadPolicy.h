#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureCheckpointPayloadClass : unsigned char
	{
		GuestVisibleState = 0,
		SecurePolicyDescriptor = 1,
		SecureSharedMemory = 2,
		SecurePrivateMemory = 3,
		HostOwnedEvidence = 4,
		SchedulerEvidence = 5,
		BackendBindingEvidence = 6,
		NativeTokenEvidence = 7,
		DebugTrace = 8,
		VmcsProjectionMetadata = 9,
		RawMeasurementSecret = 10,
		CompatibilityProjectionMetadata = 11,
		ActiveHostPointer = 12,
		RawSealingKey = 13
	};

	enum class SecureCheckpointPayloadDecision : unsigned char
	{
		Allowed = 0,
		DeniedHostOwnedEvidence = 1,
		DeniedCompatibilityProjectionAuthority = 2,
		DeniedPrivateMemoryWithoutSealedPayload = 3,
		DeniedDebugTraceAsGuestState = 4,
		DeniedRawMeasurementSecret = 5,
		DeniedActiveHostPointer = 6,
		DeniedRawSealingKey = 7
	};


}
