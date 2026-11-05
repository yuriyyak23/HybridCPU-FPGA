#include "Phase41VmReadScalarDeliveryDecisionAcceptanceV2.h"
#include "Phase41VmReadScalarDeliveryDecisionSpecV2.h"
#include "VmReadScalarDeliveryDecisionValidatorV2.h"
#include "VirtualizationDecisionContractsV2.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getRecord()
	{
		return GetRecord();
	}

	std::vector<unsigned char> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getCodeOwnersEvidence()
	{
		return GetCodeOwnersEvidence();
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::GetRecord()
	{
		static const auto record = CreateRecord();
		return record;
	}

	std::vector<unsigned char> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(GetRecord());
		return bytes;
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::GetCodeOwnersEvidence()
	{
		static const auto evidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(VirtualizationCodeOwnersEvidenceV2{true, CodeOwnersBlobSha, {
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Descriptors/ExecutionState/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Policies/ExecutionState/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/SecureCompute/Compatibility/Projection/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/ExecutionFlow/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/MicroOps/Types/"),
			Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Architecture/Registers/"),
			Rule(L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/")
		}});
		return evidence;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getUnderlyingVirtualizationMutationAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionAcceptanceV2::getCompletionPublicationAuthorized()
	{
		return false;
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha)
	{
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> spec = Phase41VmReadScalarDeliveryDecisionSpecV2::getInstance();
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> record = getRecord();
		const auto specBytes = Phase41VmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes();
		const auto acceptanceBytes = getCanonicalBytes();
		const auto codeOwners = getCodeOwnersEvidence();
		const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>> allocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>> revocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>> supersessions{};
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(specBytes, acceptanceBytes, specBytes, SpecCommitSha, acceptanceContainingCommitSha, codeOwners, allocations, revocations, supersessions);
		return VmReadScalarDeliveryDecisionValidatorV2::Validate(spec, record, evidence);
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::CreateRecord()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = Phase41VmReadScalarDeliveryDecisionSpecV2::getInstance();
		if (!spec)
		{
			throw InvalidOperationException(L"Phase 41 SpecV2 is unavailable.");
		}
		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw std::invalid_argument("The scalar-delivery SpecV2 digest drifted from the earlier commit.");
		}

		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> ownerReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, OwnerReviewEvidenceId});

//		VirtualizationDecisionReviewEvidenceV2 architectureReview = ownerReview with { Role = VirtualizationDecisionReviewRoleV2.ArchitectureReviewRole, AuthorityPlane = VirtualizationDecisionReviewAuthorityPlaneV2.Architecture, EvidenceId = ArchitectureReviewEvidenceId};
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> architectureReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId});
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> record = std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VirtualizationDecisionAcceptanceRecordV2{VmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64)});
		record = record->WithAcceptanceDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(record));
		if (record->AcceptanceDigest != ExpectedAcceptanceDigest)
		{
			throw std::invalid_argument("The scalar-delivery AcceptanceRecordV2 canonical digest drifted.");
		}

		return record;
	}

	std::shared_ptr<VirtualizationCodeOwnersRuleV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(const std::wstring &scope)
	{
		return std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{scope, RepositoryPrincipal});
	}
}
