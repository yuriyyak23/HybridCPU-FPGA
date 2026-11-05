#include "YAKSys_Hybrid_CPU.Core.SecureGrantAuthorityPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureGrantAuthorityPolicy> SecureGrantAuthorityPolicy::getDefault()
	{
		return Default;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureGrantAuthorityResult Validate(SecureGrantHandle handle, SecureGrantMaterializationSource source, SecureAuthorityBounds grantedBounds, SecureAuthorityBounds requestedBounds, SecureGrantEpochSet epochs, bool runtimeOwnerMaterialized, CapabilityDescriptorSet? capabilities = null, CapabilityGrantScope requiredScope = CapabilityGrantScope.None, bool grantRevoked = false)
	std::shared_ptr<SecureGrantAuthorityResult> SecureGrantAuthorityPolicy::Validate(std::shared_ptr<SecureGrantHandle> handle, SecureGrantMaterializationSource source, std::shared_ptr<SecureAuthorityBounds> grantedBounds, std::shared_ptr<SecureAuthorityBounds> requestedBounds, std::shared_ptr<SecureGrantEpochSet> epochs, bool runtimeOwnerMaterialized, std::shared_ptr<CapabilityDescriptorSet> capabilities, CapabilityGrantScope requiredScope, bool grantRevoked)
	{
		if (source == SecureGrantMaterializationSource::GuestArchitecturalState)
		{
			return Deny(SecureGrantAuthorityDecision::DeniedGuestScalarMaterialization, L"Guest-visible scalar values cannot materialize secure grant authority.");
		}

		if (source == SecureGrantMaterializationSource::CompatibilityProjection || requiredScope == CapabilityGrantScope::CompatibilityProjection)
		{
			return Deny(SecureGrantAuthorityDecision::DeniedCompatibilityProjectionAuthority, L"Compatibility projection scope cannot satisfy secure grant authority.");
		}

		if (!runtimeOwnerMaterialized)
		{
			return Deny(SecureGrantAuthorityDecision::DeniedMissingRuntimeOwner, L"Secure grant validation requires a neutral runtime owner.");
		}

		if (handle == nullptr || !handle->hasScalarShape() || !handle->hasProvenance())
		{
			return Deny(SecureGrantAuthorityDecision::DeniedMissingProvenance, L"Secure grant handle requires runtime provenance.");
		}

		if (grantRevoked)
		{
			return Deny(SecureGrantAuthorityDecision::DeniedRevokedGrant, L"Revoked secure grants cannot satisfy admission.");
		}

		if (epochs == nullptr || !epochs->AllCurrentFor(handle))
		{
			return Deny(SecureGrantAuthorityDecision::DeniedStaleEpoch, L"Secure grant handle must match current domain, memory, grant and provenance epochs.");
		}

		if (grantedBounds == nullptr || requestedBounds == nullptr || !requestedBounds->IsSubsetOf(grantedBounds))
		{
			return Deny(SecureGrantAuthorityDecision::DeniedAuthorityBounds, L"Secure grant requested authority exceeds granted bounds.");
		}

		if (requiredScope != CapabilityGrantScope::None && !CapabilityBoundaryRequirement::TypedGrant(handle->LocalId, requiredScope)->IsSatisfiedBy((capabilities != nullptr) ? capabilities : CapabilityDescriptorSet::getEmpty()))
		{
			return Deny(SecureGrantAuthorityDecision::DeniedMissingTypedGrant, L"Secure grant handle requires a matching neutral typed grant.");
		}

		return SecureGrantAuthorityResult::Allowed;
	}

	std::shared_ptr<SecureGrantAuthorityResult> SecureGrantAuthorityPolicy::Deny(SecureGrantAuthorityDecision decision, const std::wstring &reason)
	{
		return SecureGrantAuthorityResult::Denied(decision, reason);
	}
}
