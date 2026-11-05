#pragma once

#include "YAKSys_Hybrid_CPU.Core.SecureAuthorityBounds.h"
#include "../../../Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "../../../Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "SecureGrantHandle.h"
#include "../../../Capabilities/CapabilityBoundaryRequirement.h"
#include "SecureGrantAuthorityPolicy.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureGrantAuthorityPolicy final : public std::enable_shared_from_this<SecureGrantAuthorityPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureGrantAuthorityPolicy> Default = std::make_shared<SecureGrantAuthorityPolicy>();

	public:
		static std::shared_ptr<SecureGrantAuthorityPolicy> getDefault();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureGrantAuthorityResult Validate(SecureGrantHandle handle, SecureGrantMaterializationSource source, SecureAuthorityBounds grantedBounds, SecureAuthorityBounds requestedBounds, SecureGrantEpochSet epochs, bool runtimeOwnerMaterialized, CapabilityDescriptorSet? capabilities = null, CapabilityGrantScope requiredScope = CapabilityGrantScope.None, bool grantRevoked = false)
		std::shared_ptr<SecureGrantAuthorityResult> Validate(std::shared_ptr<SecureGrantHandle> handle, SecureGrantMaterializationSource source, std::shared_ptr<SecureAuthorityBounds> grantedBounds, std::shared_ptr<SecureAuthorityBounds> requestedBounds, std::shared_ptr<SecureGrantEpochSet> epochs, bool runtimeOwnerMaterialized, std::shared_ptr<CapabilityDescriptorSet> capabilities = nullptr, CapabilityGrantScope requiredScope = CapabilityGrantScope::None, bool grantRevoked = false);

	private:
		static std::shared_ptr<SecureGrantAuthorityResult> Deny(SecureGrantAuthorityDecision decision, const std::wstring &reason);
	};
}
