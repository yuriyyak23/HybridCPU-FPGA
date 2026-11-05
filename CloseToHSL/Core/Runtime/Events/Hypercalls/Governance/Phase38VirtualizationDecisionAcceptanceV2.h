#pragma once

#include "../../../Governance/Virtualization/VirtualizationDecisionCanonicalEncoderV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"
#include <string>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Attributable acceptance of the exact SpecV2 bytes already present in commit A.
	/// The record intentionally does not contain the SHA of its own containing commit.
	/// </summary>
	class Phase38VirtualizationDecisionAcceptanceV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> CreateRecord();

	public:
		static inline const std::wstring SpecCommitSha = L"1061eaa8bc45d598e1fe7b3fead71cf017ad81a6";
		static inline const std::wstring ExpectedSpecDigest = L"33076e430fcbc05cf0774d08baadc6d7840f88029fcfb28a458558af82f93ca8";
		static inline const std::wstring ExpectedAcceptanceDigest = L"b928ff6254139b9fc0cf9412e192db2f68b1918f01646bea65c85e33791e33e3";
		static inline const std::wstring CodeOwnersBlobSha = L"aafa6f65565345e18621eea0cc890711f64c3dbb";
		static inline const std::wstring RepositoryPrincipal = L"@yaksysdev";
		static inline const std::wstring OwnerReviewEvidenceId = L"PRB-OWNER-REVIEW-2026-08-09-1061EAA8";
		static inline const std::wstring ArchitectureReviewEvidenceId = L"PRB-ARCH-REVIEW-2026-08-09-1061EAA8";

		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> getRecord();

		static std::vector<unsigned char> getCanonicalBytes();

		static std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> getCodeOwnersEvidence();

		static bool getRuntimeAuthorityGranted();
		static bool getBackendExecutionAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getRetirePublicationAuthorized();

		static std::shared_ptr<VirtualizationDecisionValidationResultV2> ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha);

	private:
		static inline const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> CodeOwnersEvidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(true, CodeOwnersBlobSha, std::vector<std::shared_ptr<VirtualizationCodeOwnersRuleV2>>{std::make_shared<VirtualizationCodeOwnersRuleV2>(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Events/Hypercalls/", RepositoryPrincipal), std::make_shared<VirtualizationCodeOwnersRuleV2>(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Capabilities/", RepositoryPrincipal), std::make_shared<VirtualizationCodeOwnersRuleV2>(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Safety/", RepositoryPrincipal), std::make_shared<VirtualizationCodeOwnersRuleV2>(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Completion/", RepositoryPrincipal), std::make_shared<VirtualizationCodeOwnersRuleV2>(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/", RepositoryPrincipal), std::make_shared<VirtualizationCodeOwnersRuleV2>(L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/", RepositoryPrincipal)});
	};

}
