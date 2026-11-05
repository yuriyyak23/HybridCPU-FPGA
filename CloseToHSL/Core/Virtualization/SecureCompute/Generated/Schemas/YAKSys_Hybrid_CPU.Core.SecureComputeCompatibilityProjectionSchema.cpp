#include "YAKSys_Hybrid_CPU.Core.SecureComputeCompatibilityProjectionSchema.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeCompatibilityProjectionSchema::SecureComputeCompatibilityProjectionSchema() : SecureComputeCompatibilityProjectionSchema(std::vector<std::shared_ptr<SecureComputeProjectionSchemaEntry>>{})
	{
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureComputeCompatibilityProjectionSchema(IReadOnlyList<SecureComputeProjectionSchemaEntry> entries)
	SecureComputeCompatibilityProjectionSchema::SecureComputeCompatibilityProjectionSchema(const std::vector<std::shared_ptr<SecureComputeProjectionSchemaEntry>> &entries)
		: Entries(entries)
	{
	}

	std::shared_ptr<SecureComputeCompatibilityProjectionSchema> SecureComputeCompatibilityProjectionSchema::getEmpty()
	{
		return Empty;
	}

	const std::vector<std::shared_ptr<SecureComputeProjectionSchemaEntry>> SecureComputeCompatibilityProjectionSchema::getEntries() const
	{
		return Entries;
	}

	bool SecureComputeCompatibilityProjectionSchema::isProjectionOnly() const
	{
		return true;
	}
}
