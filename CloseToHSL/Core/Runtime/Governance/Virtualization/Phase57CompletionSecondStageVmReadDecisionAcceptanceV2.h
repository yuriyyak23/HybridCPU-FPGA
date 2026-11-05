#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VmReadScalarDeliveryDecisionValidationResultV2;

	class Phase57CompletionSecondStageVmReadDecisionAcceptanceV2 final
	{
	public:
		static inline constexpr wchar_t SpecCommitSha[] = L"0865929ae2ab5ee95b0912eebd63ae7af2494f4c";
		static inline constexpr wchar_t SpecTreeSha[] = L"a0108c9e035ff9c57c0167a6efd04788535a1ebd";
		static inline constexpr wchar_t ExpectedSpecDigest[] = L"c68adf194f0452c15f4d11e3a44199b50a0a6168923915e280c62fcbe683c4ab";
		static inline constexpr wchar_t ExpectedAcceptanceDigest[] = L"a6622a79ff3278227a9a39b4e51b20345366ffba24d47cb107b53a0301d3e955";
		static inline constexpr wchar_t CodeOwnersBlobSha[] = L"5ee888ca6312af05ac2780bdf288a3d3506d1a63";
		static inline constexpr wchar_t RepositoryPrincipal[] = L"@yaksysdev";
		static inline constexpr wchar_t ArchitectureReviewEvidenceId[] = L"PHASE57-ARCHITECTURE-REVIEW-2026-08-13-0865929";

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

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha);

	};

}
