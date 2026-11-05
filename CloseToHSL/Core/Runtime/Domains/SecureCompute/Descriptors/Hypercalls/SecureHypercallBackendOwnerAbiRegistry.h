#pragma once

#include "../../../../../Virtualization/Conformance/AbiFreeze/YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.h"
#include "SecureHypercallBackendContract.h"
#include "../Backend/SecureBackendOwnerDescriptor.h"
#include <string>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class SecureHypercallBackendOwnerAbiRegistry final
	{
	public:
		static inline const std::wstring RegistrySource = L"ADR-SC-HYP-BACKEND-OWNER";

		static inline const std::wstring AllocationRevision = L"ADR-SC-HYP-BACKEND-OWNER-2026-06-17";

		static constexpr unsigned short TransportOpcodeValue = CompatAbiFreezeContract::FrozenCallOpcode;

		static constexpr unsigned long long DecodedLeafValue = 0x5343'4859'5042ULL;

		static constexpr unsigned long long ServiceIdValue = 0x5343'5356'4345ULL;

		static constexpr unsigned long long OwnerIdValue = 0x5343'4F57'4E52ULL;

		static constexpr unsigned long long OwnerEpochValue = 13ULL;

		static constexpr unsigned short ContractVersionMajor = 1;

		static constexpr unsigned short ContractVersionMinor = 0;

		static constexpr unsigned long long RequiredGrantLocalIdValue = 0x5343'4752'4E54ULL;

		static constexpr unsigned long long RequiredGrantProvenanceHashValue = 0x5343'4144'5250ULL;

		static constexpr unsigned long long OwnerPolicyDigestValue = 0x5343'504F'4C59ULL;

		static constexpr unsigned long long OwnerProofDigestValue = 0x5343'5052'4F46ULL;

		static std::shared_ptr<SecureHypercallTransportOpcode> getTransportOpcode();

		static std::shared_ptr<SecureHypercallDecodedLeaf> getDecodedLeaf();

		static std::shared_ptr<SecureComputeServiceId> getServiceId();

		static std::shared_ptr<SecureBackendOwnerId> getOwnerId();

		static std::shared_ptr<SecureRevocationEpoch> getOwnerEpoch();

		static std::shared_ptr<SecureHypercallContractVersion> getContractVersion();

		static std::shared_ptr<SecureGrantHandle> getRequiredGrant();

		static std::shared_ptr<SecureHypercallBackendContractDescriptor> getProductionContract();

		static std::shared_ptr<SecureBackendOwnerDescriptor> CreateOwnerDescriptor(SecureBackendOwnerSource source = SecureBackendOwnerSource::NeutralRuntimeService, std::optional<std::shared_ptr<SecureRevocationEpoch>> epoch = std::nullopt, bool grantProofValidated = true, bool evidenceProofValidated = true, bool completionFenceValidated = true, bool retireFenceValidated = true, bool negativeTestsPresent = true);
	};

}
