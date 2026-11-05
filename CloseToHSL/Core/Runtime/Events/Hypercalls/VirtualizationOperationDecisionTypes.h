#pragma once
#include "NeutralHypercallBackendOwnerDescriptor.h"
#include <string>
#include <vector>
namespace YAKSys_Hybrid_CPU::Core {
enum class VirtualizationOperationDecisionState : unsigned char { Absent = 0, Draft = 1, Accepted = 2, Withdrawn = 3 };
struct VirtualizationOperationDecisionManifest final {
    unsigned int SchemaVersion = 0;
    std::wstring DecisionId;
    VirtualizationOperationDecisionState State = VirtualizationOperationDecisionState::Absent;
    std::wstring OperationName, DecisionOwner;
    NeutralHypercallBackendOwnerSource OwnerSource = NeutralHypercallBackendOwnerSource::None;
    std::wstring AcceptedCommitSha;
    std::vector<std::wstring> RequiredReviewers;
    std::vector<unsigned long long> ExactNumericLeaves;
    std::wstring ValueSource, CapabilityPolicy, EvidenceClass, MigrationClass, DenialReason;
    bool operator==(const VirtualizationOperationDecisionManifest&) const = default;
};
struct VirtualizationDecisionAttributionEvidence final {
    std::wstring DecisionOwner, AcceptedCommitSha;
    bool CodeOwnersRulePresent = false, CodeOwnersRuleMatched = false, RequiredReviewersApproved = false;
    std::vector<std::wstring> ApprovedReviewers;
    bool CompatibilityFrontendSelfApproved = false;
    bool operator==(const VirtualizationDecisionAttributionEvidence&) const = default;
};
}