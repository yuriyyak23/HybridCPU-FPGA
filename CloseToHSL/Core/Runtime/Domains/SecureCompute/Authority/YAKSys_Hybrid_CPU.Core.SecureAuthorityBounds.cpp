#include "YAKSys_Hybrid_CPU.Core.SecureAuthorityBounds.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureAuthorityBounds::SecureAuthorityBounds() : SecureAuthorityBounds(false, false, false, false, false, false, false, false)
	{
	}

	SecureAuthorityBounds::SecureAuthorityBounds(bool allowsPrivateMemory, bool allowsSharedMemory, bool allowsIo, bool allowsDma, bool allowsHypercalls, bool allowsDebug, bool allowsMigration, bool allowsCompatibilityProjection)
		: AllowsPrivateMemory(allowsPrivateMemory),
		  AllowsSharedMemory(allowsSharedMemory),
		  AllowsIo(allowsIo),
		  AllowsDma(allowsDma),
		  AllowsHypercalls(allowsHypercalls),
		  AllowsDebug(allowsDebug),
		  AllowsMigration(allowsMigration),
		  AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	std::shared_ptr<SecureAuthorityBounds> SecureAuthorityBounds::getNone()
	{
		return None;
	}

	bool SecureAuthorityBounds::getAllowsPrivateMemory() const
	{
		return AllowsPrivateMemory;
	}

	bool SecureAuthorityBounds::getAllowsSharedMemory() const
	{
		return AllowsSharedMemory;
	}

	bool SecureAuthorityBounds::getAllowsIo() const
	{
		return AllowsIo;
	}

	bool SecureAuthorityBounds::getAllowsDma() const
	{
		return AllowsDma;
	}

	bool SecureAuthorityBounds::getAllowsHypercalls() const
	{
		return AllowsHypercalls;
	}

	bool SecureAuthorityBounds::getAllowsDebug() const
	{
		return AllowsDebug;
	}

	bool SecureAuthorityBounds::getAllowsMigration() const
	{
		return AllowsMigration;
	}

	bool SecureAuthorityBounds::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool SecureAuthorityBounds::IsSubsetOf(std::shared_ptr<SecureAuthorityBounds> parent)
	{
		if (!parent)
		{
			return false;
		}
		return (!getAllowsPrivateMemory() || parent->getAllowsPrivateMemory()) && (!getAllowsSharedMemory() || parent->getAllowsSharedMemory()) && (!getAllowsIo() || parent->getAllowsIo()) && (!getAllowsDma() || parent->getAllowsDma()) && (!getAllowsHypercalls() || parent->getAllowsHypercalls()) && (!getAllowsDebug() || parent->getAllowsDebug()) && (!getAllowsMigration() || parent->getAllowsMigration()) && (!getAllowsCompatibilityProjection() || parent->getAllowsCompatibilityProjection());
	}
}
