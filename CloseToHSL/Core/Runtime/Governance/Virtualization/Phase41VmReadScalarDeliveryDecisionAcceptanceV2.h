#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VmReadScalarDeliveryDecisionValidationResultV2;

	/// <summary>
	/// Later attributable acceptance of the exact scalar-delivery SpecV2 bytes at
	/// SpecCommitSha. It is governance policy only and never names its own commit.
	/// </summary>
	class Phase41VmReadScalarDeliveryDecisionAcceptanceV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> GetRecord();
		static std::vector<unsigned char> GetCanonicalBytes();
		static std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> GetCodeOwnersEvidence();

	public:
		static inline constexpr wchar_t SpecCommitSha[] = L"bb2125226425eb341fd94cc67cc6bb26abf918fe";
		static inline constexpr wchar_t ExpectedSpecDigest[] = L"ccda8698dbeb3f6eef1b4f13e22a3fb7607e939f493138fb7e3373674e234309";
		static inline constexpr wchar_t ExpectedAcceptanceDigest[] = L"465a887c2918a762af6bf2039f98c294b2f601b1bb7b71512df3f95a9141b5d5";
		static inline constexpr wchar_t CodeOwnersBlobSha[] = L"6a9aef1c554340818860242cdb571b4278b5735c";
		static inline constexpr wchar_t RepositoryPrincipal[] = L"@yaksysdev";
		static inline constexpr wchar_t OwnerReviewEvidenceId[] = L"PHASE41-SCALAR-DELIVERY-OWNER-REVIEW-2026-08-11-BB21252";
		static inline constexpr wchar_t ArchitectureReviewEvidenceId[] = L"PHASE41-SCALAR-DELIVERY-ARCH-REVIEW-2026-08-11-BB21252";

		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> getRecord();

		static std::vector<unsigned char> getCanonicalBytes();

		static std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> getCodeOwnersEvidence();

		static bool getRuntimeAuthorityGranted();
		static bool getSourceValueAvailable();
		static bool getResultReceiptIssued();
		static bool getRegisterWritebackAuthorized();
		static bool getRetireCommitAuthorized();
		static bool getBackendExecutionAuthorized();
		static bool getUnderlyingVirtualizationMutationAuthorized();
		static bool getCompletionPublicationAuthorized();

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha);

	private:
		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> CreateRecord();

		static std::shared_ptr<VirtualizationCodeOwnersRuleV2> Rule(const std::wstring &scope);
	};

}
