#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VmReadScalarDeliveryDecisionValidationResultV2;

	class Phase55CompletionReasonQualificationVmReadDecisionAcceptanceV2 final
	{
	public:
		static inline constexpr wchar_t SpecCommitSha[] = L"9497d6152bee14e3743e99edc6c4c451b869ee8a";
		static inline constexpr wchar_t SpecTreeSha[] = L"60d9d11f8ef783466d7d05e03a52ccdb43ecb007";
		static inline constexpr wchar_t ExpectedSpecDigest[] = L"649f576e09ca0168ba60be3f632a4d8c93f6fabaa8571c22a5445f438dcda6d2";
		static inline constexpr wchar_t ExpectedAcceptanceDigest[] = L"4954b25a5e3c91e2d4c54f0d4e75f83c87e495773889b94e4fd6e98f5617f5be";
		static inline constexpr wchar_t CodeOwnersBlobSha[] = L"5ee888ca6312af05ac2780bdf288a3d3506d1a63";
		static inline constexpr wchar_t RepositoryPrincipal[] = L"@yaksysdev";
		static inline constexpr wchar_t ArchitectureReviewEvidenceId[] = L"PHASE55-ARCHITECTURE-REVIEW-2026-08-13-9497D61";

	private:
		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> CreateRecord();
		static std::shared_ptr<VirtualizationCodeOwnersRuleV2> Rule(const std::wstring &scope);
		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> GetRecord();
		static std::vector<unsigned char> GetCanonicalBytes();
		static std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> GetCodeOwnersEvidence();

	public:

		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> getRecord();
		static std::vector<unsigned char> getCanonicalBytes();
		static std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> getCodeOwnersEvidence();

		static bool getRuntimeAuthorityGranted();
		static bool getCompletionCreationAuthorized();
		static bool getProductionCompositionAuthorized();
		static bool getGuestPhysicalAddressAuthorized();
		static bool getEptViolationQualificationAuthorized();

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha);

	};

}
