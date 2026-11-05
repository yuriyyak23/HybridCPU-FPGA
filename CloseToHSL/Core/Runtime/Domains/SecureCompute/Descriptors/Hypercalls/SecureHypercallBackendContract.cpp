#include "SecureHypercallBackendContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureHypercallBackendContractDescriptor::SecureHypercallBackendContractDescriptor(std::shared_ptr<SecureHypercallDecodedLeaf> decodedLeaf, std::shared_ptr<SecureComputeServiceId> serviceId, std::shared_ptr<SecureBackendOwnerId> ownerId, std::shared_ptr<SecureRevocationEpoch> ownerEpoch, std::shared_ptr<SecureHypercallContractVersion> version, std::shared_ptr<SecureGrantHandle> requiredGrant, SecureHypercallReplayPolicy replayPolicy, SecureHypercallCancellationPolicy cancellationPolicy, SecureHypercallRequestMigrationClass requestMigrationClass, SecureHypercallResultMigrationClass resultMigrationClass)
    : DecodedLeaf(std::move(decodedLeaf)), ServiceId(std::move(serviceId)), OwnerId(std::move(ownerId)),
      OwnerEpoch(ownerEpoch ? std::move(ownerEpoch) : SecureRevocationEpoch::Unmaterialized),
      Version(std::move(version)), RequiredGrant(std::move(requiredGrant)), ReplayPolicy(replayPolicy),
      CancellationPolicy(cancellationPolicy), RequestMigrationClass(requestMigrationClass), ResultMigrationClass(resultMigrationClass)
{
}

	std::shared_ptr<SecureHypercallBackendContractDescriptor> SecureHypercallBackendContractDescriptor::getUnresolved()
	{
		return Unresolved;
	}

	std::shared_ptr<SecureHypercallDecodedLeaf> SecureHypercallBackendContractDescriptor::getDecodedLeaf() const
	{
		return DecodedLeaf;
	}

	std::shared_ptr<SecureComputeServiceId> SecureHypercallBackendContractDescriptor::getServiceId() const
	{
		return ServiceId;
	}

	std::shared_ptr<SecureBackendOwnerId> SecureHypercallBackendContractDescriptor::getOwnerId() const
	{
		return OwnerId;
	}

	std::shared_ptr<SecureRevocationEpoch> SecureHypercallBackendContractDescriptor::getOwnerEpoch() const
	{
		return OwnerEpoch;
	}

	std::shared_ptr<SecureHypercallContractVersion> SecureHypercallBackendContractDescriptor::getVersion() const
	{
		return Version;
	}

	std::shared_ptr<SecureGrantHandle> SecureHypercallBackendContractDescriptor::getRequiredGrant() const
	{
		return RequiredGrant;
	}

	SecureHypercallReplayPolicy SecureHypercallBackendContractDescriptor::getReplayPolicy() const
	{
		return ReplayPolicy;
	}

	SecureHypercallCancellationPolicy SecureHypercallBackendContractDescriptor::getCancellationPolicy() const
	{
		return CancellationPolicy;
	}

	SecureHypercallRequestMigrationClass SecureHypercallBackendContractDescriptor::getRequestMigrationClass() const
	{
		return RequestMigrationClass;
	}

	SecureHypercallResultMigrationClass SecureHypercallBackendContractDescriptor::getResultMigrationClass() const
	{
		return ResultMigrationClass;
	}

	bool SecureHypercallBackendContractDescriptor::isMaterialized() const
	{
		return getDecodedLeaf()->IsMaterialized && getServiceId()->IsMaterialized && getOwnerId()->IsMaterialized && getOwnerEpoch()->isMaterialized() && getVersion()->IsMaterialized && getRequiredGrant()->isMaterialized();
	}
}
