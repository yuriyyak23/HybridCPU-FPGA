#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include "VmReadProjectionDecisionValidatorV2.h"
#include <string>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Later attributable acceptance of the exact VMREAD SpecV2 bytes already
	/// present at SpecCommitSha. The record never names its containing commit and
	/// has no runtime projection, capability, admission or execution API.
	/// </summary>
	class Phase40VmReadProjectionDecisionAcceptanceV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> CreateRecord();
		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> GetRecord();
		static std::vector<unsigned char> GetCanonicalBytes();
		static std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> GetCodeOwnersEvidence();

	public:
		static inline constexpr wchar_t SpecCommitSha[] = L"4d3b5b97c22661652c94357319e2e6b16615cceb";
		static inline constexpr wchar_t ExpectedSpecDigest[] = L"52ce040b93f54b36a427c4269f2afff77b2e66f83ceda3ece1b1dc917a58241f";
		static inline constexpr wchar_t ExpectedAcceptanceDigest[] = L"cf99799baba3ce6595fef61b2f53a5ec1a8e1c144d0bccd29df8171f603c34d8";
		static inline constexpr wchar_t CodeOwnersBlobSha[] = L"f659868c26110e7c0f82778be6dcd3f4f59f43b7";
		static inline constexpr wchar_t RepositoryPrincipal[] = L"@yaksysdev";
		static inline constexpr wchar_t OwnerReviewEvidenceId[] = L"PHASE40-OWNER-REVIEW-2026-08-11-4D3B5B9";
		static inline constexpr wchar_t ArchitectureReviewEvidenceId[] = L"PHASE40-ARCH-REVIEW-2026-08-11-4D3B5B9";

		static std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> getRecord();

		static std::vector<unsigned char> getCanonicalBytes();

		static std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> getCodeOwnersEvidence();

		static bool getRuntimeAuthorityGranted();
		static bool getProjectionValueAvailable();
		static bool getCapabilityGranted();
		static bool getBackendExecutionAuthorized();
		static bool getMutationAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getRetirePublicationAuthorized();

		static std::shared_ptr<VmReadProjectionDecisionValidationResultV2> ValidateRepositoryArtifact(const std::wstring &acceptanceContainingCommitSha);

	};

}
