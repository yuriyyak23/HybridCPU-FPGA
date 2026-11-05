#pragma once

#include "../../Authority/SecureRevocationEpoch.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureBackendOwnerSource : unsigned char
	{
		None = 0,
		NeutralRuntimeService = 1,
		NeutralDeviceModel = 2,
		NeutralMigrationService = 3,
		CompatibilityProjection = 4,
		VmxFrontend = 5,
		VmcsProjection = 6,
		VmxCapsProjection = 7,
		ShadowVmcsProjection = 8
	};

	enum class SecureBackendRfcAdrState : unsigned char
	{
		Missing = 0,
		Draft = 1,
		Approved = 2
	};

struct SecureBackendOwnerDescriptor final {
    const unsigned long long OwnerId;
    const SecureBackendOwnerSource Source;
    const unsigned long long PolicyDigest;
    const unsigned long long ProofDigest;
    const std::shared_ptr<SecureRevocationEpoch> Epoch;
    const bool Materialized;
    const bool GrantProofValidated;
    const bool EvidenceProofValidated;
    const bool CompletionFenceValidated;
    const bool RetireFenceValidated;
    const bool NegativeTestsPresent;
    const bool HasIdentity;
    const bool IsNeutralSource;
    const bool HasProofChain;

    SecureBackendOwnerDescriptor(unsigned long long ownerId, SecureBackendOwnerSource source,
        unsigned long long policyDigest, unsigned long long proofDigest,
        std::shared_ptr<SecureRevocationEpoch> epoch, bool materialized,
        bool grantProofValidated, bool evidenceProofValidated,
        bool completionFenceValidated, bool retireFenceValidated,
        bool negativeTestsPresent)
        : OwnerId(ownerId), Source(source), PolicyDigest(policyDigest), ProofDigest(proofDigest),
          Epoch(epoch ? std::move(epoch) : SecureRevocationEpoch::Unmaterialized),
          Materialized(materialized), GrantProofValidated(grantProofValidated),
          EvidenceProofValidated(evidenceProofValidated), CompletionFenceValidated(completionFenceValidated),
          RetireFenceValidated(retireFenceValidated), NegativeTestsPresent(negativeTestsPresent),
          HasIdentity(ownerId != 0),
          IsNeutralSource(source == SecureBackendOwnerSource::NeutralRuntimeService ||
              source == SecureBackendOwnerSource::NeutralDeviceModel ||
              source == SecureBackendOwnerSource::NeutralMigrationService),
          HasProofChain(policyDigest != 0 && proofDigest != 0 && grantProofValidated &&
              evidenceProofValidated && completionFenceValidated && retireFenceValidated) {}

    static const std::shared_ptr<SecureBackendOwnerDescriptor> None;
};

inline const std::shared_ptr<SecureBackendOwnerDescriptor> SecureBackendOwnerDescriptor::None =
    std::make_shared<SecureBackendOwnerDescriptor>(0, SecureBackendOwnerSource::None, 0, 0,
        SecureRevocationEpoch::Unmaterialized, false, false, false, false, false, false);

}
