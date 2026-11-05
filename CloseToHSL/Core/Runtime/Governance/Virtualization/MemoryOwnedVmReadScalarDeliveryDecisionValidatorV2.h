#pragma once

#include "VirtualizationDecisionContractsV2.h"
#include "VmReadScalarDeliveryDecisionValidatorV2.h"
#include <string>
#include <vector>
#include <optional>
#include <format>
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{

	class MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2 final
	{
	public:
		static constexpr unsigned int SchemaVersion = 2;
		static inline const std::wstring ExpectedDecisionId = L"D2-HV-VMREAD-SCALAR-DELIVERY-V1-GUEST-CR3-EPTP-VPID-CR3TC-0003";
		static inline const std::wstring ExpectedOperationNamespace = L"HybridCPU.VMREAD.ScalarDelivery.v1";
		static inline const std::wstring ExpectedOperationId = L"DELIVER_GUEST_CR3_EPTP_VPID_CR3TC_SCALAR_V1";
		static constexpr unsigned long long ExpectedSourceOwnerId = 0x004D'454D'4144'4452ULL;
		static inline const std::wstring ExpectedDependencyContract = L"CanonicalIngress=VmxCompatibilityAdmissionService.AdmitVmReadProjection;RuntimeBoundary=ReadCompatibilityProjection+FullDomainRuntimeUnchanged;AuthorityPlane=MemoryAddressSpaceReadProjection;Source=MemoryDomainDescriptor.MaterializedMemoryDomainReadOnlyTranslationView;Freshness=CanonicalMemoryDomainRuntimeNonZeroCurrentAddressSpaceGenerationAtomicOwnerValueDomainAddressSpaceGenerationFieldCapture;Fields=GuestCr3AddressSpaceRoot+EptPointerOwnedValidSecondStageRoot+VpidEnabledNonZeroTag+Cr3TargetCountCanonicalBound;Capability=None;Evidence=GuestArchitecturalStateOrCompatibilityAlias+MaterializedFieldSourceGenerationProof;Result=ScalarU64ToDestinationRegister;Effect=ArchitecturalRegisterResultOnly;Transport=ExistingScalarReceiptCanonicalPRFRenameWriteback;Commit=RetireRecord.RegisterWrite+RetireCoordinator;Migration=DrainOnlyNoReceiptSealPhysicalDestinationOrOutputSerialization;Activation=DefaultDisabled;NoProductionCompositionNoVMCSFallbackNoVMCALLReuseNoVmxRetireEffectNoBackendCompletionNoDirectArchitecturalWrite";

	private:
		static inline const std::vector<std::wstring> RequiredCodeOwnersScopes = {
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/Descriptors/MemoryDomain/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/Admission/Memory/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Memory/Translation/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/",
			L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/"};

	public:
		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> ValidateSpecShape(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec);

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Validate(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec, std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance, std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence);

	private:
		static bool ValidateExactProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec);

		static bool ValidateOwnerMap(const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &map);

		static bool Review(std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review, VirtualizationDecisionReviewRoleV2 role, VirtualizationDecisionReviewAuthorityPlaneV2 plane, std::shared_ptr<VirtualizationDecisionSpecV2> spec, std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool FixedEquals(string? left, string? right)
		static bool FixedEquals(const std::wstring &left, const std::wstring &right);
		static bool FixedEquals(const std::vector<unsigned char> &left, const std::vector<unsigned char> &right);
		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Deny(VmReadScalarDeliveryDecisionValidationDecisionV2 decision, const std::wstring &reason);
	};

}
