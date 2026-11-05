#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VmReadScalarDeliveryPolicyLookup(string DecisionId, string OperationNamespace, string OperationId, ImmutableArray<ushort> FieldIds, string SourceOwner, VirtualizationDecisionResultAbiV2 ResultAbi, VirtualizationDecisionEffectClassV2 EffectClass, VirtualizationCapabilityRequirementV2 CapabilityRequirement, VirtualizationOperationMigrationPolicyV2 MigrationPolicy, bool Revoked);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VmReadScalarDeliveryPolicyResolution(System.Nullable<AcceptedVmReadScalarDeliveryDecisionV2> Policy, string Reason)
//	{
//		internal bool IsResolved
//		{
//			get
//			{
//				return Policy != nullptr;
//			}
//		}
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
//	}

    struct AcceptedVmReadScalarDeliveryDecisionV2;
    enum class VirtualizationDecisionResultAbiV2 : unsigned char;
    enum class VirtualizationDecisionEffectClassV2 : unsigned char;
    enum class VirtualizationCapabilityRequirementV2 : unsigned char;
    enum class VirtualizationOperationMigrationPolicyV2 : unsigned char;
    struct VmReadScalarDeliveryPolicyLookup final {
        std::wstring DecisionId, OperationNamespace, OperationId;
        std::vector<unsigned short> FieldIds;
        std::wstring SourceOwner;
        
        VirtualizationDecisionResultAbiV2 ResultAbi{};
        VirtualizationDecisionEffectClassV2 EffectClass{};
        VirtualizationCapabilityRequirementV2 CapabilityRequirement{};
        VirtualizationOperationMigrationPolicyV2 MigrationPolicy{};
        bool Revoked = false;
    };
    struct VmReadScalarDeliveryPolicyResolution final {
        std::shared_ptr<AcceptedVmReadScalarDeliveryDecisionV2> Policy;
        std::wstring Reason;
        bool isResolved() const noexcept { return Policy != nullptr; }
        bool getRuntimeAuthorityGranted() const noexcept { return false; }
        bool getSourceValueAvailable() const noexcept { return false; }
        bool getResultReceiptIssued() const noexcept { return false; }
    };
	class VmReadScalarDeliveryAcceptedPolicyResolver final
	{
	public:
		static inline const std::wstring AcceptanceContainingCommitSha = L"44fc1c3daf7d9359ffb5e4b00ccfa35953670515";

		static std::shared_ptr<VmReadScalarDeliveryPolicyResolution> Resolve(std::shared_ptr<VmReadScalarDeliveryPolicyLookup> lookup);

		static std::shared_ptr<VmReadScalarDeliveryPolicyLookup> ExactLookup(bool revoked = false);
	};

}
