#pragma once

#include "SecureComputeCompatibilityProjectionSchema.h"

#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeCompatibilityProjectionSchema final : public std::enable_shared_from_this<SecureComputeCompatibilityProjectionSchema>
	{
	private:
		static inline const std::shared_ptr<SecureComputeCompatibilityProjectionSchema> Empty = std::make_shared<SecureComputeCompatibilityProjectionSchema>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<SecureComputeProjectionSchemaEntry> Entries;
		const std::vector<std::shared_ptr<SecureComputeProjectionSchemaEntry>> Entries;

	public:
		SecureComputeCompatibilityProjectionSchema();

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureComputeCompatibilityProjectionSchema(IReadOnlyList<SecureComputeProjectionSchemaEntry> entries)
		SecureComputeCompatibilityProjectionSchema(const std::vector<std::shared_ptr<SecureComputeProjectionSchemaEntry>> &entries);

		static std::shared_ptr<SecureComputeCompatibilityProjectionSchema> getEmpty();

		const std::vector<std::shared_ptr<SecureComputeProjectionSchemaEntry>> getEntries() const;

		bool isProjectionOnly() const;
	};
}
