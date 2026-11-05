#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmcsFieldProjectionSchemaConformanceViolation : unsigned char
	{
		None = 0,
		MissingCanonicalArtifact = 1,
		MissingFrozenField = 2,
		DuplicateField = 3,
		MissingGeneratedAlias = 4,
		MissingOwner = 5,
		EntryCountMismatch = 6,
		MissingAccessPolicy = 7,
		MissingMigrationPolicy = 8,
		CompletionFieldWritable = 9,
		HostAliasWritable = 10,
		HostEvidenceExposed = 11,
		ProjectionOnlyGuestArchitecturalState = 12,
		FieldAliasWritable = 13
	};


}
