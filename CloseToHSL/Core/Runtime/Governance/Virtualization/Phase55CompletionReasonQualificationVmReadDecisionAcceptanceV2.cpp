#include "Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2.h"
#include "CompletionReasonQualificationVmReadDecisionValidatorV2.h"
#include "Phase55CompletionReasonQualificationVmReadDecisionSpecV2.h"
#include "VirtualizationDecisionContractsV2.h"
#include <optional>

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getRecord()
	{
		return GetRecord();
	}

	std::vector<unsigned char> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getCodeOwnersEvidence()
	{
		return GetCodeOwnersEvidence();
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::GetRecord()
	{
		static const auto record = CreateRecord();
		return record;
	}

	std::vector<unsigned char> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(GetRecord());
		return bytes;
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::GetCodeOwnersEvidence()
	{
		static const auto evidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(true, CodeOwnersBlobSha, std::vector<std::shared_ptr<VirtualizationCodeOwnersRuleV2>>{Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/"), Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Completion/"), Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/Completion/"), Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/"), Rule(L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/"),});
		return evidence;
	}

	bool Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getCompletionCreationAuthorized()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getProductionCompositionAuthorized()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getGuestPhysicalAddressAuthorized()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::getEptViolationQualificationAuthorized()
	{
		return false;
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha)
	{
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> spec = Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getInstance();
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> record = getRecord();
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getCanonicalBytes(), getCanonicalBytes(), Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getCanonicalBytes(), SpecCommitSha, acceptanceContainingCommitSha, getCodeOwnersEvidence(), std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>>{});
		return CompletionReasonQualificationVmReadDecisionValidatorV2::Validate(spec, record, evidence);
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::CreateRecord()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getInstance();
		if (!spec)
		{
			throw InvalidOperationException(L"Phase 55 SpecV2 is unavailable.");
		}
		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw InvalidOperationException(L"Phase 55 SpecV2 drifted from its immutable subject commit.");
		}
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> ownerReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, L"PHASE55-COMPLETION-OBSERVATION-OWNER-REVIEW-2026-08-13-9497D61");

//		VirtualizationDecisionReviewEvidenceV2 architectureReview = ownerReview with { Role = VirtualizationDecisionReviewRoleV2.ArchitectureReviewRole, AuthorityPlane = VirtualizationDecisionReviewAuthorityPlaneV2.Architecture, EvidenceId = "PHASE55-ARCHITECTURE-REVIEW-2026-08-13-9497D61"};
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> architectureReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId);
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> record = std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(CompletionReasonQualificationVmReadDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64));
		record = record->WithAcceptanceDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(record));
		if (record->AcceptanceDigest != ExpectedAcceptanceDigest)
		{
			throw InvalidOperationException(std::format(L"Phase 55 AcceptanceRecordV2 digest is {0:s}.", record->AcceptanceDigest));
		}
		return record;
	}

	std::shared_ptr<VirtualizationCodeOwnersRuleV2> Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2::Rule(const std::wstring &scope)
	{
		return std::make_shared<VirtualizationCodeOwnersRuleV2>(scope, RepositoryPrincipal);
	}
}
