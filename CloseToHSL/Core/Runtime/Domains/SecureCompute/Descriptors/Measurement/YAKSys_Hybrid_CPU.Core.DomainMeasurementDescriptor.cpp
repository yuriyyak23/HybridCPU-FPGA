#include "YAKSys_Hybrid_CPU.Core.DomainMeasurementDescriptor.h"
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	DomainMeasurementDescriptor::DomainMeasurementDescriptor() : DomainMeasurementDescriptor(SecureMeasurementHandle::None, SecureMeasurementState::Missing, SecureMeasurementDebugClass::Production, 0, 0, 0)
	{
	}

	DomainMeasurementDescriptor::DomainMeasurementDescriptor(std::shared_ptr<SecureMeasurementHandle> handle, SecureMeasurementState state, SecureMeasurementDebugClass debugClass, unsigned long long policyDigest, unsigned long long memoryDigest, unsigned long long runtimeDigest) : DomainMeasurementDescriptor(handle, state, debugClass, policyDigest, memoryDigest, runtimeDigest, SecureEvidenceVisibilityClass::HostOwnedQuarantined, 0, 0, 0)
	{
	}

	DomainMeasurementDescriptor::DomainMeasurementDescriptor(std::shared_ptr<SecureMeasurementHandle> handle, SecureMeasurementState state, SecureMeasurementDebugClass debugClass, unsigned long long policyDigest, unsigned long long memoryDigest, unsigned long long runtimeDigest, SecureEvidenceVisibilityClass attestationEvidenceClass, unsigned long long creatorDomainTag, unsigned long long parentMeasurementId, unsigned long long policySourceHash)
		: Handle(handle != nullptr ? std::move(handle) : SecureMeasurementHandle::None),
		  State(state),
		  DebugClass(debugClass),
		  PolicyDigest(policyDigest),
		  MemoryDigest(memoryDigest),
		  RuntimeDigest(runtimeDigest),
		  AttestationEvidenceClass(attestationEvidenceClass),
		  CreatorDomainTag(creatorDomainTag),
		  ParentMeasurementId(parentMeasurementId),
		  PolicySourceHash(policySourceHash)
	{
	}

	std::shared_ptr<DomainMeasurementDescriptor> DomainMeasurementDescriptor::getMissing()
	{
		return Missing;
	}

	std::shared_ptr<SecureMeasurementHandle> DomainMeasurementDescriptor::getHandle() const
	{
		return Handle;
	}

	SecureMeasurementState DomainMeasurementDescriptor::getState() const
	{
		return State;
	}

	SecureMeasurementDebugClass DomainMeasurementDescriptor::getDebugClass() const
	{
		return DebugClass;
	}

	unsigned long long DomainMeasurementDescriptor::getPolicyDigest() const
	{
		return PolicyDigest;
	}

	unsigned long long DomainMeasurementDescriptor::getMemoryDigest() const
	{
		return MemoryDigest;
	}

	unsigned long long DomainMeasurementDescriptor::getRuntimeDigest() const
	{
		return RuntimeDigest;
	}

	SecureEvidenceVisibilityClass DomainMeasurementDescriptor::getAttestationEvidenceClass() const
	{
		return AttestationEvidenceClass;
	}

	unsigned long long DomainMeasurementDescriptor::getCreatorDomainTag() const
	{
		return CreatorDomainTag;
	}

	unsigned long long DomainMeasurementDescriptor::getParentMeasurementId() const
	{
		return ParentMeasurementId;
	}

	unsigned long long DomainMeasurementDescriptor::getPolicySourceHash() const
	{
		return PolicySourceHash;
	}

	unsigned long long DomainMeasurementDescriptor::getMeasurementEpoch() const
	{
		return getHandle()->Epoch;
	}

	bool DomainMeasurementDescriptor::isMaterialized() const
	{
		return getState() == SecureMeasurementState::Materialized && getHandle()->IsMaterialized && getPolicyDigest() != 0 && getCreatorDomainTag() != 0 && getPolicySourceHash() != 0;
	}

	bool DomainMeasurementDescriptor::isAttestationEvidenceOnly() const
	{
		return true;
	}

	bool DomainMeasurementDescriptor::isStale() const
	{
		return ([&] { const auto codexPatternValue = getState(); return codexPatternValue == SecureMeasurementState::Stale || codexPatternValue == SecureMeasurementState::Revoked; }());
	}

	bool DomainMeasurementDescriptor::IsCurrentFor(std::shared_ptr<SecureRevocationEpoch> policyEpoch)
	{
		return getHandle()->MatchesEpoch(policyEpoch);
	}

	bool DomainMeasurementDescriptor::BindsDomain(unsigned long long domainTag)
	{
		return getCreatorDomainTag() != 0 && getCreatorDomainTag() == domainTag;
	}

	bool DomainMeasurementDescriptor::BindsPolicyDigest(unsigned long long expectedPolicyDigest)
	{
		return expectedPolicyDigest != 0 && getPolicyDigest() == expectedPolicyDigest;
	}

	bool DomainMeasurementDescriptor::BindsMemoryDigest(unsigned long long expectedMemoryDigest)
	{
		return expectedMemoryDigest == 0 || getMemoryDigest() == expectedMemoryDigest;
	}

	bool DomainMeasurementDescriptor::MustRevalidateOnRestore(std::shared_ptr<SecureMigrationDescriptor> migrationPolicy)
	{
		return migrationPolicy->getRequiresReattestation() || getAttestationEvidenceClass() == SecureEvidenceVisibilityClass::RecomputedAfterRestore;
	}

	unsigned long long DomainMeasurementDescriptor::ComputePolicyDigest(std::shared_ptr<SecureComputeDomainDescriptor> descriptor)
	{
		unsigned long long digest = Mix(0x5EC0D1AULL, descriptor->getDomainTag());
		digest = Mix(digest, static_cast<unsigned long long>(descriptor->getSecurityLevel()));
		digest = Mix(digest, descriptor->getMeasurementRequired() ? 1ULL : 0ULL);
		digest = Mix(digest, descriptor->getPrivateMemoryRequired() ? 1ULL : 0ULL);
		digest = Mix(digest, static_cast<unsigned long long>(descriptor->getHostInspectionPolicy()->getMode()));
		digest = Mix(digest, descriptor->getHostInspectionPolicy()->getAllowPrivateMemoryInspection() ? 1ULL : 0ULL);
		digest = Mix(digest, descriptor->getEvidenceVisibilityPolicy()->getAllowGuestVisibleEvidence() ? 1ULL : 0ULL);
		digest = Mix(digest, descriptor->getEvidenceVisibilityPolicy()->getAllowMigrationSerializableEvidence() ? 1ULL : 0ULL);
		digest = Mix(digest, descriptor->getEvidenceVisibilityPolicy()->getAllowCompatibilityAliasEvidence() ? 1ULL : 0ULL);
		digest = Mix(digest, descriptor->getEvidenceVisibilityPolicy()->getAllowDebugEvidence() ? 1ULL : 0ULL);
		digest = Mix(digest, static_cast<unsigned long long>(descriptor->getMigrationPolicy()->getMode()));
		digest = Mix(digest, static_cast<unsigned long long>(descriptor->getMigrationPolicy()->getPrivateMemoryPolicy()));
		digest = Mix(digest, descriptor->getMigrationPolicy()->getPolicyEpoch()->Current);
		digest = Mix(digest, static_cast<unsigned long long>(descriptor->getMigrationPolicy()->getMeasurementRestorePolicy()));
		digest = Mix(digest, static_cast<unsigned long long>(descriptor->getMigrationPolicy()->getGrantRestorePolicy()));
		digest = MixIoPolicy(digest, descriptor->getIoPolicy());
		digest = MixHypercallPolicy(digest, descriptor->getHypercallPolicy());
		digest = Mix(digest, static_cast<unsigned long long>(descriptor->getDebugPolicy()->getMode()));
		digest = Mix(digest, descriptor->getDebugPolicy()->getChangesMeasurementClass() ? 1ULL : 0ULL);
		digest = Mix(digest, descriptor->getCompatibilityProjectionPolicy()->getAllowReadOnlyAliases() ? 1ULL : 0ULL);
		digest = Mix(digest, descriptor->getCompatibilityProjectionPolicy()->getAllowedAliasMask());
		return NonZero(digest);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ulong ComputeMemoryDigest(SecureMemoryDomainDescriptor? memory)
	unsigned long long DomainMeasurementDescriptor::ComputeMemoryDigest(std::shared_ptr<SecureMemoryDomainDescriptor> memory)
	{
		if (memory == nullptr || !memory->isMaterialized())
		{
			return 0;
		}

		unsigned long long digest = Mix(0x5EC0D2AULL, memory->getDomainTag());
		digest = Mix(digest, memory->getAddressSpaceTag());
		digest = Mix(digest, memory->getPolicyEpoch()->Current);
		digest = Mix(digest, static_cast<unsigned long long>(memory->getDmaPolicy()));

		for (auto region : memory->getRegions())
		{
			digest = Mix(digest, static_cast<unsigned long long>(region->RegionClass));
			digest = Mix(digest, region->Start);
			digest = Mix(digest, region->Length);
			digest = Mix(digest, static_cast<unsigned long long>(region->HostVisibility));
			digest = Mix(digest, region->PolicyEpoch);
			digest = Mix(digest, static_cast<unsigned long long>(region->RuntimeDirtyPolicy));
			digest = Mix(digest, static_cast<unsigned long long>(region->RuntimeMigrationClass));
		}

		return NonZero(digest);
	}

	unsigned long long DomainMeasurementDescriptor::Mix(unsigned long long digest, unsigned long long value)
	{
		return ((digest << 7) | (digest >> 57)) ^ value ^ 0x9E3779B97F4A7C15ULL;
	}

	unsigned long long DomainMeasurementDescriptor::MixIoPolicy(unsigned long long digest, std::shared_ptr<SecureIoDomainDescriptor> policy)
	{
		digest = Mix(digest, static_cast<unsigned long long>(policy->getDmaPolicy()));
		digest = Mix(digest, policy->getRequireCompletionFence() ? 1ULL : 0ULL);
		digest = Mix(digest, policy->getNeutralIoOwnerMaterialized() ? 1ULL : 0ULL);

		for (auto buffer : policy->getSharedBuffers())
		{
			digest = Mix(digest, buffer->BufferId);
			digest = Mix(digest, buffer->Start);
			digest = Mix(digest, buffer->Length);
			digest = Mix(digest, static_cast<unsigned long long>(buffer->Direction));
			digest = MixGrantHandle(digest, buffer->Grant);
			digest = Mix(digest, static_cast<unsigned long long>(buffer->EvidenceClass));
			digest = Mix(digest, buffer->OwnerDomainTag);
			digest = Mix(digest, buffer->LifetimeEpoch);
		}

		return digest;
	}

	unsigned long long DomainMeasurementDescriptor::MixHypercallPolicy(unsigned long long digest, std::shared_ptr<SecureHypercallDescriptor> policy)
	{
		digest = Mix(digest, policy->getNeutralBackendOwnerRequired() ? 1ULL : 0ULL);
		digest = Mix(digest, policy->getAllowBackendExecution() ? 1ULL : 0ULL);
		digest = MixGrantHandle(digest, policy->getRequiredGrant());
		digest = Mix(digest, policy->getRequireEvidenceApproval() ? 1ULL : 0ULL);
		digest = Mix(digest, policy->getRequireCompletionFence() ? 1ULL : 0ULL);
		digest = Mix(digest, policy->getRequireRetirePublicationRule() ? 1ULL : 0ULL);

		for (auto hypercallId : policy->getAllowedHypercallIds())
		{
			digest = Mix(digest, hypercallId);
		}

		for (auto argument : policy->getArguments())
		{
			digest = Mix(digest, argument->Index);
			digest = Mix(digest, static_cast<unsigned long long>(argument->ArgumentClass));
			digest = Mix(digest, argument->SharedBufferId);
			digest = MixGrantHandle(digest, argument->Grant);
		}

		return digest;
	}

	unsigned long long DomainMeasurementDescriptor::MixGrantHandle(unsigned long long digest, SecureGrantHandle grant)
	{
		digest = Mix(digest, static_cast<unsigned long long>(grant.Kind));
		digest = Mix(digest, grant.LocalId);
		digest = Mix(digest, grant.ProvenanceHash);
		return Mix(digest, grant.Epoch);
	}

	unsigned long long DomainMeasurementDescriptor::MixGrantHandle(unsigned long long digest, std::shared_ptr<SecureGrantHandle> grant)
	{
		return MixGrantHandle(digest, grant != nullptr ? *grant : SecureGrantHandle{});
	}

	unsigned long long DomainMeasurementDescriptor::NonZero(unsigned long long value)
	{
		return value == 0 ? 1ULL : value;
	}
}
