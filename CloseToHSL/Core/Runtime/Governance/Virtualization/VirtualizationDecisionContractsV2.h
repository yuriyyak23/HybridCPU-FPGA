#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationDecisionOwnerClassV2 : unsigned char
	{
		Unspecified = 0,
		NeutralRuntimeOwner = 1,
		CompatibilityFrontend = 2
	};

	enum class VirtualizationDecisionResultAbiV2 : unsigned char
	{
		Unspecified = 0,
		NoPayload = 1,
		ArchitecturalScalar64 = 2,
		ScalarU64ToDestinationRegister = 3
	};

	enum class VirtualizationDecisionEffectClassV2 : unsigned char
	{
		Unspecified = 0,
		NoStateNoPayload = 1,
		ReadOnlyProjectionNoStateMutation = 2,
		ArchitecturalRegisterResultOnly = 3
	};

	enum class VirtualizationCapabilityRequirementV2 : unsigned char
	{
		Unspecified = 0,
		DomainGrantedVmCallProbeNoStateV1 = 1,
		None = 2
	};

	enum class VirtualizationDelegationPolicyV2 : unsigned char
	{
		Unspecified = 0,
		NonDelegable = 1
	};

	enum class VirtualizationRevocationPolicyV2 : unsigned char
	{
		Unspecified = 0,
		RuntimeRevocable = 1,
		GovernanceRevocable = 2
	};

	enum class VirtualizationCapabilityMigrationClassV2 : unsigned char
	{
		Unspecified = 0,
		DomainLocal = 1,
		None = 2
	};

	enum class VirtualizationEvidenceVisibilityV2 : unsigned char
	{
		Unspecified = 0,
		HostOnly = 1,
		GuestVisibleReadOnly = 2
	};

	enum class VirtualizationProjectionPolicyV2 : unsigned char
	{
		Unspecified = 0,
		NeverProject = 1,
		ExactReadOnlyFieldSet = 2
	};

	enum class VirtualizationExecutionEvidenceRequirementV2 : unsigned char
	{
		Unspecified = 0,
		None = 1,
		FieldConformanceProof = 2
	};

	enum class VirtualizationDomainRequirementV2 : unsigned char
	{
		Unspecified = 0,
		ExecutionDomainBound = 1,
		ExecutionDomainAndAddressSpaceBound = 2
	};

	enum class VirtualizationAddressSpaceRequirementV2 : unsigned char
	{
		Unspecified = 0,
		None = 1,
		ExactNonZeroAddressSpaceTag = 2
	};

	enum class VirtualizationSecureDomainPolicyV2 : unsigned char
	{
		Unspecified = 0,
		Deny = 1
	};

	enum class VirtualizationCancellationPolicyV2 : unsigned char
	{
		Unspecified = 0,
		DenyBeforeExecution = 1,
		NotApplicableReadOnlyProjection = 2,
		SquashBeforeRetireZeroArchitecturalEffect = 3
	};

	enum class VirtualizationReplayPolicyV2 : unsigned char
	{
		Unspecified = 0,
		DenyAttemptReplay = 1,
		NotApplicableReadOnlyProjection = 2,
		AttemptBoundReceiptNoReplayReuse = 3
	};

	enum class VirtualizationOperationMigrationPolicyV2 : unsigned char
	{
		Unspecified = 0,
		DrainOnly = 1,
		RevalidatedAfterRestore = 2
	};

	enum class VirtualizationCompletionEvidenceClassV2 : unsigned char
	{
		Unspecified = 0,
		HostOwnedRuntimeEvidence = 1,
		None = 2
	};

	enum class VirtualizationCompletionMigrationClassV2 : unsigned char
	{
		Unspecified = 0,
		HostOwnedNonMigratable = 1,
		None = 2
	};

	enum class VirtualizationCompletionPolicyV2 : unsigned char
	{
		Unspecified = 0,
		AtomicE3ToCompletionRecordAndE5 = 1,
		None = 2
	};

	enum class VirtualizationRetirePolicyV2 : unsigned char
	{
		Unspecified = 0,
		PreciseE5BoundNoStateRetire = 1,
		None = 2,
		CanonicalRetireCoordinatorArchitecturalRegisterCommit = 3
	};

	enum class VirtualizationAdjacentLeafPolicyV2 : unsigned char
	{
		Unspecified = 0,
		DenyAllExceptExact = 1,
		DenyAllExceptExactFieldSet = 2
	};

	enum class VirtualizationDecisionOperationClassV2 : unsigned char
	{
		Unspecified = 0,
		ReadOnlyArchitecturalVmReadCompatibilityProjection = 1,
		ReadOnlyArchitecturalVmReadScalarDelivery = 2
	};

	enum class VirtualizationDecisionAuthorityPlaneV2 : unsigned char
	{
		Unspecified = 0,
		PrivilegedExecutionStateReadProjection = 1,
		PrivilegedExecutionStateSourceCanonicalRegisterDelivery = 2,
		ExecutionDomainReadOnlyStateCanonicalRegisterDelivery = 3,
		MemoryAddressSpaceReadProjection = 4,
		CompletionObservationReadProjection = 5
	};

	enum class VirtualizationDecisionMutationClassV2 : unsigned char
	{
		Unspecified = 0,
		ReadOnly = 1,
		UnderlyingVirtualizationStateReadOnly = 2
	};

	enum class VirtualizationCrossNamespacePolicyV2 : unsigned char
	{
		Unspecified = 0,
		AllowDistinctFrozenCompatibilityNamespaceOnly = 1,
		DenyCrossNamespaceReuse = 2
	};

	enum class VirtualizationDecisionAcceptanceStateV2 : unsigned char
	{
		Draft = 0,
		Accepted = 1,
		Revoked = 2,
		Superseded = 3
	};

	enum class VirtualizationDecisionReviewRoleV2 : unsigned char
	{
		Unspecified = 0,
		OwnerReviewRole = 1,
		ArchitectureReviewRole = 2
	};

	enum class VirtualizationDecisionReviewAuthorityPlaneV2 : unsigned char
	{
		Unspecified = 0,
		NeutralRuntimeOwner = 1,
		Architecture = 2,
		CompatibilityFrontend = 3
	};

	enum class VirtualizationDecisionReviewStateV2 : unsigned char
	{
		Missing = 0,
		Completed = 1
	};

	enum class VirtualizationDecisionRevocationStateV2 : unsigned char
	{
		Draft = 0,
		Effective = 1
	};

	enum class VirtualizationDecisionSupersessionStateV2 : unsigned char
	{
		Draft = 0,
		Effective = 1
	};

	enum class VirtualizationNamespaceClassV2 : unsigned char
	{
		RuntimeAuthority = 0,
		FrozenCompatibility = 1
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionOwnerMapEntryV2(string FieldOrOperation, string Owner, string ValueSource, string CapabilityPolicy, string EvidenceClass, string MigrationClass, string DenialReason);

	/// <summary>
	/// Immutable governance specification. It describes policy, never a runtime grant.
	/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal sealed record VirtualizationDecisionSpecV2(uint SchemaVersion, string DecisionId, string OperationNamespace, ushort LeafWidth, ushort InvalidLeaf, ushort NumericLeaf, string OperationId, VirtualizationDecisionOwnerClassV2 OwnerClass, ulong OwnerId, uint OwnerPolicyVersion, uint OwnerEpoch, uint OperandAbiVersion, string Rs1Contract, string Rs2Contract, string RdContract, VirtualizationDecisionResultAbiV2 ResultAbi, VirtualizationDecisionEffectClassV2 EffectClass, VirtualizationCapabilityRequirementV2 CapabilityRequirement, ulong CapabilityMask, bool RequiresTypedGrant, VirtualizationDelegationPolicyV2 DelegationPolicy, VirtualizationRevocationPolicyV2 RevocationPolicy, VirtualizationCapabilityMigrationClassV2 CapabilityMigrationClass, VirtualizationEvidenceVisibilityV2 EvidenceVisibility, VirtualizationProjectionPolicyV2 FrontendProjectionPolicy, VirtualizationExecutionEvidenceRequirementV2 ExecutionEvidenceRequirement, VirtualizationDomainRequirementV2 DomainRequirement, bool RequireNonZeroDomainTag, bool RequiresMemoryDomain, bool RequiresIoDomain, VirtualizationAddressSpaceRequirementV2 AddressSpaceRequirement, VirtualizationSecureDomainPolicyV2 SecureDomainPolicy, VirtualizationCancellationPolicyV2 CancellationPolicy, VirtualizationReplayPolicyV2 ReplayPolicy, VirtualizationOperationMigrationPolicyV2 OperationMigrationPolicy, VirtualizationCompletionEvidenceClassV2 CompletionEvidenceClass, VirtualizationCompletionMigrationClassV2 CompletionMigrationClass, VirtualizationProjectionPolicyV2 CompletionProjectionPolicy, VirtualizationCompletionPolicyV2 CompletionPolicy, VirtualizationRetirePolicyV2 RetirePolicy, VirtualizationAdjacentLeafPolicyV2 AdjacentLeafPolicy, VirtualizationCrossNamespacePolicyV2 CrossNamespacePolicy, ImmutableArray<VirtualizationDecisionOwnerMapEntryV2> OwnerMap, string SpecDigest, VirtualizationDecisionOperationClassV2 OperationClass = VirtualizationDecisionOperationClassV2.Unspecified, VirtualizationDecisionAuthorityPlaneV2 AuthorityPlane = VirtualizationDecisionAuthorityPlaneV2.Unspecified, ImmutableArray<ushort>? ExactFieldIds = null, VirtualizationDecisionMutationClassV2 MutationClass = VirtualizationDecisionMutationClassV2.Unspecified, string DependencyContract = "", bool VmcsMetadataOnly = false, bool RequiresConformanceProof = false);
//	internal sealed record VirtualizationDecisionSpecV2(uint SchemaVersion, string DecisionId, string OperationNamespace, ushort LeafWidth, ushort InvalidLeaf, ushort NumericLeaf, string OperationId, VirtualizationDecisionOwnerClassV2 OwnerClass, ulong OwnerId, uint OwnerPolicyVersion, uint OwnerEpoch, uint OperandAbiVersion, string Rs1Contract, string Rs2Contract, string RdContract, VirtualizationDecisionResultAbiV2 ResultAbi, VirtualizationDecisionEffectClassV2 EffectClass, VirtualizationCapabilityRequirementV2 CapabilityRequirement, ulong CapabilityMask, bool RequiresTypedGrant, VirtualizationDelegationPolicyV2 DelegationPolicy, VirtualizationRevocationPolicyV2 RevocationPolicy, VirtualizationCapabilityMigrationClassV2 CapabilityMigrationClass, VirtualizationEvidenceVisibilityV2 EvidenceVisibility, VirtualizationProjectionPolicyV2 FrontendProjectionPolicy, VirtualizationExecutionEvidenceRequirementV2 ExecutionEvidenceRequirement, VirtualizationDomainRequirementV2 DomainRequirement, bool RequireNonZeroDomainTag, bool RequiresMemoryDomain, bool RequiresIoDomain, VirtualizationAddressSpaceRequirementV2 AddressSpaceRequirement, VirtualizationSecureDomainPolicyV2 SecureDomainPolicy, VirtualizationCancellationPolicyV2 CancellationPolicy, VirtualizationReplayPolicyV2 ReplayPolicy, VirtualizationOperationMigrationPolicyV2 OperationMigrationPolicy, VirtualizationCompletionEvidenceClassV2 CompletionEvidenceClass, VirtualizationCompletionMigrationClassV2 CompletionMigrationClass, VirtualizationProjectionPolicyV2 CompletionProjectionPolicy, VirtualizationCompletionPolicyV2 CompletionPolicy, VirtualizationRetirePolicyV2 RetirePolicy, VirtualizationAdjacentLeafPolicyV2 AdjacentLeafPolicy, VirtualizationCrossNamespacePolicyV2 CrossNamespacePolicy, ImmutableArray<VirtualizationDecisionOwnerMapEntryV2> OwnerMap, string SpecDigest, VirtualizationDecisionOperationClassV2 OperationClass = VirtualizationDecisionOperationClassV2.Unspecified, VirtualizationDecisionAuthorityPlaneV2 AuthorityPlane = VirtualizationDecisionAuthorityPlaneV2.Unspecified, ImmutableArray<ushort> ExactFieldIds = nullptr, VirtualizationDecisionMutationClassV2 MutationClass = VirtualizationDecisionMutationClassV2.Unspecified, string DependencyContract = "", bool VmcsMetadataOnly = false, bool RequiresConformanceProof = false);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionReviewEvidenceV2(VirtualizationDecisionReviewRoleV2 Role, VirtualizationDecisionReviewAuthorityPlaneV2 AuthorityPlane, VirtualizationDecisionReviewStateV2 State, string Principal, string ReviewedDecisionId, string ReviewedSpecDigest, string ReviewedSpecCommitSha, string EvidenceId);

	/// <summary>
	/// Immutable later artifact referring to spec bytes already present at SpecCommitSha.
	/// It never records the SHA of its own future containing commit.
	/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal sealed record VirtualizationDecisionAcceptanceRecordV2(uint SchemaVersion, string DecisionId, string SpecDigest, string SpecCommitSha, VirtualizationDecisionAcceptanceStateV2 AcceptanceState, string AcceptedBy, uint AcceptancePolicyVersion, VirtualizationDecisionReviewEvidenceV2 OwnerReviewEvidence, VirtualizationDecisionReviewEvidenceV2 ArchitectureReviewEvidence, string CodeOwnersBlobSha, string? SupersedesDecisionId, string? SupersedesAcceptanceDigest, string AcceptanceDigest);
//	internal sealed record VirtualizationDecisionAcceptanceRecordV2(uint SchemaVersion, string DecisionId, string SpecDigest, string SpecCommitSha, VirtualizationDecisionAcceptanceStateV2 AcceptanceState, string AcceptedBy, uint AcceptancePolicyVersion, VirtualizationDecisionReviewEvidenceV2 OwnerReviewEvidence, VirtualizationDecisionReviewEvidenceV2 ArchitectureReviewEvidence, string CodeOwnersBlobSha, string SupersedesDecisionId, string SupersedesAcceptanceDigest, string AcceptanceDigest);

	/// <summary>
	/// Revocation is append-only governance lineage; an accepted record is never edited.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionRevocationRecordV2(uint SchemaVersion, string RevocationId, string DecisionId, string AcceptanceDigest, VirtualizationDecisionRevocationStateV2 State, string RevokedBy, string Reason, ulong Sequence, string RevocationDigest);

	/// <summary>
	/// Supersession is append-only governance lineage; both referenced decisions remain immutable.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionSupersessionRecordV2(uint SchemaVersion, string SupersessionId, string SupersededDecisionId, string SupersededAcceptanceDigest, string SupersedingDecisionId, string SupersedingAcceptanceDigest, VirtualizationDecisionSupersessionStateV2 State, string SupersededBy, ulong Sequence, string SupersessionDigest);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationCodeOwnersRuleV2(string Scope, string Principal);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationCodeOwnersEvidenceV2(bool FilePresent, string BlobSha, ImmutableArray<VirtualizationCodeOwnersRuleV2> Rules);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationNamespaceAllocationV2(string OperationNamespace, ushort LeafWidth, ushort NumericLeaf, string DecisionId, VirtualizationNamespaceClassV2 NamespaceClass);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionRevocationEvidenceV2(VirtualizationDecisionRevocationRecordV2 Record, ImmutableArray<byte> CanonicalBytes);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionSupersessionEvidenceV2(VirtualizationDecisionSupersessionRecordV2 Record, ImmutableArray<byte> CanonicalBytes);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionValidationEvidenceV2(ImmutableArray<byte> SpecCanonicalBytes, ImmutableArray<byte> AcceptanceCanonicalBytes, ImmutableArray<byte> SpecBytesAtCommit, string ResolvedSpecCommitSha, string AcceptanceContainingCommitSha, VirtualizationCodeOwnersEvidenceV2 CodeOwners, ImmutableArray<VirtualizationNamespaceAllocationV2> ExistingAllocations, ImmutableArray<VirtualizationDecisionRevocationEvidenceV2> Revocations, ImmutableArray<VirtualizationDecisionSupersessionEvidenceV2> Supersessions);

	/// <summary>
	/// Positive validator output is immutable policy metadata. It is deliberately not a
	/// capability, grant, owner registry entry, operation lookup, admission or execution token.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record AcceptedVirtualizationDecision(string DecisionId, string SpecDigest, string AcceptanceDigest, string SpecCommitSha, string OperationNamespace, ushort NumericLeaf, ulong OwnerId, uint OwnerPolicyVersion, uint OwnerEpoch, VirtualizationDecisionEffectClassV2 EffectClass, VirtualizationDecisionAdjacentPolicySnapshotV2 AdjacentPolicy);

	/// <summary>
	/// Machine-accepted read-projection policy metadata. It deliberately has no
	/// projection value, capability, admission, execution or publication method.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record AcceptedVmReadProjectionDecisionV2(string DecisionId, string SpecDigest, string AcceptanceDigest, string SpecCommitSha, string OperationNamespace, ImmutableArray<ushort> ExactFieldIds, ulong OwnerId, uint OwnerPolicyVersion, uint OwnerEpoch, VirtualizationDecisionMutationClassV2 MutationClass);

	/// <summary>
	/// Machine-accepted scalar-delivery policy metadata. It is not a source owner,
	/// result receipt, execution token, writeback packet or retire authorization.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record AcceptedVmReadScalarDeliveryDecisionV2(string DecisionId, string SpecDigest, string AcceptanceDigest, string SpecCommitSha, string OperationNamespace, ImmutableArray<ushort> ExactFieldIds, ulong SourceOwnerId, uint SourceOwnerPolicyVersion, uint SourceOwnerEpoch, VirtualizationDecisionResultAbiV2 ResultAbi, VirtualizationDecisionEffectClassV2 EffectClass, VirtualizationOperationMigrationPolicyV2 MigrationPolicy, VirtualizationRetirePolicyV2 RetirePolicy);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationDecisionAdjacentPolicySnapshotV2(VirtualizationAdjacentLeafPolicyV2 AdjacentLeafPolicy, VirtualizationCrossNamespacePolicyV2 CrossNamespacePolicy);


	// Active immutable native forms of the C# records above.
	struct VirtualizationDecisionOwnerMapEntryV2 final {
		const std::wstring FieldOrOperation, Owner, ValueSource, CapabilityPolicy, EvidenceClass, MigrationClass, DenialReason;
		friend bool operator==(const VirtualizationDecisionOwnerMapEntryV2&, const VirtualizationDecisionOwnerMapEntryV2&) = default;
	};
	struct VirtualizationDecisionSpecV2 final {
		const unsigned int SchemaVersion{};
		const std::wstring DecisionId, OperationNamespace;
		const unsigned short LeafWidth{}, InvalidLeaf{}, NumericLeaf{};
		const std::wstring OperationId;
		const VirtualizationDecisionOwnerClassV2 OwnerClass{};
		const unsigned long long OwnerId{};
		const unsigned int OwnerPolicyVersion{}, OwnerEpoch{}, OperandAbiVersion{};
		const std::wstring Rs1Contract, Rs2Contract, RdContract;
		const VirtualizationDecisionResultAbiV2 ResultAbi{};
		const VirtualizationDecisionEffectClassV2 EffectClass{};
		const VirtualizationCapabilityRequirementV2 CapabilityRequirement{};
		const unsigned long long CapabilityMask{};
		const bool RequiresTypedGrant{};
		const VirtualizationDelegationPolicyV2 DelegationPolicy{};
		const VirtualizationRevocationPolicyV2 RevocationPolicy{};
		const VirtualizationCapabilityMigrationClassV2 CapabilityMigrationClass{};
		const VirtualizationEvidenceVisibilityV2 EvidenceVisibility{};
		const VirtualizationProjectionPolicyV2 FrontendProjectionPolicy{};
		const VirtualizationExecutionEvidenceRequirementV2 ExecutionEvidenceRequirement{};
		const VirtualizationDomainRequirementV2 DomainRequirement{};
		const bool RequireNonZeroDomainTag{}, RequiresMemoryDomain{}, RequiresIoDomain{};
		const VirtualizationAddressSpaceRequirementV2 AddressSpaceRequirement{};
		const VirtualizationSecureDomainPolicyV2 SecureDomainPolicy{};
		const VirtualizationCancellationPolicyV2 CancellationPolicy{};
		const VirtualizationReplayPolicyV2 ReplayPolicy{};
		const VirtualizationOperationMigrationPolicyV2 OperationMigrationPolicy{};
		const VirtualizationCompletionEvidenceClassV2 CompletionEvidenceClass{};
		const VirtualizationCompletionMigrationClassV2 CompletionMigrationClass{};
		const VirtualizationProjectionPolicyV2 CompletionProjectionPolicy{};
		const VirtualizationCompletionPolicyV2 CompletionPolicy{};
		const VirtualizationRetirePolicyV2 RetirePolicy{};
		const VirtualizationAdjacentLeafPolicyV2 AdjacentLeafPolicy{};
		const VirtualizationCrossNamespacePolicyV2 CrossNamespacePolicy{};
		const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> OwnerMap;
		const std::wstring SpecDigest;
		const VirtualizationDecisionOperationClassV2 OperationClass = VirtualizationDecisionOperationClassV2::Unspecified;
		const VirtualizationDecisionAuthorityPlaneV2 AuthorityPlane = VirtualizationDecisionAuthorityPlaneV2::Unspecified;
		const std::optional<std::vector<unsigned short>> ExactFieldIds = std::nullopt;
		const VirtualizationDecisionMutationClassV2 MutationClass = VirtualizationDecisionMutationClassV2::Unspecified;
		const std::wstring DependencyContract = L"";
		const bool VmcsMetadataOnly = false, RequiresConformanceProof = false;
        std::shared_ptr<VirtualizationDecisionSpecV2> WithSpecDigest(const std::wstring& digest) const
		{
			return std::make_shared<VirtualizationDecisionSpecV2>(SchemaVersion, DecisionId, OperationNamespace, LeafWidth, InvalidLeaf, NumericLeaf, OperationId, OwnerClass, OwnerId, OwnerPolicyVersion, OwnerEpoch, OperandAbiVersion, Rs1Contract, Rs2Contract, RdContract, ResultAbi, EffectClass, CapabilityRequirement, CapabilityMask, RequiresTypedGrant, DelegationPolicy, RevocationPolicy, CapabilityMigrationClass, EvidenceVisibility, FrontendProjectionPolicy, ExecutionEvidenceRequirement, DomainRequirement, RequireNonZeroDomainTag, RequiresMemoryDomain, RequiresIoDomain, AddressSpaceRequirement, SecureDomainPolicy, CancellationPolicy, ReplayPolicy, OperationMigrationPolicy, CompletionEvidenceClass, CompletionMigrationClass, CompletionProjectionPolicy, CompletionPolicy, RetirePolicy, AdjacentLeafPolicy, CrossNamespacePolicy, OwnerMap, digest, OperationClass, AuthorityPlane, ExactFieldIds, MutationClass, DependencyContract, VmcsMetadataOnly, RequiresConformanceProof);
		}
		friend bool operator==(const VirtualizationDecisionSpecV2&, const VirtualizationDecisionSpecV2&) = default;
	};
	struct VirtualizationDecisionReviewEvidenceV2 final {
		const VirtualizationDecisionReviewRoleV2 Role{};
		const VirtualizationDecisionReviewAuthorityPlaneV2 AuthorityPlane{};
		const VirtualizationDecisionReviewStateV2 State{};
		const std::wstring Principal, ReviewedDecisionId, ReviewedSpecDigest, ReviewedSpecCommitSha, EvidenceId;
		friend bool operator==(const VirtualizationDecisionReviewEvidenceV2&, const VirtualizationDecisionReviewEvidenceV2&) = default;
	};
	struct VirtualizationDecisionAcceptanceRecordV2 final {
		const unsigned int SchemaVersion{};
		const std::wstring DecisionId, SpecDigest, SpecCommitSha;
		const VirtualizationDecisionAcceptanceStateV2 AcceptanceState{};
		const std::wstring AcceptedBy;
		const unsigned int AcceptancePolicyVersion{};
		const std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> OwnerReviewEvidence, ArchitectureReviewEvidence;
		const std::wstring CodeOwnersBlobSha;
		const std::shared_ptr<const std::wstring> SupersedesDecisionId, SupersedesAcceptanceDigest;
		const std::wstring AcceptanceDigest;
        std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> WithAcceptanceDigest(const std::wstring& digest) const
		{
			return std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(SchemaVersion, DecisionId, SpecDigest, SpecCommitSha, AcceptanceState, AcceptedBy, AcceptancePolicyVersion, OwnerReviewEvidence, ArchitectureReviewEvidence, CodeOwnersBlobSha, SupersedesDecisionId, SupersedesAcceptanceDigest, digest);
		}
		friend bool operator==(const VirtualizationDecisionAcceptanceRecordV2&, const VirtualizationDecisionAcceptanceRecordV2&) = default;
	};
	struct VirtualizationDecisionRevocationRecordV2 final {
		const unsigned int SchemaVersion{};
		const std::wstring RevocationId, DecisionId, AcceptanceDigest;
		const VirtualizationDecisionRevocationStateV2 State{};
		const std::wstring RevokedBy, Reason;
		const unsigned long long Sequence{};
		const std::wstring RevocationDigest;
		friend bool operator==(const VirtualizationDecisionRevocationRecordV2&, const VirtualizationDecisionRevocationRecordV2&) = default;
	};
	struct VirtualizationDecisionSupersessionRecordV2 final {
		const unsigned int SchemaVersion{};
		const std::wstring SupersessionId, SupersededDecisionId, SupersededAcceptanceDigest, SupersedingDecisionId, SupersedingAcceptanceDigest;
		const VirtualizationDecisionSupersessionStateV2 State{};
		const std::wstring SupersededBy;
		const unsigned long long Sequence{};
		const std::wstring SupersessionDigest;
		friend bool operator==(const VirtualizationDecisionSupersessionRecordV2&, const VirtualizationDecisionSupersessionRecordV2&) = default;
	};
	struct VirtualizationCodeOwnersRuleV2 final {
		const std::wstring Scope, Principal;
		friend bool operator==(const VirtualizationCodeOwnersRuleV2&, const VirtualizationCodeOwnersRuleV2&) = default;
	};
	struct VirtualizationCodeOwnersEvidenceV2 final {
		const bool FilePresent{};
		const std::wstring BlobSha;
		const std::vector<std::shared_ptr<VirtualizationCodeOwnersRuleV2>> Rules;
		friend bool operator==(const VirtualizationCodeOwnersEvidenceV2&, const VirtualizationCodeOwnersEvidenceV2&) = default;
	};
	struct VirtualizationNamespaceAllocationV2 final {
		const std::wstring OperationNamespace;
		const unsigned short LeafWidth{}, NumericLeaf{};
		const std::wstring DecisionId;
		const VirtualizationNamespaceClassV2 NamespaceClass{};
		friend bool operator==(const VirtualizationNamespaceAllocationV2&, const VirtualizationNamespaceAllocationV2&) = default;
	};
	struct VirtualizationDecisionRevocationEvidenceV2 final {
		const std::shared_ptr<VirtualizationDecisionRevocationRecordV2> Record;
		const std::vector<unsigned char> CanonicalBytes;
		friend bool operator==(const VirtualizationDecisionRevocationEvidenceV2&, const VirtualizationDecisionRevocationEvidenceV2&) = default;
	};
	struct VirtualizationDecisionSupersessionEvidenceV2 final {
		const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2> Record;
		const std::vector<unsigned char> CanonicalBytes;
		friend bool operator==(const VirtualizationDecisionSupersessionEvidenceV2&, const VirtualizationDecisionSupersessionEvidenceV2&) = default;
	};
	struct VirtualizationDecisionValidationEvidenceV2 final {
		const std::vector<unsigned char> SpecCanonicalBytes, AcceptanceCanonicalBytes, SpecBytesAtCommit;
		const std::wstring ResolvedSpecCommitSha, AcceptanceContainingCommitSha;
		const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> CodeOwners;
		const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>> ExistingAllocations;
		const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>> Revocations;
		const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>> Supersessions;
		VirtualizationDecisionValidationEvidenceV2(
			std::vector<unsigned char> specCanonicalBytes,
			std::vector<unsigned char> acceptanceCanonicalBytes,
			std::vector<unsigned char> specBytesAtCommit,
			std::wstring resolvedSpecCommitSha,
			std::wstring acceptanceContainingCommitSha,
			std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> codeOwners,
			std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>> existingAllocations,
			std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>> revocations,
			std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>> supersessions)
			: SpecCanonicalBytes(std::move(specCanonicalBytes)),
			  AcceptanceCanonicalBytes(std::move(acceptanceCanonicalBytes)),
			  SpecBytesAtCommit(std::move(specBytesAtCommit)),
			  ResolvedSpecCommitSha(std::move(resolvedSpecCommitSha)),
			  AcceptanceContainingCommitSha(std::move(acceptanceContainingCommitSha)),
			  CodeOwners(std::move(codeOwners)),
			  ExistingAllocations(std::move(existingAllocations)),
			  Revocations(std::move(revocations)),
			  Supersessions(std::move(supersessions)) {}
		friend bool operator==(const VirtualizationDecisionValidationEvidenceV2&, const VirtualizationDecisionValidationEvidenceV2&) = default;
	};
	struct VirtualizationDecisionAdjacentPolicySnapshotV2 final {
		const VirtualizationAdjacentLeafPolicyV2 AdjacentLeafPolicy{};
		const VirtualizationCrossNamespacePolicyV2 CrossNamespacePolicy{};
		friend bool operator==(const VirtualizationDecisionAdjacentPolicySnapshotV2&, const VirtualizationDecisionAdjacentPolicySnapshotV2&) = default;
	};
	struct AcceptedVirtualizationDecision final {
		const std::wstring DecisionId, SpecDigest, AcceptanceDigest, SpecCommitSha, OperationNamespace;
		const unsigned short NumericLeaf{};
		const unsigned long long OwnerId{};
		const unsigned int OwnerPolicyVersion{}, OwnerEpoch{};
		const VirtualizationDecisionEffectClassV2 EffectClass{};
		const std::shared_ptr<VirtualizationDecisionAdjacentPolicySnapshotV2> AdjacentPolicy;
		friend bool operator==(const AcceptedVirtualizationDecision&, const AcceptedVirtualizationDecision&) = default;
	};
	struct AcceptedVmReadProjectionDecisionV2 final {
		const std::wstring DecisionId, SpecDigest, AcceptanceDigest, SpecCommitSha, OperationNamespace;
		const std::vector<unsigned short> ExactFieldIds;
		const unsigned long long OwnerId{};
		const unsigned int OwnerPolicyVersion{}, OwnerEpoch{};
		const VirtualizationDecisionMutationClassV2 MutationClass{};
		friend bool operator==(const AcceptedVmReadProjectionDecisionV2&, const AcceptedVmReadProjectionDecisionV2&) = default;
	};
	struct AcceptedVmReadScalarDeliveryDecisionV2 final {
		const std::wstring DecisionId, SpecDigest, AcceptanceDigest, SpecCommitSha, OperationNamespace;
		const std::vector<unsigned short> ExactFieldIds;
		const unsigned long long SourceOwnerId{};
		const unsigned int SourceOwnerPolicyVersion{}, SourceOwnerEpoch{};
		const VirtualizationDecisionResultAbiV2 ResultAbi{};
		const VirtualizationDecisionEffectClassV2 EffectClass{};
		const VirtualizationOperationMigrationPolicyV2 MigrationPolicy{};
		const VirtualizationRetirePolicyV2 RetirePolicy{};
		friend bool operator==(const AcceptedVmReadScalarDeliveryDecisionV2&, const AcceptedVmReadScalarDeliveryDecisionV2&) = default;
	};
}
