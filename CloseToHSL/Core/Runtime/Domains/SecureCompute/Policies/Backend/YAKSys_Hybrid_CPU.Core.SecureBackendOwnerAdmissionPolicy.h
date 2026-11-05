#pragma once

#include "SecureBackendOwnerAdmissionPolicy.h"
#include <string>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureBackendOwnerAdmissionPolicy final : public std::enable_shared_from_this<SecureBackendOwnerAdmissionPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureBackendOwnerAdmissionPolicy> Default = std::make_shared<SecureBackendOwnerAdmissionPolicy>();

	public:
		static std::shared_ptr<SecureBackendOwnerAdmissionPolicy> getDefault();

		std::shared_ptr<SecureBackendOwnerAdmissionResult> Admit(std::shared_ptr<SecureBackendOwnerAdmissionRequest> request);

	private:
		static std::shared_ptr<SecureBackendOwnerAdmissionResult> Deny(SecureBackendOwnerAdmissionDecision decision, const std::wstring &reason);
	};
}
