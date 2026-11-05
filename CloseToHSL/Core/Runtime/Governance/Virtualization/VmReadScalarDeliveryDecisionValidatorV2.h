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

	enum class VmReadScalarDeliveryDecisionValidationDecisionV2 : unsigned char
	{
		ExactPolicyShape = 0,
		DeniedMissingSpec = 1,
		DeniedIdentity = 2,
		DeniedProfile = 3,
		DeniedOwnerMap = 4,
		DeniedCanonicalDigest = 5,
		AcceptedPolicyObject = 6,
		DeniedMissingArtifact = 7,
		DeniedProvenance = 8,
		DeniedAcceptance = 9,
		DeniedReview = 10,
		DeniedCodeOwners = 11,
		DeniedLineage = 12
	};

	struct VmReadScalarDeliveryDecisionValidationResultV2 final
	{
		VmReadScalarDeliveryDecisionValidationDecisionV2 Decision;
		std::wstring Reason;
		std::shared_ptr<AcceptedVmReadScalarDeliveryDecisionV2> AcceptedDecision;
		bool IsExactPolicyShape;
		bool IsAcceptedPolicyObject;
		bool RuntimeAuthorityGranted = false;
		bool SourceValueAvailable = false;
		bool ResultReceiptIssued = false;
		bool RegisterWritebackAuthorized = false;
		bool RetireCommitAuthorized = false;
		bool BackendExecutionAuthorized = false;
		bool UnderlyingVirtualizationMutationAuthorized = false;
		bool CompletionPublicationAuthorized = false;

		VmReadScalarDeliveryDecisionValidationResultV2(
			VmReadScalarDeliveryDecisionValidationDecisionV2 decision,
			std::wstring reason,
			std::shared_ptr<AcceptedVmReadScalarDeliveryDecisionV2> acceptedDecision = nullptr)
			: Decision(decision), Reason(std::move(reason)), AcceptedDecision(std::move(acceptedDecision)),
			  IsExactPolicyShape(decision == VmReadScalarDeliveryDecisionValidationDecisionV2::ExactPolicyShape),
			  IsAcceptedPolicyObject(decision == VmReadScalarDeliveryDecisionValidationDecisionV2::AcceptedPolicyObject && AcceptedDecision != nullptr)
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VmReadScalarDeliveryDecisionValidationResultV2(VmReadScalarDeliveryDecisionValidationDecisionV2 Decision, string Reason, System.Nullable<AcceptedVmReadScalarDeliveryDecisionV2> AcceptedDecision = nullptr)
//	{
//		internal bool IsExactPolicyShape
//		{
//			get
//			{
//				return Decision == VmReadScalarDeliveryDecisionValidationDecisionV2.ExactPolicyShape;
//			}
//		}
//
//		internal bool IsAcceptedPolicyObject
//		{
//			get
//			{
//				return Decision == VmReadScalarDeliveryDecisionValidationDecisionV2.AcceptedPolicyObject && AcceptedDecision != nullptr;
//			}
//		}
//
//		internal bool RuntimeAuthorityGranted
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool SourceValueAvailable
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool ResultReceiptIssued
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RegisterWritebackAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RetireCommitAuthorized
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
//		internal bool UnderlyingVirtualizationMutationAuthorized
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
//	}

	/// <summary>
	/// Validates the immutable policy shape before a later attributable acceptance
	/// exists. A positive result is not accepted policy and has no runtime effect.
	/// </summary>
	class VmReadScalarDeliveryDecisionValidatorV2 final
	{
	public:
		static constexpr unsigned int SchemaVersion = 2;
		static inline const std::wstring ExpectedDecisionId = L"D2-HV-VMREAD-SCALAR-DELIVERY-V1-GUEST-CR0-CR4-0001";
		static inline const std::wstring ExpectedOperationNamespace = L"HybridCPU.VMREAD.ScalarDelivery.v1";
		static inline const std::wstring ExpectedOperationId = L"DELIVER_GUEST_CR0_CR4_SCALAR_V1";
		static constexpr unsigned long long ExpectedSourceOwnerId = 0x0048'4350'4553'524FULL;
		static inline const std::wstring ExpectedDependencyContract = L"RequiresAcceptedD2-HV-VMREAD-PROJECTION-V1-GUEST-CR0-CR4-0001@SpecDigest=52ce040b93f54b36a427c4269f2afff77b2e66f83ceda3ece1b1dc917a58241f@AcceptanceDigest=cf99799baba3ce6595fef61b2f53a5ec1a8e1c144d0bccd29df8171f603c34d8;OpaqueVmReadScalarResultReceipt=SingleUseAttemptReplayBundleDomainAddressSpaceDescriptorEpochFieldDestinationBound;Transport=CanonicalPRFRenameEXMEMWB;Commit=WBLocalRetireRecord+RetireCoordinator;NoVMCALLE5E6NoTrapCompletionNoDirectArchitecturalWriteNoVMCSWriteback";

	private:
		static inline const std::vector<std::wstring> RequiredCodeOwnersScopes = {L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/", L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Descriptors/ExecutionState/", L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Policies/ExecutionState/", L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/SecureCompute/Compatibility/Projection/", L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/", L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/", L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/ExecutionFlow/", L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/MicroOps/Types/", L"/HybridCPU_ISE/CloseToHSL/Core/Architecture/Registers/", L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/"};

	public:
		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> ValidateSpecShape(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec);

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Validate(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec, std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance, std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence);

	private:
		static bool ValidateExactProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec);

		static bool ValidateOwnerMap(const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &ownerMap);

		static bool ValidateReview(std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review, VirtualizationDecisionReviewRoleV2 role, VirtualizationDecisionReviewAuthorityPlaneV2 plane, std::shared_ptr<VirtualizationDecisionSpecV2> spec, std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool FixedEquals(string? left, string? right)
		static bool FixedEquals(const std::wstring &left, const std::wstring &right);

		static bool FixedEquals(const std::vector<unsigned char> &left, const std::vector<unsigned char> &right);

		static std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> Deny(VmReadScalarDeliveryDecisionValidationDecisionV2 decision, const std::wstring &reason);
	};

}
