#pragma once
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeProjectionOwnerKind : unsigned char
	{
		None = 0,
		SecureDomainDescriptor = 1,
		SecureMemoryDescriptor = 2,
		SecureEvidencePolicy = 3,
		SecureMigrationDescriptor = 4,
		SecureCompatibilityProjectionPolicy = 5
	};

	struct SecureComputeProjectionSchemaEntry final
	{
		const unsigned long long FieldId;
		const SecureComputeProjectionOwnerKind Owner;
		const bool ReadOnly;
		const bool SecureSensitive;
		const bool MigrationClassified;
		SecureComputeProjectionSchemaEntry(unsigned long long fieldId, SecureComputeProjectionOwnerKind owner,
			bool readOnly, bool secureSensitive, bool migrationClassified)
			: FieldId(fieldId), Owner(owner), ReadOnly(readOnly), SecureSensitive(secureSensitive), MigrationClassified(migrationClassified) {}
		bool operator==(const SecureComputeProjectionSchemaEntry &) const = default;
	};
}
