#pragma once

#include "SecureCompatibilityProjectionPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureCompatibilityProjectionPolicy final : public std::enable_shared_from_this<SecureCompatibilityProjectionPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureCompatibilityProjectionPolicy> DenyAll = std::make_shared<SecureCompatibilityProjectionPolicy>();
		const bool AllowReadOnlyAliases;
		const unsigned long long AllowedAliasMask;

	public:
		SecureCompatibilityProjectionPolicy();

		SecureCompatibilityProjectionPolicy(bool allowReadOnlyAliases, unsigned long long allowedAliasMask);

		static std::shared_ptr<SecureCompatibilityProjectionPolicy> getDenyAll();

		bool getAllowReadOnlyAliases() const;

		unsigned long long getAllowedAliasMask() const;

		bool CanProjectAlias(unsigned long long aliasBit);

		SecureCompatibilityProjectionDecision ValidateReadOnlyProjection(unsigned long long aliasBit, bool hasNeutralOwner, bool hasReadOnlySource, bool evidenceAllowed, bool migrationClassified);
	};
}
