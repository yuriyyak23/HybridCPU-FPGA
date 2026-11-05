#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeDescriptorSidebandKind : unsigned char
	{
		DomainDescriptor = 0,
		MemoryDescriptor = 1,
		EvidencePolicy = 2,
		MigrationDescriptor = 3,
		IoDescriptor = 4,
		HypercallDescriptor = 5
	};


}
