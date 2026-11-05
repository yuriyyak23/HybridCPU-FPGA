#pragma once

#include "MemoryTranslationPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class MemoryTranslationPolicy final : public std::enable_shared_from_this<MemoryTranslationPolicy>
	{
	private:
		const MemoryTranslationAuthority Authority;
		const TranslationInvalidationPermission InvalidationPermissions;
		const bool RequireFenceBeforeInvalidation;
		const bool AllowCompatibilityProjection;

	public:
		MemoryTranslationPolicy();

		MemoryTranslationPolicy(MemoryTranslationAuthority authority, TranslationInvalidationPermission invalidationPermissions, bool requireFenceBeforeInvalidation, bool allowCompatibilityProjection);

		MemoryTranslationAuthority getAuthority() const;

		TranslationInvalidationPermission getInvalidationPermissions() const;

		bool getRequireFenceBeforeInvalidation() const;

		bool getAllowCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool AllowsInvalidation(TranslationInvalidationPermission requestedPermission);

		std::shared_ptr<MemoryTranslationPolicy> WithInvalidationPermissions(TranslationInvalidationPermission permissions);
	};
}
