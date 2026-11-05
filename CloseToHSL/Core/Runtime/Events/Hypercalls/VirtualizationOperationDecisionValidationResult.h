#pragma once
#include <string>
#include <utility>
namespace YAKSys_Hybrid_CPU::Core {
	enum class VirtualizationOperationDecisionValidationDecision : unsigned char
	{
		ValidGovernanceArtifactOnly = 0,
		DeniedSchemaVersion = 1,
		DeniedDecisionNotAccepted = 2,
		DeniedDecisionIdentity = 3,
		DeniedNeutralOwnerAttribution = 4,
		DeniedAcceptedCommitSha = 5,
		DeniedCodeOwnersRule = 6,
		DeniedRequiredReview = 7,
		DeniedReviewerMismatch = 8,
		DeniedCompatibilitySelfApproval = 9,
		DeniedExactLeafCardinality = 10,
		DeniedDuplicateExactLeaf = 11,
		DeniedOwnerMapIncomplete = 12
	};
struct VirtualizationOperationDecisionValidationResult final {
    VirtualizationOperationDecisionValidationDecision Decision;
    std::wstring Reason;
    bool IsStructurallyValidGovernanceEvidence() const noexcept { return Decision == VirtualizationOperationDecisionValidationDecision::ValidGovernanceArtifactOnly; }
    bool BackendExecutionAuthorized() const noexcept { return false; }
    bool CompletionPublicationAuthorized() const noexcept { return false; }
    bool RetirePublicationAuthorized() const noexcept { return false; }
    bool operator==(const VirtualizationOperationDecisionValidationResult&) const = default;
};
}