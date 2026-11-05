#pragma once
#include "VirtualizationDecisionContractsV2.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
    enum class VirtualizationDecisionValidationDecisionV2 : unsigned char
    {
        AcceptedPolicyObject = 0, DeniedMissingArtifact = 1, DeniedSchemaVersion = 2,
        DeniedMalformedDigest = 3, DeniedSpecDigestMismatch = 4, DeniedAcceptanceDigestMismatch = 5,
        DeniedNonCanonicalSpecBytes = 6, DeniedNonCanonicalAcceptanceBytes = 7,
        DeniedSpecBytesAtCommitMismatch = 8, DeniedMalformedCommitSha = 9,
        DeniedSelfReferentialCommitSha = 10, DeniedDecisionIdentity = 11, DeniedOwner = 12,
        DeniedAbi = 13, DeniedPolicyMissing = 14, DeniedUnknownPolicy = 15,
        DeniedPolicyProfile = 16, DeniedOwnerMapIncomplete = 17, DeniedOwnerMapMismatch = 18,
        DeniedZeroLeaf = 19, DeniedDuplicateLeaf = 20, DeniedAdjacentLeaf = 21,
        DeniedCrossNamespaceLeaf = 22, DeniedNamespaceRule = 23, DeniedAcceptanceState = 24,
        DeniedCodeOwners = 25, DeniedReviewRole = 26, DeniedReviewMismatch = 27,
        DeniedCompatibilityReview = 28, DeniedInvalidLineage = 29, DeniedRevoked = 30,
        DeniedSuperseded = 31
    };

    struct VirtualizationDecisionValidationResultV2 final
    {
        const VirtualizationDecisionValidationDecisionV2 Decision;
        const std::wstring Reason;
        const std::shared_ptr<AcceptedVirtualizationDecision> AcceptedDecision;
        const bool IsAcceptedPolicyObject;
        const bool RuntimeCapabilityGranted = false;
        const bool BackendExecutionAuthorized = false;
        const bool CompletionPublicationAuthorized = false;
        const bool RetirePublicationAuthorized = false;
        VirtualizationDecisionValidationResultV2(VirtualizationDecisionValidationDecisionV2 decision,
            std::wstring reason, std::shared_ptr<AcceptedVirtualizationDecision> acceptedDecision)
            : Decision(decision), Reason(std::move(reason)), AcceptedDecision(std::move(acceptedDecision)),
              IsAcceptedPolicyObject(decision == VirtualizationDecisionValidationDecisionV2::AcceptedPolicyObject && AcceptedDecision != nullptr) {}
    };

    class VirtualizationDecisionValidatorV2 final
    {
    public:
        static constexpr unsigned int CurrentSchemaVersion = 2;
        static inline const std::wstring ExpectedDecisionId = L"D2-HV-VMCALL-RUNTIME-V1-PROBE-0001";
        static inline const std::wstring ExpectedOperationNamespace = L"HybridCPU.VMCALL.Runtime.v1";
        static inline const std::wstring ExpectedOperationId = L"PROBE_NO_STATE_V1";
        static constexpr unsigned long long ExpectedOwnerId = 0x4843'4F57'4E52ULL;
        static constexpr unsigned long long ExpectedCapabilityMask = 1ULL << 41;
        static std::shared_ptr<VirtualizationDecisionValidationResultV2> Validate(
            const std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>>& spec,
            const std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>>& acceptance,
            const std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>>& evidence);
    private:
        static inline const std::unordered_map<std::wstring, std::wstring> RequiredOwnerMap = {
            {L"Operation", L"DomainHypercallRuntimeOwner"},
            {L"OperandAbi", L"CanonicalOperandSnapshotOwner"},
            {L"CapabilityAdmission", L"RuntimeCapabilityOwner"},
            {L"DomainAdmission", L"RuntimeDomainOwner"},
            {L"ExecutionAdmission", L"SafetyVerifier"},
            {L"CancellationReplay", L"DomainHypercallRuntimeOwner"},
            {L"CompletionPublication", L"NeutralCompletionOwner"},
            {L"RetirePublication", L"CanonicalRetireOwner"},
            {L"MigrationRestore", L"CheckpointRestoreOwner"},
            {L"AdjacentLeafDenial", L"VirtualizationDecisionValidatorV2"}};
        static inline const std::vector<std::wstring> RequiredCodeOwnersScopes = {
            L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Events/Hypercalls/",
            L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Capabilities/",
            L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Safety/",
            L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Completion/",
            L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/",
            L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/"};

        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidateCanonicalArtifacts(
            const std::shared_ptr<VirtualizationDecisionSpecV2>& spec,
            const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& acceptance,
            const std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>& evidence);
        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidateOwnerAndAbi(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec);
        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidatePolicies(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec);
        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidateOwnerMap(const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>& ownerMap);
        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidateNamespaceAndLeaf(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec, const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>>& allocations);
        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidateAcceptance(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec, const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& acceptance, const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2>& codeOwners);
        static bool IsRepositoryPrincipal(const std::wstring& principal);
        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidateReview(const std::shared_ptr<VirtualizationDecisionReviewEvidenceV2>& review, VirtualizationDecisionReviewRoleV2 requiredRole, VirtualizationDecisionReviewAuthorityPlaneV2 requiredPlane, const std::shared_ptr<VirtualizationDecisionSpecV2>& spec, const std::wstring& specCommitSha);
        static std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> ValidateLineage(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& acceptance, const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>>& revocations, const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>>& supersessions);
        static bool FixedEquals(const std::wstring& left, const std::wstring& right);
        static bool FixedEquals(const std::vector<unsigned char>& left, const std::vector<unsigned char>& right);
        static std::shared_ptr<VirtualizationDecisionValidationResultV2> Deny(VirtualizationDecisionValidationDecisionV2 decision, const std::wstring& reason);
    };
}
