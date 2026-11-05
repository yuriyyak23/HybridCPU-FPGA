#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureCompatibilityProjectionDecision : unsigned char
	{
		DeniedByDefault = 0,
		AllowedReadOnlyProjection = 1,
		MissingNeutralOwner = 2,
		MissingReadOnlySource = 3,
		EvidenceDenied = 4,
		MigrationClassMissing = 5,
		WriteDenied = 6
	};


}
