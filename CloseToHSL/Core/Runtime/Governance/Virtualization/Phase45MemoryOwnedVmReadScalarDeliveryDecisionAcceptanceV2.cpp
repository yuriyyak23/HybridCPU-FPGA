#include "Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2.h"
#include "MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2.h"
#include "Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2.h"
#include "VirtualizationDecisionContractsV2.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::GetRecord()
	{
		static const auto record = CreateRecord();
		return record;
	}

	std::vector<unsigned char> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(GetRecord());
		return bytes;
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::GetCodeOwnersEvidence()
	{
		static const auto evidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(VirtualizationCodeOwnersEvidenceV2{true, CodeOwnersBlobSha, std::vector<std::shared_ptr<VirtualizationCodeOwnersRuleV2>>{
		Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/"),
		Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/Descriptors/MemoryDomain/"),
		Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/Admission/Memory/"),
		Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Memory/Translation/"),
		Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/"),
		Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/"),
		Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/")
	}});
		return evidence;
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getRecord()
	{
		return GetRecord();
	}

	std::vector<unsigned char> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getCodeOwnersEvidence()
	{
		return GetCodeOwnersEvidence();
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getUnderlyingMemoryMutationAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::getProductionCompositionAuthorized()
	{
		return false;
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha)
	{
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> spec = Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getInstance();
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> record = getRecord();
		const auto specBytes = Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes();
		const auto acceptanceBytes = getCanonicalBytes();
		const auto specBytesAtCommit = specBytes;
		const auto codeOwners = getCodeOwnersEvidence();
		const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>> allocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>> revocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>> supersessions{};
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(specBytes, acceptanceBytes, specBytesAtCommit, SpecCommitSha, acceptanceContainingCommitSha, codeOwners, allocations, revocations, supersessions);
		return MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::Validate(spec, record, evidence);
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::CreateRecord()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getInstance();
		if (!spec)
		{
			throw InvalidOperationException("Phase 45 SpecV2 is unavailable.");
		}
		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw std::invalid_argument("Phase 45 SpecV2 drifted from its immutable subject commit.");
		}
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> ownerReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, L"PHASE45-MEMORY-DOMAIN-OWNER-REVIEW-2026-08-12-4299D74"});

//		VirtualizationDecisionReviewEvidenceV2 architectureReview = ownerReview with { Role = VirtualizationDecisionReviewRoleV2.ArchitectureReviewRole, AuthorityPlane = VirtualizationDecisionReviewAuthorityPlaneV2.Architecture, EvidenceId = "PHASE45-ARCHITECTURE-REVIEW-2026-08-12-4299D74"};
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> architectureReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId});
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> record = std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VirtualizationDecisionAcceptanceRecordV2{MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64)});
		record = record->WithAcceptanceDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(record));
		if (record->AcceptanceDigest != ExpectedAcceptanceDigest)
		{
			throw InvalidOperationException(std::format(L"Phase 45 AcceptanceRecordV2 digest is {0:s}.", record->AcceptanceDigest));
		}
		return record;
	}

	std::shared_ptr<VirtualizationCodeOwnersRuleV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::Rule(const std::wstring &scope)
	{
		return std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{scope, RepositoryPrincipal});
	}
}


