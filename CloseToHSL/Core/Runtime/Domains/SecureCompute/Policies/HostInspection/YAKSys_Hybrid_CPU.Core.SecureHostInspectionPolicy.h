#pragma once

#include "SecureHostInspectionPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureHostInspectionPolicy final : public std::enable_shared_from_this<SecureHostInspectionPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureHostInspectionPolicy> DenyAll = std::make_shared<SecureHostInspectionPolicy>();
		const SecureHostInspectionMode Mode;
		const bool AllowPrivateMemoryInspection;

	public:
		SecureHostInspectionPolicy();

		SecureHostInspectionPolicy(SecureHostInspectionMode mode, bool allowPrivateMemoryInspection);

		static std::shared_ptr<SecureHostInspectionPolicy> getDenyAll();

		SecureHostInspectionMode getMode() const;

		bool getAllowPrivateMemoryInspection() const;

		bool getCanInspectPrivateMemory() const;
	};
}
