#include "Phase40VmReadProjectionDecisionAcceptanceV2.h"
#include "Phase40VmReadProjectionDecisionSpecV2.h"
#include "VmReadProjectionDecisionValidatorV2.h"
#include "VirtualizationDecisionContractsV2.h"
#include "VirtualizationDecisionValidatorV2.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase40VmReadProjectionDecisionAcceptanceV2::getRecord()
	{
		return GetRecord();
	}

	std::vector<unsigned char> Phase40VmReadProjectionDecisionAcceptanceV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase40VmReadProjectionDecisionAcceptanceV2::getCodeOwnersEvidence()
	{
		return GetCodeOwnersEvidence();
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase40VmReadProjectionDecisionAcceptanceV2::GetRecord()
	{
		static const auto record = CreateRecord();
		return record;
	}

	std::vector<unsigned char> Phase40VmReadProjectionDecisionAcceptanceV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(GetRecord());
		return bytes;
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase40VmReadProjectionDecisionAcceptanceV2::GetCodeOwnersEvidence()
	{
		static const auto evidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(VirtualizationCodeOwnersEvidenceV2{true, CodeOwnersBlobSha, {
			std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/", RepositoryPrincipal}),
			std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Descriptors/ExecutionState/", RepositoryPrincipal}),
			std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Policies/ExecutionState/", RepositoryPrincipal}),
			std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/SecureCompute/Compatibility/Projection/", RepositoryPrincipal}),
			std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/", RepositoryPrincipal}),
			std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/", RepositoryPrincipal})
		}});
		return evidence;
	}

	bool Phase40VmReadProjectionDecisionAcceptanceV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase40VmReadProjectionDecisionAcceptanceV2::getProjectionValueAvailable()
	{
		return false;
	}

	bool Phase40VmReadProjectionDecisionAcceptanceV2::getCapabilityGranted()
	{
		return false;
	}

	bool Phase40VmReadProjectionDecisionAcceptanceV2::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase40VmReadProjectionDecisionAcceptanceV2::getMutationAuthorized()
	{
		return false;
	}

	bool Phase40VmReadProjectionDecisionAcceptanceV2::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase40VmReadProjectionDecisionAcceptanceV2::getRetirePublicationAuthorized()
	{
		return false;
	}

	std::shared_ptr<VmReadProjectionDecisionValidationResultV2> Phase40VmReadProjectionDecisionAcceptanceV2::ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha)
	{
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> spec = Phase40VmReadProjectionDecisionSpecV2::getInstance();
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> record = getRecord();
		const auto specBytes = Phase40VmReadProjectionDecisionSpecV2::getCanonicalBytes();
		const auto acceptanceBytes = getCanonicalBytes();
		const auto codeOwners = getCodeOwnersEvidence();
		const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>> allocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>> revocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>> supersessions{};
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(specBytes, acceptanceBytes, specBytes, SpecCommitSha, acceptanceContainingCommitSha, codeOwners, allocations, revocations, supersessions);
		return VmReadProjectionDecisionValidatorV2::Validate(spec, record, evidence);
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase40VmReadProjectionDecisionAcceptanceV2::CreateRecord()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = Phase40VmReadProjectionDecisionSpecV2::getInstance();
		if (!spec)
		{
			throw InvalidOperationException(L"Phase 40 SpecV2 is unavailable.");
		}
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw std::invalid_argument("The VMREAD SpecV2 digest drifted from the earlier reviewed commit.");
		}

		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> ownerReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, OwnerReviewEvidenceId});

//		VirtualizationDecisionReviewEvidenceV2 architectureReview = ownerReview with { Role = VirtualizationDecisionReviewRoleV2.ArchitectureReviewRole, AuthorityPlane = VirtualizationDecisionReviewAuthorityPlaneV2.Architecture, EvidenceId = ArchitectureReviewEvidenceId};
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> architectureReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId});
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> record = std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VirtualizationDecisionAcceptanceRecordV2{VirtualizationDecisionValidatorV2::CurrentSchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64)});
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		record = record->WithAcceptanceDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(record));
        if (record->AcceptanceDigest != ExpectedAcceptanceDigest)
		{
			throw std::invalid_argument("The attributable VMREAD AcceptanceRecordV2 digest drifted.");
		}

		return record;
	}
}
