#include "SecureHypercallBackendOwnerAbiRegistry.h"
#include "../../Authority/SecureGrantHandle.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureHypercallTransportOpcode> SecureHypercallBackendOwnerAbiRegistry::getTransportOpcode()
	{
		return std::make_shared<SecureHypercallTransportOpcode>(TransportOpcodeValue);
	}

	std::shared_ptr<SecureHypercallDecodedLeaf> SecureHypercallBackendOwnerAbiRegistry::getDecodedLeaf()
	{
		return std::make_shared<SecureHypercallDecodedLeaf>(DecodedLeafValue);
	}

	std::shared_ptr<SecureComputeServiceId> SecureHypercallBackendOwnerAbiRegistry::getServiceId()
	{
		return std::make_shared<SecureComputeServiceId>(ServiceIdValue);
	}

	std::shared_ptr<SecureBackendOwnerId> SecureHypercallBackendOwnerAbiRegistry::getOwnerId()
	{
		return std::make_shared<SecureBackendOwnerId>(OwnerIdValue);
	}

	std::shared_ptr<SecureRevocationEpoch> SecureHypercallBackendOwnerAbiRegistry::getOwnerEpoch()
	{
		return std::make_shared<SecureRevocationEpoch>(OwnerEpochValue);
	}

	std::shared_ptr<SecureHypercallContractVersion> SecureHypercallBackendOwnerAbiRegistry::getContractVersion()
	{
		return std::make_shared<SecureHypercallContractVersion>(ContractVersionMajor, ContractVersionMinor);
	}

	std::shared_ptr<SecureGrantHandle> SecureHypercallBackendOwnerAbiRegistry::getRequiredGrant()
	{
		return std::make_shared<SecureGrantHandle>(SecureGrantHandle{ SecureGrantHandleKind::HypercallPolicy, RequiredGrantLocalIdValue, RequiredGrantProvenanceHashValue, OwnerEpochValue });
	}

	std::shared_ptr<SecureHypercallBackendContractDescriptor> SecureHypercallBackendOwnerAbiRegistry::getProductionContract()
	{
		return std::make_shared<SecureHypercallBackendContractDescriptor>(getDecodedLeaf(), getServiceId(), getOwnerId(), getOwnerEpoch(), getContractVersion(), getRequiredGrant(), SecureHypercallReplayPolicy::DenyReplay, SecureHypercallCancellationPolicy::DenyBeforeExecution, SecureHypercallRequestMigrationClass::NonMigratableInFlight, SecureHypercallResultMigrationClass::NoResultBeforeExecution);
	}

	std::shared_ptr<SecureBackendOwnerDescriptor> SecureHypercallBackendOwnerAbiRegistry::CreateOwnerDescriptor(SecureBackendOwnerSource source, std::optional<std::shared_ptr<SecureRevocationEpoch>> epoch, bool grantProofValidated, bool evidenceProofValidated, bool completionFenceValidated, bool retireFenceValidated, bool negativeTestsPresent)
	{
		std::shared_ptr<SecureRevocationEpoch> ownerEpoch = epoch && epoch.value() ? epoch.value() : getOwnerEpoch(); return std::make_shared<SecureBackendOwnerDescriptor>(OwnerIdValue, source, OwnerPolicyDigestValue, OwnerProofDigestValue, ownerEpoch, true, grantProofValidated, evidenceProofValidated, completionFenceValidated, retireFenceValidated, negativeTestsPresent);
	}
}
