#include "YAKSys_Hybrid_CPU.Core.SecureHostInspectionPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureHostInspectionPolicy::SecureHostInspectionPolicy() : SecureHostInspectionPolicy(SecureHostInspectionMode::Denied, false)
	{
	}

	SecureHostInspectionPolicy::SecureHostInspectionPolicy(SecureHostInspectionMode mode, bool allowPrivateMemoryInspection)
		: Mode(mode), AllowPrivateMemoryInspection(allowPrivateMemoryInspection)
	{
	}

	std::shared_ptr<SecureHostInspectionPolicy> SecureHostInspectionPolicy::getDenyAll()
	{
		return DenyAll;
	}

	SecureHostInspectionMode SecureHostInspectionPolicy::getMode() const
	{
		return Mode;
	}

	bool SecureHostInspectionPolicy::getAllowPrivateMemoryInspection() const
	{
		return AllowPrivateMemoryInspection;
	}

	bool SecureHostInspectionPolicy::getCanInspectPrivateMemory() const
	{
		return getMode() != SecureHostInspectionMode::Denied && getAllowPrivateMemoryInspection();
	}
}
