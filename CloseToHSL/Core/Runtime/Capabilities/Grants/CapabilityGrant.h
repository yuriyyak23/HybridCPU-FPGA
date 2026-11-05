#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CapabilityGrantScope : unsigned char
	{
		None = 0,
		HardwareAvailable = 1,
		RuntimeEnabled = 2,
		DomainGranted = 3,
		CompatibilityProjection = 4
	};

	enum class CapabilityDelegationPolicy : unsigned char
	{
		None = 0,
		NonDelegable = 1,
		DelegableWithinOwnerDomain = 2
	};

	enum class CapabilityRevocationPolicy : unsigned char
	{
		None = 0,
		RuntimeRevocable = 1,
		RevokedOnMigration = 2
	};

	enum class CapabilityMigrationClass : unsigned char
	{
		None = 0,
		DomainLocal = 1,
		MigratableDescriptor = 2
	};

	enum class CapabilityEvidenceVisibility : unsigned char
	{
		None = 0,
		HostOnly = 1,
		GuestVisibleProjection = 2
	};

	enum class CapabilityFrontendProjectionPolicy : unsigned char
	{
		None = 0,
		NeverProject = 1,
		ProjectIfCompatible = 2
	};



}
