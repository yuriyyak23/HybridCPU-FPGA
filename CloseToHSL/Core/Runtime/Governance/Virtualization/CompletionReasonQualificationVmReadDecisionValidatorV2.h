#pragma once

#include "VirtualizationDecisionContractsV2.h"
#include "VmReadScalarDeliveryDecisionValidatorV2.h"
#include <string>
#include <optional>
#include <memory>
#include <vector>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{

	class CompletionReasonQualificationVmReadDecisionValidatorV2 final
	{
	private:
		static const std::vector<std::wstring> &GetRequiredCodeOwnersScopes();
	public:
		static constexpr unsigned int SchemaVersion = 2;
		static inline constexpr wchar_t ExpectedDecisionId[] = L"D2-HV-VMREAD-COMPLETION-REASON-QUALIFICATION-0004";
		static inline constexpr wchar_t ExpectedOperationNamespace[] = L"HybridCPU.VMREAD.ScalarDelivery.v1";
		static inline constexpr wchar_t ExpectedOperationId[] = L"DELIVER_COMPLETION_REASON_QUALIFICATION_SCALAR_V1";
		static constexpr unsigned long long ExpectedSourceOwnerId = 0x434F'4D50'4F42'5356ULL;
		static inline constexpr wchar_t ExpectedDependencyContract[] = L"Source=DomainCompletionObservationOwner.ExactCommittedSnapshot;Producer=CanonicalCpuInstructionTranslationFaultProducer;Reasons=AccessDenied+OwnerScopeMismatch;ReasonMapping=FrozenSecurityPolicyViolation;Qualification=ExactReasonAccessKindAccessSizeEncodingUnchanged;PairBinding=Required;Fields=ExitReason+ExitQualification;Denied=GuestPhysicalAddress+EptViolationQualification;Freshness=CompletionGeneration+RestoreGeneration+OwnerScope+Attempt+Event+Digest;Transport=ExistingE1CarrierScalarReceiptCanonicalPRFRenameWritebackRetire;Activation=DefaultDisabled;NoInferenceNoZeroFallbackNoIommuNoNestedNoGenericExceptionNoVmcsBackingNoCompletionCreationNoVmxAuthority";

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> ValidateSpecShape(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec);

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Validate(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec, std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance, std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence);

	private:
		static bool ValidateProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec);

		static bool ValidateOwnerMap(const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &map);

		static bool Review(std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review, VirtualizationDecisionReviewRoleV2 role, VirtualizationDecisionReviewAuthorityPlaneV2 plane, std::shared_ptr<VirtualizationDecisionSpecV2> spec, std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool FixedEquals(string? left, string? right)
		static bool FixedEquals(const std::wstring &left, const std::wstring &right);
		static bool FixedEquals(const std::vector<unsigned char> &left, const std::vector<unsigned char> &right);

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Deny(VmReadScalarDeliveryDecisionValidationDecisionV2 decision, const std::wstring &reason);
	};

}
