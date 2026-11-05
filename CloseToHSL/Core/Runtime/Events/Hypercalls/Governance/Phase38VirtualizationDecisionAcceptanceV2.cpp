#include "Phase38VirtualizationDecisionAcceptanceV2.h"
#include "Phase38VirtualizationDecisionSpecV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase38VirtualizationDecisionAcceptanceV2::getRecord()
	{
		static const auto record = CreateRecord();
		return record;
	}

	std::vector<unsigned char> Phase38VirtualizationDecisionAcceptanceV2::getCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(getRecord());
		return bytes;
	}

	std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase38VirtualizationDecisionAcceptanceV2::getCodeOwnersEvidence()
	{
		return CodeOwnersEvidence;
	}

	bool Phase38VirtualizationDecisionAcceptanceV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase38VirtualizationDecisionAcceptanceV2::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase38VirtualizationDecisionAcceptanceV2::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase38VirtualizationDecisionAcceptanceV2::getRetirePublicationAuthorized()
	{
		return false;
	}

	std::shared_ptr<VirtualizationDecisionValidationResultV2> Phase38VirtualizationDecisionAcceptanceV2::ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha)
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = Phase38VirtualizationDecisionSpecV2::getInstance();
		return VirtualizationDecisionValidatorV2::Validate(spec, getRecord(), std::make_shared<VirtualizationDecisionValidationEvidenceV2>(Phase38VirtualizationDecisionSpecV2::getCanonicalBytes(), getCanonicalBytes(), Phase38VirtualizationDecisionSpecV2::getCanonicalBytes(), SpecCommitSha, acceptanceContainingCommitSha, getCodeOwnersEvidence(), std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>>{std::make_shared<VirtualizationNamespaceAllocationV2>(L"HybridCPU.VMFUNC.FrozenAbi.v1", 16, 1, L"FROZEN-VMFUNC-CAPABILITY-QUERY", VirtualizationNamespaceClassV2::FrozenCompatibility)}, std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>>{}));
	}

	std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase38VirtualizationDecisionAcceptanceV2::CreateRecord()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = Phase38VirtualizationDecisionSpecV2::getInstance();
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));
        if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw InvalidOperationException(L"The materialized SpecV2 digest drifted from reviewed commit A.");
		}

		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> ownerReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, OwnerReviewEvidenceId);

//		VirtualizationDecisionReviewEvidenceV2 architectureReview = ownerReview with { Role = VirtualizationDecisionReviewRoleV2.ArchitectureReviewRole, AuthorityPlane = VirtualizationDecisionReviewAuthorityPlaneV2.Architecture, EvidenceId = ArchitectureReviewEvidenceId};
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> architectureReview = std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId);
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> record = std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VirtualizationDecisionValidatorV2::CurrentSchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64));
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		record = record->WithAcceptanceDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(record));
        if (record->AcceptanceDigest != ExpectedAcceptanceDigest)
		{
			throw InvalidOperationException(L"The attributable AcceptanceRecordV2 digest drifted.");
		}

		return record;
	}
}
