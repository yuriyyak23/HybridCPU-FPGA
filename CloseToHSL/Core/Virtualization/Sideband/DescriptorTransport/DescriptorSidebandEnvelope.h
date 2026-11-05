#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DescriptorSidebandKind : unsigned char
	{
		None = 0,
		DomainDescriptor = 1,
		CapabilityDescriptor = 2,
		EvidencePolicy = 3,
		ObservabilityDescriptor = 4,
		MigrationDescriptor = 5,
		LaneDescriptor = 6,
		CompatibilityProjection = 7
	};


}
