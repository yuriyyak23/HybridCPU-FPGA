#pragma once

#include "SecureRevocationEpoch.h"
#include "SecureGrantHandle.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class SecureGrantMaterializationSource : unsigned char
	{
		None = 0,
		NeutralRuntimeOwner = 1,
		MigrationRestore = 2,
		GuestArchitecturalState = 3,
		CompatibilityProjection = 4
	};

	enum class SecureGrantAuthorityDecision : unsigned char
	{
		Allowed = 0,
		DeniedMissingRuntimeOwner = 1,
		DeniedGuestScalarMaterialization = 2,
		DeniedCompatibilityProjectionAuthority = 3,
		DeniedMissingProvenance = 4,
		DeniedStaleEpoch = 5,
		DeniedAuthorityBounds = 6,
		DeniedMissingTypedGrant = 7,
		DeniedRevokedGrant = 8
	};

	struct SecureGrantEpochSet final {
		std::shared_ptr<SecureRevocationEpoch> DomainPolicyEpoch;
		std::shared_ptr<SecureRevocationEpoch> MemoryPolicyEpoch;
		std::shared_ptr<SecureRevocationEpoch> GrantCollectionEpoch;
		std::shared_ptr<SecureRevocationEpoch> ProvenanceEpoch;

		SecureGrantEpochSet(std::shared_ptr<SecureRevocationEpoch> domain, std::shared_ptr<SecureRevocationEpoch> memory, std::shared_ptr<SecureRevocationEpoch> grants, std::shared_ptr<SecureRevocationEpoch> provenance)
			: DomainPolicyEpoch(std::move(domain)), MemoryPolicyEpoch(std::move(memory)), GrantCollectionEpoch(std::move(grants)), ProvenanceEpoch(std::move(provenance)) {}

		static std::shared_ptr<SecureGrantEpochSet> Single(std::shared_ptr<SecureRevocationEpoch> epoch) {
			return std::make_shared<SecureGrantEpochSet>(epoch, epoch, epoch, epoch);
		}

		bool AllCurrentFor(const std::shared_ptr<SecureGrantHandle>& handle) const {
			return handle != nullptr && DomainPolicyEpoch && MemoryPolicyEpoch && GrantCollectionEpoch && ProvenanceEpoch
				&& DomainPolicyEpoch->IsCurrent(handle->Epoch) && MemoryPolicyEpoch->IsCurrent(handle->Epoch)
				&& GrantCollectionEpoch->IsCurrent(handle->Epoch) && ProvenanceEpoch->IsCurrent(handle->Epoch);
		}
	};

	struct SecureGrantAuthorityResult final {
		const SecureGrantAuthorityDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;
		SecureGrantAuthorityResult(SecureGrantAuthorityDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == SecureGrantAuthorityDecision::Allowed) {}
		static const std::shared_ptr<SecureGrantAuthorityResult> Allowed;
		static std::shared_ptr<SecureGrantAuthorityResult> Denied(SecureGrantAuthorityDecision decision, std::wstring reason) {
			return std::make_shared<SecureGrantAuthorityResult>(decision, std::move(reason));
		}
	};

	inline const std::shared_ptr<SecureGrantAuthorityResult> SecureGrantAuthorityResult::Allowed = std::make_shared<SecureGrantAuthorityResult>(SecureGrantAuthorityDecision::Allowed, L"");
}
