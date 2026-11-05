#pragma once

#include "VirtualizationDecisionContractsV2.h"
#include <string>
#include <stdexcept>
#include <optional>
#include <memory>
#include <utility>
#include <vector>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmReadProjectionDecisionValidationDecisionV2 : unsigned char
	{
		AcceptedPolicyObject = 0,
		DeniedMissingArtifact = 1,
		DeniedCanonicalArtifact = 2,
		DeniedProvenance = 3,
		DeniedIdentity = 4,
		DeniedProfile = 5,
		DeniedOwnerMap = 6,
		DeniedAcceptance = 7,
		DeniedReview = 8,
		DeniedCodeOwners = 9,
		DeniedLineage = 10
	};

	struct VmReadProjectionDecisionValidationResultV2 final
	{
		VmReadProjectionDecisionValidationDecisionV2 Decision;
		std::wstring Reason;
		std::shared_ptr<AcceptedVmReadProjectionDecisionV2> AcceptedDecision;
		bool IsAcceptedPolicyObject;
		bool RuntimeCapabilityGranted = false;
		bool ProjectionValueAvailable = false;
		bool BackendExecutionAuthorized = false;
		bool MutationAuthorized = false;
		bool CompletionPublicationAuthorized = false;
		bool RetirePublicationAuthorized = false;

		VmReadProjectionDecisionValidationResultV2(
			VmReadProjectionDecisionValidationDecisionV2 decision,
			std::wstring reason,
			std::shared_ptr<AcceptedVmReadProjectionDecisionV2> acceptedDecision = nullptr)
			: Decision(decision), Reason(std::move(reason)), AcceptedDecision(std::move(acceptedDecision)),
			  IsAcceptedPolicyObject(decision == VmReadProjectionDecisionValidationDecisionV2::AcceptedPolicyObject && AcceptedDecision != nullptr)
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VmReadProjectionDecisionValidationResultV2(VmReadProjectionDecisionValidationDecisionV2 Decision, string Reason, System.Nullable<AcceptedVmReadProjectionDecisionV2> AcceptedDecision)
//	{
//		internal bool IsAcceptedPolicyObject
//		{
//			get
//			{
//				return Decision == VmReadProjectionDecisionValidationDecisionV2.AcceptedPolicyObject && AcceptedDecision != nullptr;
//			}
//		}
//
//		internal bool RuntimeCapabilityGranted
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool ProjectionValueAvailable
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool BackendExecutionAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool MutationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool CompletionPublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RetirePublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

	/// <summary>
	/// Fail-closed governance validator for one exact read-only VMREAD profile.
	/// It is intentionally separate from the Phase 38 VMCALL validator and exposes
	/// no runtime lookup, projection or admission operation.
	/// </summary>
	class VmReadProjectionDecisionValidatorV2 final
	{
	public:
		static inline constexpr wchar_t ExpectedDecisionId[] = L"D2-HV-VMREAD-PROJECTION-V1-GUEST-CR0-CR4-0001";
		static inline constexpr wchar_t ExpectedOperationNamespace[] = L"HybridCPU.VMREAD.Projection.v1";
		static inline constexpr wchar_t ExpectedOperationId[] = L"READ_GUEST_CR0_CR4_V1";
		static constexpr unsigned long long ExpectedOwnerId = 0x0048'4350'4553'524FULL; // HCPESRO

	private:
		static const std::vector<std::wstring> &GetRequiredCodeOwnersScopes();

	public:
		static std::shared_ptr<VmReadProjectionDecisionValidationResultV2> Validate(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec, std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance, std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence);

	private:
		static bool ValidateProvenance(std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance, std::shared_ptr<VirtualizationDecisionValidationEvidenceV2> evidence);

		static bool ValidateExactProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec);

		static bool ValidateOwnerMap(const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &ownerMap);

		static bool ValidateAcceptance(std::shared_ptr<VirtualizationDecisionSpecV2> spec, std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance);

		static bool ValidateReviews(std::shared_ptr<VirtualizationDecisionSpecV2> spec, std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance);

		static bool ValidateReview(std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review, VirtualizationDecisionReviewRoleV2 role, VirtualizationDecisionReviewAuthorityPlaneV2 plane, std::shared_ptr<VirtualizationDecisionSpecV2> spec, std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance);

		static bool ValidateCodeOwners(std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance, std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> codeOwners);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool FixedEquals(string? left, string? right)
		static bool FixedEquals(const std::wstring &left, const std::wstring &right);

		static bool FixedEquals(const std::vector<unsigned char> &left, const std::vector<unsigned char> &right);

		static std::shared_ptr<VmReadProjectionDecisionValidationResultV2> Deny(VmReadProjectionDecisionValidationDecisionV2 decision, const std::wstring &reason);
	};

}
