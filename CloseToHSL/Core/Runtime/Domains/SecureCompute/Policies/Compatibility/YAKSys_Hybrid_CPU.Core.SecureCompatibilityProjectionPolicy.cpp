#include "YAKSys_Hybrid_CPU.Core.SecureCompatibilityProjectionPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureCompatibilityProjectionPolicy::SecureCompatibilityProjectionPolicy() : SecureCompatibilityProjectionPolicy(false, 0)
	{
	}

	SecureCompatibilityProjectionPolicy::SecureCompatibilityProjectionPolicy(bool allowReadOnlyAliases, unsigned long long allowedAliasMask)
		: AllowReadOnlyAliases(allowReadOnlyAliases),
		  AllowedAliasMask(allowedAliasMask)
	{
	}

	std::shared_ptr<SecureCompatibilityProjectionPolicy> SecureCompatibilityProjectionPolicy::getDenyAll()
	{
		return DenyAll;
	}

	bool SecureCompatibilityProjectionPolicy::getAllowReadOnlyAliases() const
	{
		return AllowReadOnlyAliases;
	}

	unsigned long long SecureCompatibilityProjectionPolicy::getAllowedAliasMask() const
	{
		return AllowedAliasMask;
	}

	bool SecureCompatibilityProjectionPolicy::CanProjectAlias(unsigned long long aliasBit)
	{
		return getAllowReadOnlyAliases() && aliasBit != 0 && (getAllowedAliasMask() & aliasBit) == aliasBit;
	}

	SecureCompatibilityProjectionDecision SecureCompatibilityProjectionPolicy::ValidateReadOnlyProjection(unsigned long long aliasBit, bool hasNeutralOwner, bool hasReadOnlySource, bool evidenceAllowed, bool migrationClassified)
	{
		if (!hasNeutralOwner)
		{
			return SecureCompatibilityProjectionDecision::MissingNeutralOwner;
		}

		if (!hasReadOnlySource)
		{
			return SecureCompatibilityProjectionDecision::MissingReadOnlySource;
		}

		if (!evidenceAllowed)
		{
			return SecureCompatibilityProjectionDecision::EvidenceDenied;
		}

		if (!migrationClassified)
		{
			return SecureCompatibilityProjectionDecision::MigrationClassMissing;
		}

		return CanProjectAlias(aliasBit) ? SecureCompatibilityProjectionDecision::AllowedReadOnlyProjection : SecureCompatibilityProjectionDecision::DeniedByDefault;
	}
}
