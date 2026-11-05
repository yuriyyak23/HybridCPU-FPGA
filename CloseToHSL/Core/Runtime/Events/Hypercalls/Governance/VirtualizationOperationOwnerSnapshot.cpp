#include "VirtualizationOperationOwnerSnapshot.h"
#include "HypercallRuntimeOwnerRegistry.h"
#include "Phase38AcceptedVirtualizationDecisionRegistry.g.h"
#include "Phase38VirtualizationDecisionAcceptanceV2.h"
#include "Phase38VirtualizationDecisionSpecV2.h"
#include "../../../Capabilities/Governance/YAKSys_Hybrid_CPU.Core.RuntimeCapabilityIds.h"
#include "../../../../Decoder/Sha256Digest.h"
#include "../../../../../../tangible_exception_helper.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
	VirtualizationOperationOwnerSnapshot::VirtualizationOperationOwnerSnapshot(std::shared_ptr<VirtualizationDecisionSpecV2> policy, const std::wstring &acceptanceDigest, const std::wstring &policyDigest)
		: AcceptanceDigest(acceptanceDigest), PolicyDigest(policyDigest),
		  _policy(policy ? std::make_shared<VirtualizationDecisionSpecV2>(*policy) : nullptr)
	{
	}

	unsigned int VirtualizationOperationOwnerSnapshot::getSchemaVersion() const
	{
		return _policy->SchemaVersion;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getDecisionId() const
	{
		return _policy->DecisionId;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getSpecDigest() const
	{
		return _policy->SpecDigest;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getAcceptanceDigest() const
	{
		return AcceptanceDigest;
	}

	unsigned long long VirtualizationOperationOwnerSnapshot::getOwnerId() const
	{
		return _policy->OwnerId;
	}

	unsigned int VirtualizationOperationOwnerSnapshot::getOwnerPolicyVersion() const
	{
		return _policy->OwnerPolicyVersion;
	}

	unsigned int VirtualizationOperationOwnerSnapshot::getOwnerEpoch() const
	{
		return _policy->OwnerEpoch;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getOperationNamespace() const
	{
		return _policy->OperationNamespace;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getOperationId() const
	{
		return _policy->OperationId;
	}

	unsigned short VirtualizationOperationOwnerSnapshot::getLeafWidth() const
	{
		return _policy->LeafWidth;
	}

	unsigned short VirtualizationOperationOwnerSnapshot::getNumericLeaf() const
	{
		return _policy->NumericLeaf;
	}

	unsigned int VirtualizationOperationOwnerSnapshot::getOperandAbiVersion() const
	{
		return _policy->OperandAbiVersion;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getRs1Contract() const
	{
		return _policy->Rs1Contract;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getRs2Contract() const
	{
		return _policy->Rs2Contract;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getRdContract() const
	{
		return _policy->RdContract;
	}

	VirtualizationDecisionResultAbiV2 VirtualizationOperationOwnerSnapshot::getResultAbi() const
	{
		return _policy->ResultAbi;
	}

	VirtualizationDecisionEffectClassV2 VirtualizationOperationOwnerSnapshot::getEffectClass() const
	{
		return _policy->EffectClass;
	}

	VirtualizationCapabilityRequirementV2 VirtualizationOperationOwnerSnapshot::getCapabilityRequirement() const
	{
		return _policy->CapabilityRequirement;
	}

	unsigned long long VirtualizationOperationOwnerSnapshot::getCapabilityMask() const
	{
		return _policy->CapabilityMask;
	}

	bool VirtualizationOperationOwnerSnapshot::getRequiresTypedGrant() const
	{
		return _policy->RequiresTypedGrant;
	}

	VirtualizationDelegationPolicyV2 VirtualizationOperationOwnerSnapshot::getDelegationPolicy() const
	{
		return _policy->DelegationPolicy;
	}

	VirtualizationRevocationPolicyV2 VirtualizationOperationOwnerSnapshot::getRevocationPolicy() const
	{
		return _policy->RevocationPolicy;
	}

	VirtualizationCapabilityMigrationClassV2 VirtualizationOperationOwnerSnapshot::getCapabilityMigrationClass() const
	{
		return _policy->CapabilityMigrationClass;
	}

	VirtualizationEvidenceVisibilityV2 VirtualizationOperationOwnerSnapshot::getEvidenceVisibility() const
	{
		return _policy->EvidenceVisibility;
	}

	VirtualizationProjectionPolicyV2 VirtualizationOperationOwnerSnapshot::getFrontendProjectionPolicy() const
	{
		return _policy->FrontendProjectionPolicy;
	}

	VirtualizationExecutionEvidenceRequirementV2 VirtualizationOperationOwnerSnapshot::getExecutionEvidenceRequirement() const
	{
		return _policy->ExecutionEvidenceRequirement;
	}

	VirtualizationDomainRequirementV2 VirtualizationOperationOwnerSnapshot::getDomainRequirement() const
	{
		return _policy->DomainRequirement;
	}

	bool VirtualizationOperationOwnerSnapshot::getRequireNonZeroDomainTag() const
	{
		return _policy->RequireNonZeroDomainTag;
	}

	bool VirtualizationOperationOwnerSnapshot::getRequiresMemoryDomain() const
	{
		return _policy->RequiresMemoryDomain;
	}

	bool VirtualizationOperationOwnerSnapshot::getRequiresIoDomain() const
	{
		return _policy->RequiresIoDomain;
	}

	VirtualizationAddressSpaceRequirementV2 VirtualizationOperationOwnerSnapshot::getAddressSpaceRequirement() const
	{
		return _policy->AddressSpaceRequirement;
	}

	VirtualizationSecureDomainPolicyV2 VirtualizationOperationOwnerSnapshot::getSecureDomainPolicy() const
	{
		return _policy->SecureDomainPolicy;
	}

	VirtualizationCancellationPolicyV2 VirtualizationOperationOwnerSnapshot::getCancellationPolicy() const
	{
		return _policy->CancellationPolicy;
	}

	VirtualizationReplayPolicyV2 VirtualizationOperationOwnerSnapshot::getReplayPolicy() const
	{
		return _policy->ReplayPolicy;
	}

	VirtualizationOperationMigrationPolicyV2 VirtualizationOperationOwnerSnapshot::getOperationMigrationPolicy() const
	{
		return _policy->OperationMigrationPolicy;
	}

	VirtualizationCompletionEvidenceClassV2 VirtualizationOperationOwnerSnapshot::getCompletionEvidenceClass() const
	{
		return _policy->CompletionEvidenceClass;
	}

	VirtualizationCompletionMigrationClassV2 VirtualizationOperationOwnerSnapshot::getCompletionMigrationClass() const
	{
		return _policy->CompletionMigrationClass;
	}

	VirtualizationProjectionPolicyV2 VirtualizationOperationOwnerSnapshot::getCompletionProjectionPolicy() const
	{
		return _policy->CompletionProjectionPolicy;
	}

	VirtualizationCompletionPolicyV2 VirtualizationOperationOwnerSnapshot::getCompletionPolicy() const
	{
		return _policy->CompletionPolicy;
	}

	VirtualizationRetirePolicyV2 VirtualizationOperationOwnerSnapshot::getRetirePolicy() const
	{
		return _policy->RetirePolicy;
	}

	VirtualizationAdjacentLeafPolicyV2 VirtualizationOperationOwnerSnapshot::getAdjacentLeafPolicy() const
	{
		return _policy->AdjacentLeafPolicy;
	}

	VirtualizationCrossNamespacePolicyV2 VirtualizationOperationOwnerSnapshot::getCrossNamespacePolicy() const
	{
		return _policy->CrossNamespacePolicy;
	}

	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> VirtualizationOperationOwnerSnapshot::getOwnerMap() const
	{
		return _policy->OwnerMap;
	}

	std::wstring VirtualizationOperationOwnerSnapshot::getPolicyDigest() const
	{
		return PolicyDigest;
	}

	bool VirtualizationOperationOwnerSnapshot::isCapability() const
	{
		return false;
	}

	bool VirtualizationOperationOwnerSnapshot::getRuntimeAuthorityGranted() const
	{
		return false;
	}

	bool VirtualizationOperationOwnerSnapshot::getBackendExecutionAuthorized() const
	{
		return false;
	}

	bool VirtualizationOperationOwnerSnapshot::getCompletionPublicationAuthorized() const
	{
		return false;
	}

	bool VirtualizationOperationOwnerSnapshot::getRetirePublicationAuthorized() const
	{
		return false;
	}

	std::shared_ptr<VirtualizationOperationOwnerSnapshot> VirtualizationOperationOwnerSnapshot::CreateValidated(std::shared_ptr<VirtualizationDecisionSpecV2> policy, const std::wstring &acceptanceDigest)
	{
		return std::shared_ptr<VirtualizationOperationOwnerSnapshot>(new VirtualizationOperationOwnerSnapshot(policy, acceptanceDigest, VirtualizationOperationOwnerSnapshotDigest::Compute(policy->SpecDigest, acceptanceDigest, policy->OwnerId, policy->OwnerPolicyVersion, policy->OwnerEpoch)));
	}

	std::wstring VirtualizationOperationOwnerSnapshotDigest::Compute(const std::wstring &specDigest, const std::wstring &acceptanceDigest, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch)
	{
		auto decodeHex = [](const std::wstring &value)
		{
			if (value.size() != 64) throw std::invalid_argument("SHA-256 digest must contain 64 hexadecimal characters.");
			auto nibble = [](wchar_t c) -> unsigned char
			{
				if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
				if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
				if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
				throw std::invalid_argument("SHA-256 digest contains a non-hexadecimal character.");
			};
			std::vector<unsigned char> decoded;
			decoded.reserve(32);
			for (std::size_t i = 0; i < value.size(); i += 2)
				decoded.push_back(static_cast<unsigned char>((nibble(value[i]) << 4) | nibble(value[i + 1])));
			return decoded;
		};

		std::vector<unsigned char> bytes(Envelope);
		const auto specBytes = decodeHex(specDigest);
		const auto acceptanceBytes = decodeHex(acceptanceDigest);
		bytes.insert(bytes.end(), specBytes.begin(), specBytes.end());
		bytes.insert(bytes.end(), acceptanceBytes.begin(), acceptanceBytes.end());
		for (int shift = 56; shift >= 0; shift -= 8)
			bytes.push_back(static_cast<unsigned char>((ownerId >> shift) & 0xff));
		for (int shift = 24; shift >= 0; shift -= 8)
			bytes.push_back(static_cast<unsigned char>((ownerPolicyVersion >> shift) & 0xff));
		for (int shift = 24; shift >= 0; shift -= 8)
			bytes.push_back(static_cast<unsigned char>((ownerEpoch >> shift) & 0xff));

		return Decoder::Sha256Hex(std::span<const unsigned char>(bytes));
	}

	std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> VirtualizationOperationOwnerSnapshotLoader::LoadExactAcceptedPolicy()
	{
		const auto entry = Phase38AcceptedVirtualizationDecisionRegistry::getExactEntry();
		if (!entry) return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::MissingInput, L"O1 requires the exact accepted decision registry entry.");
		return TryLoad(entry->Policy, Phase38VirtualizationDecisionSpecV2::getInstance(), Phase38VirtualizationDecisionAcceptanceV2::getRecord(), HypercallRuntimeOwnerRegistry::getPhase38ProbeOwner());
	}

	std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> VirtualizationOperationOwnerSnapshotLoader::TryLoad(const std::shared_ptr<AcceptedVirtualizationDecision> &accepted, const std::shared_ptr<VirtualizationDecisionSpecV2> &spec, const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> &acceptance, const std::shared_ptr<HypercallRuntimeOwnerAllocation> &allocation)
	{
		if (!accepted || !spec || !acceptance || !allocation)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::MissingInput, L"O1 requires D2 policy, spec, acceptance and owner allocation inputs.");

		if (accepted->DecisionId != spec->DecisionId || acceptance->DecisionId != spec->DecisionId)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::DecisionMismatch, L"O1 decision identity does not match D2.");

		if (accepted->SpecDigest != spec->SpecDigest || acceptance->SpecDigest != spec->SpecDigest || accepted->AcceptanceDigest != acceptance->AcceptanceDigest)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::DigestMismatch, L"O1 digest lineage does not match D2.");

		if (acceptance->AcceptanceState != VirtualizationDecisionAcceptanceStateV2::Accepted || acceptance->SupersedesDecisionId != nullptr || acceptance->SupersedesAcceptanceDigest != nullptr)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::AcceptanceNotCurrent, L"O1 requires one current non-superseding accepted record.");

		if (accepted->OwnerId == 0 || accepted->OwnerId != spec->OwnerId || allocation->OwnerId != spec->OwnerId)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::OwnerMismatch, L"O1 owner identity is missing or mismatched.");

		if (accepted->OwnerPolicyVersion != spec->OwnerPolicyVersion || accepted->OwnerEpoch != spec->OwnerEpoch || allocation->OwnerPolicyVersion != spec->OwnerPolicyVersion || allocation->OwnerEpoch != spec->OwnerEpoch)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::PolicyVersionMismatch, L"O1 owner policy version or epoch is stale.");

		if (accepted->OperationNamespace != spec->OperationNamespace || accepted->NumericLeaf != spec->NumericLeaf || spec->LeafWidth != 16 || spec->NumericLeaf == 0)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::NamespaceOrLeafMismatch, L"O1 namespace or exact leaf does not match D2.");

		if (spec->OperandAbiVersion != 1 || spec->Rs1Contract != L"ArchitecturalRegisterFullNumericLeafValue" || spec->Rs2Contract != L"X0" || spec->RdContract != L"X0NoResult" || spec->ResultAbi != VirtualizationDecisionResultAbiV2::NoPayload || spec->EffectClass != VirtualizationDecisionEffectClassV2::NoStateNoPayload)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::AbiMismatch, L"O1 ABI or effect policy does not match the exact first slice.");

		if (spec->CapabilityMask != RuntimeCapabilityIds::VmCallProbeNoStateV1Mask || !spec->RequiresTypedGrant || spec->DomainRequirement != VirtualizationDomainRequirementV2::ExecutionDomainBound || !spec->RequireNonZeroDomainTag || spec->RequiresMemoryDomain || spec->RequiresIoDomain || spec->SecureDomainPolicy != VirtualizationSecureDomainPolicyV2::Deny || spec->FrontendProjectionPolicy != VirtualizationProjectionPolicyV2::NeverProject || spec->AdjacentLeafPolicy != VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExact)
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::PolicyMismatch, L"O1 runtime policy does not match the exact accepted profile.");

		if (allocation->DecisionId != spec->DecisionId || allocation->SpecDigest != spec->SpecDigest || allocation->OwnerRole != L"DomainHypercallRuntimeOwner")
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::AllocationMismatch, L"O1 owner allocation is not the accepted HCOWNR allocation.");

		const auto exactEntry = Phase38AcceptedVirtualizationDecisionRegistry::getExactEntry();
		if (!exactEntry || accepted != exactEntry->Policy || spec != Phase38VirtualizationDecisionSpecV2::getInstance() || acceptance != Phase38VirtualizationDecisionAcceptanceV2::getRecord() || allocation != HypercallRuntimeOwnerRegistry::getPhase38ProbeOwner())
			return Deny(VirtualizationOperationOwnerSnapshotLoadDecision::NotExactMachineAcceptedSource, L"O1 cannot be loaded from a cloned, caller-built or unregistered policy object.");

		const auto snapshot = VirtualizationOperationOwnerSnapshot::CreateValidated(spec, acceptance->AcceptanceDigest);
		return std::make_shared<VirtualizationOperationOwnerSnapshotLoadResult>(VirtualizationOperationOwnerSnapshotLoadDecision::Loaded, snapshot, L"Exact accepted D2 policy loaded as immutable O1.");
	}

	std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> VirtualizationOperationOwnerSnapshotLoader::Deny(VirtualizationOperationOwnerSnapshotLoadDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VirtualizationOperationOwnerSnapshotLoadResult>(decision, nullptr, reason);
	}

	std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> Phase38VirtualizationOperationOwnerSnapshotRegistry::GetLoadResult()
	{
		static const auto result = VirtualizationOperationOwnerSnapshotLoader::LoadExactAcceptedPolicy();
		return result;
	}

	std::shared_ptr<VirtualizationOperationOwnerSnapshot> Phase38VirtualizationOperationOwnerSnapshotRegistry::getExactSnapshot()
	{
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: return LoadResult.Snapshot ?? throw new InvalidOperationException(LoadResult.Reason);
		const auto result = GetLoadResult();
		if (!result->Snapshot) throw InvalidOperationException(result->Reason);
		return result->Snapshot;
	}

	bool Phase38VirtualizationOperationOwnerSnapshotRegistry::TryResolve(const std::wstring &operationNamespace, unsigned short numericLeaf, std::shared_ptr<VirtualizationOperationOwnerSnapshot> &snapshot)
	{
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (GetLoadResult()->getIsLoaded() && operationNamespace == getExactSnapshot()->getOperationNamespace() && numericLeaf == getExactSnapshot()->getNumericLeaf())
		{
			snapshot = getExactSnapshot();
			return true;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: snapshot = null!;
		snapshot = nullptr;
		return false;
	}

	bool Phase38VirtualizationOperationOwnerSnapshotRegistry::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase38VirtualizationOperationOwnerSnapshotRegistry::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase38VirtualizationOperationOwnerSnapshotRegistry::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase38VirtualizationOperationOwnerSnapshotRegistry::getRetirePublicationAuthorized()
	{
		return false;
	}
}
