#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2.h"
#include "GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2.h"
#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2.h"
#include "VirtualizationDecisionContractsV2.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getRecord()
	{
		return GetRecord();
	}

	std::vector<unsigned char> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getCodeOwnersEvidence()
	{
		return GetCodeOwnersEvidence();
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::GetRecord()
	{
		static const auto record = CreateRecord();
		return record;
	}

	std::vector<unsigned char> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(GetRecord());
		return bytes;
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::GetCodeOwnersEvidence()
	{
		static const auto evidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(VirtualizationCodeOwnersEvidenceV2{true, CodeOwnersBlobSha, {
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/Descriptors/ExecutionDomain/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/ExecutionFlow/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/MicroOps/Types/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Architecture/Registers/"),
			Rule(L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/")
		}});
		return evidence;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getUnderlyingVirtualizationMutationAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::getProductionCompositionAuthorized()
	{
		return false;
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha)
	{
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> spec = Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getInstance();
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> record = getRecord();
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes(), getCanonicalBytes(), Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes(), SpecCommitSha, acceptanceContainingCommitSha, getCodeOwnersEvidence(), std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>>{});
		return GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::Validate(spec, record, evidence);
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::CreateRecord()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getInstance();
		if (!spec)
		{
			throw InvalidOperationException("Phase 43 SpecV2 is unavailable.");
		}
		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw std::invalid_argument("Phase 43 SpecV2 drifted from its immutable subject commit.");
		}
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> ownerReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, L"PHASE43-EXECUTION-DOMAIN-OWNER-REVIEW-2026-08-11-46FC02C"});

//		VirtualizationDecisionReviewEvidenceV2 architectureReview = ownerReview with { Role = VirtualizationDecisionReviewRoleV2.ArchitectureReviewRole, AuthorityPlane = VirtualizationDecisionReviewAuthorityPlaneV2.Architecture, EvidenceId = "PHASE43-ARCHITECTURE-REVIEW-2026-08-11-46FC02C"};
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> architectureReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId});
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> record = std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VirtualizationDecisionAcceptanceRecordV2{GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64)});
		record = record->WithAcceptanceDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(record));
		if (record->AcceptanceDigest != ExpectedAcceptanceDigest)
		{
			throw InvalidOperationException(std::format(L"Phase 43 AcceptanceRecordV2 digest is {0:s}.", record->AcceptanceDigest));
		}
		return record;
	}

	std::shared_ptr<VirtualizationCodeOwnersRuleV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(const std::wstring &scope)
	{
		return std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{scope, RepositoryPrincipal});
	}
}
