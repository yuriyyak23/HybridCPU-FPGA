#pragma once

#include "../../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include "../../../Capabilities/Grants/CapabilityGrant.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

    class VirtualizationOperationOwnerSnapshot;
    struct VirtualizationDecisionSpecV2;
    struct VirtualizationDecisionOwnerMapEntryV2;
    struct VirtualizationDecisionAcceptanceRecordV2;
    struct AcceptedVirtualizationDecision;
    struct HypercallRuntimeOwnerAllocation;
    struct VirtualizationOperationOwnerSnapshotLoadResult;

    enum class VirtualizationOperationOwnerSnapshotLoadDecision : unsigned char
	{
		Loaded = 0,
		MissingInput = 1,
		DecisionMismatch = 2,
		DigestMismatch = 3,
		AcceptanceNotCurrent = 4,
		OwnerMismatch = 5,
		PolicyVersionMismatch = 6,
		NamespaceOrLeafMismatch = 7,
		AbiMismatch = 8,
		PolicyMismatch = 9,
		AllocationMismatch = 10,
		NotExactMachineAcceptedSource = 11
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct VirtualizationOperationOwnerSnapshotLoadResult(VirtualizationOperationOwnerSnapshotLoadDecision Decision, VirtualizationOperationOwnerSnapshot? Snapshot, string Reason)
//	internal readonly record struct VirtualizationOperationOwnerSnapshotLoadResult(VirtualizationOperationOwnerSnapshotLoadDecision Decision, VirtualizationOperationOwnerSnapshot Snapshot, string Reason)
//	{
//		internal bool IsLoaded
//		{
//			get
//			{
//				return Decision == VirtualizationOperationOwnerSnapshotLoadDecision.Loaded && Snapshot != nullptr;
//			}
//		}
//	}

	/// <summary>
	/// O1 is an immutable runtime-loaded copy of one accepted D2 policy. It is not
	/// a capability, live admission, execution token, completion or retire grant.
	/// </summary>
	struct VirtualizationOperationOwnerSnapshotLoadResult final
	{
		const VirtualizationOperationOwnerSnapshotLoadDecision Decision;
		const std::shared_ptr<VirtualizationOperationOwnerSnapshot> Snapshot;
		const std::wstring Reason;

		VirtualizationOperationOwnerSnapshotLoadResult(
			VirtualizationOperationOwnerSnapshotLoadDecision decision,
			std::shared_ptr<VirtualizationOperationOwnerSnapshot> snapshot,
			std::wstring reason)
			: Decision(decision), Snapshot(std::move(snapshot)), Reason(std::move(reason)) {}

		bool getIsLoaded() const noexcept
		{
			return Decision == VirtualizationOperationOwnerSnapshotLoadDecision::Loaded && Snapshot != nullptr;
		}
	};
	class VirtualizationOperationOwnerSnapshot final : public std::enable_shared_from_this<VirtualizationOperationOwnerSnapshot>
	{
	private:
		const std::wstring AcceptanceDigest;
		const std::wstring PolicyDigest;

		const std::shared_ptr<VirtualizationDecisionSpecV2> _policy;

		VirtualizationOperationOwnerSnapshot(std::shared_ptr<VirtualizationDecisionSpecV2> policy, const std::wstring &acceptanceDigest, const std::wstring &policyDigest);

	public:
		unsigned int getSchemaVersion() const;
		std::wstring getDecisionId() const;
		std::wstring getSpecDigest() const;
		std::wstring getAcceptanceDigest() const;
		unsigned long long getOwnerId() const;
		unsigned int getOwnerPolicyVersion() const;
		unsigned int getOwnerEpoch() const;
		std::wstring getOperationNamespace() const;
		std::wstring getOperationId() const;
		unsigned short getLeafWidth() const;
		unsigned short getNumericLeaf() const;
		unsigned int getOperandAbiVersion() const;
		std::wstring getRs1Contract() const;
		std::wstring getRs2Contract() const;
		std::wstring getRdContract() const;
		VirtualizationDecisionResultAbiV2 getResultAbi() const;
		VirtualizationDecisionEffectClassV2 getEffectClass() const;
		VirtualizationCapabilityRequirementV2 getCapabilityRequirement() const;
		unsigned long long getCapabilityMask() const;
		bool getRequiresTypedGrant() const;
		VirtualizationDelegationPolicyV2 getDelegationPolicy() const;
		VirtualizationRevocationPolicyV2 getRevocationPolicy() const;
		VirtualizationCapabilityMigrationClassV2 getCapabilityMigrationClass() const;
		VirtualizationEvidenceVisibilityV2 getEvidenceVisibility() const;
		VirtualizationProjectionPolicyV2 getFrontendProjectionPolicy() const;
		VirtualizationExecutionEvidenceRequirementV2 getExecutionEvidenceRequirement() const;
		VirtualizationDomainRequirementV2 getDomainRequirement() const;
		bool getRequireNonZeroDomainTag() const;
		bool getRequiresMemoryDomain() const;
		bool getRequiresIoDomain() const;
		VirtualizationAddressSpaceRequirementV2 getAddressSpaceRequirement() const;
		VirtualizationSecureDomainPolicyV2 getSecureDomainPolicy() const;
		VirtualizationCancellationPolicyV2 getCancellationPolicy() const;
		VirtualizationReplayPolicyV2 getReplayPolicy() const;
		VirtualizationOperationMigrationPolicyV2 getOperationMigrationPolicy() const;
		VirtualizationCompletionEvidenceClassV2 getCompletionEvidenceClass() const;
		VirtualizationCompletionMigrationClassV2 getCompletionMigrationClass() const;
		VirtualizationProjectionPolicyV2 getCompletionProjectionPolicy() const;
		VirtualizationCompletionPolicyV2 getCompletionPolicy() const;
		VirtualizationRetirePolicyV2 getRetirePolicy() const;
		VirtualizationAdjacentLeafPolicyV2 getAdjacentLeafPolicy() const;
		VirtualizationCrossNamespacePolicyV2 getCrossNamespacePolicy() const;
		std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> getOwnerMap() const;
		std::wstring getPolicyDigest() const;

		bool isCapability() const;
		bool getRuntimeAuthorityGranted() const;
		bool getBackendExecutionAuthorized() const;
		bool getCompletionPublicationAuthorized() const;
		bool getRetirePublicationAuthorized() const;

		static std::shared_ptr<VirtualizationOperationOwnerSnapshot> CreateValidated(std::shared_ptr<VirtualizationDecisionSpecV2> policy, const std::wstring &acceptanceDigest);
	};

	class VirtualizationOperationOwnerSnapshotDigest final
	{
	private:
		static inline const std::vector<unsigned char> Envelope = {'H', 'C', 'P', 'U', 'O', '1', 0};

	public:
		static std::wstring Compute(const std::wstring &specDigest, const std::wstring &acceptanceDigest, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch);
	};

	class VirtualizationOperationOwnerSnapshotLoader final
	{
	public:
		static std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> LoadExactAcceptedPolicy();

		static std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> TryLoad(const std::shared_ptr<AcceptedVirtualizationDecision> &accepted, const std::shared_ptr<VirtualizationDecisionSpecV2> &spec, const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> &acceptance, const std::shared_ptr<HypercallRuntimeOwnerAllocation> &allocation);

	private:
		static std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> Deny(VirtualizationOperationOwnerSnapshotLoadDecision decision, const std::wstring &reason);
	};

	class Phase38VirtualizationOperationOwnerSnapshotRegistry final
	{
	private:
		static std::shared_ptr<VirtualizationOperationOwnerSnapshotLoadResult> GetLoadResult();

	public:
		static std::shared_ptr<VirtualizationOperationOwnerSnapshot> getExactSnapshot();

		static bool TryResolve(const std::wstring &operationNamespace, unsigned short numericLeaf, std::shared_ptr<VirtualizationOperationOwnerSnapshot> &snapshot);

		static bool getRuntimeAuthorityGranted();
		static bool getBackendExecutionAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getRetirePublicationAuthorized();
	};

}

