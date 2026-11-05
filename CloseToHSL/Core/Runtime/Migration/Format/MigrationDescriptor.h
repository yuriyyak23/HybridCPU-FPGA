#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class MigrationPayloadClass : unsigned char
	{
		GuestArchitecturalState = 0,
		DomainDescriptorState = 1,
		CompatibilityProjectionMetadata = 2,
		HostOwnedRuntimeEvidence = 3,
		SchedulerEvidence = 4,
		BackendBindingEvidence = 5,
		NativeTokenEvidence = 6
	};


}
