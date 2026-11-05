#include "YAKSys_Hybrid_CPU.Core.MemoryTranslationPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	MemoryTranslationPolicy::MemoryTranslationPolicy() : MemoryTranslationPolicy(MemoryTranslationAuthority::RuntimeOwned, TranslationInvalidationPermission::None, true, false)
	{
	}

	MemoryTranslationPolicy::MemoryTranslationPolicy(MemoryTranslationAuthority authority, TranslationInvalidationPermission invalidationPermissions, bool requireFenceBeforeInvalidation, bool allowCompatibilityProjection)
		: Authority(authority),
		  InvalidationPermissions(invalidationPermissions),
		  RequireFenceBeforeInvalidation(requireFenceBeforeInvalidation),
		  AllowCompatibilityProjection(allowCompatibilityProjection)
	{
	}

	MemoryTranslationAuthority MemoryTranslationPolicy::getAuthority() const
	{
		return Authority;
	}

	TranslationInvalidationPermission MemoryTranslationPolicy::getInvalidationPermissions() const
	{
		return InvalidationPermissions;
	}

	bool MemoryTranslationPolicy::getRequireFenceBeforeInvalidation() const
	{
		return RequireFenceBeforeInvalidation;
	}

	bool MemoryTranslationPolicy::getAllowCompatibilityProjection() const
	{
		return AllowCompatibilityProjection;
	}

	bool MemoryTranslationPolicy::isRuntimeAuthoritative() const
	{
		return getAuthority() == MemoryTranslationAuthority::RuntimeOwned;
	}

	bool MemoryTranslationPolicy::AllowsInvalidation(TranslationInvalidationPermission requestedPermission)
	{
		const auto granted = static_cast<unsigned char>(getInvalidationPermissions());
		const auto requested = static_cast<unsigned char>(requestedPermission);
		return isRuntimeAuthoritative() && requestedPermission != TranslationInvalidationPermission::None && (granted & requested) == requested;
	}

	std::shared_ptr<MemoryTranslationPolicy> MemoryTranslationPolicy::WithInvalidationPermissions(TranslationInvalidationPermission permissions)
	{
		return std::make_shared<MemoryTranslationPolicy>(getAuthority(), permissions, getRequireFenceBeforeInvalidation(), getAllowCompatibilityProjection());
	}
}
