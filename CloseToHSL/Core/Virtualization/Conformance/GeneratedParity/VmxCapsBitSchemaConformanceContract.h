#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCapsBitSchemaConformanceViolation : unsigned char
	{
		None = 0,
		MissingSchema = 1,
		MissingCanonicalArtifact = 2,
		SchemaMismatch = 3,
		MissingTypedGrantSource = 4,
		MissingProjectionPolicy = 5,
		HostOnlyEvidenceProjection = 6,
		DescriptorSetClaimedAsTypedGrantSource = 7
	};


}
