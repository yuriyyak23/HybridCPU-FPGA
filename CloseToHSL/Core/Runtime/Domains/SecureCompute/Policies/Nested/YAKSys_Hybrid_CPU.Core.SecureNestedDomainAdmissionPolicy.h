#pragma once

#include "SecureNestedDomainAdmissionPolicy.h"
#include "../../Authority/YAKSys_Hybrid_CPU.Core.SecureAuthorityBounds.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureNestedDomainAdmissionPolicy final : public std::enable_shared_from_this<SecureNestedDomainAdmissionPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureNestedDomainAdmissionPolicy> Default = std::make_shared<SecureNestedDomainAdmissionPolicy>();

	public:
		static std::shared_ptr<SecureNestedDomainAdmissionPolicy> getDefault();

		std::shared_ptr<SecureNestedDomainAdmissionResult> Admit(std::shared_ptr<SecureNestedDomainAdmissionRequest> request);

		std::shared_ptr<SecureNestedDomainAdmissionResult> AdmitCheckpointPayload(SecureNestedCheckpointPayloadClass payloadClass, bool shadowVmcsStoresMutableAuthority = false);

	private:
		static std::shared_ptr<SecureNestedDomainAdmissionResult> DenyChildExpansion(std::shared_ptr<SecureAuthorityBounds> parentBounds, std::shared_ptr<SecureAuthorityBounds> childBounds);

		static std::shared_ptr<SecureNestedDomainAdmissionResult> Deny(SecureNestedDomainAdmissionDecision decision, const std::wstring &reason);
	};
}
