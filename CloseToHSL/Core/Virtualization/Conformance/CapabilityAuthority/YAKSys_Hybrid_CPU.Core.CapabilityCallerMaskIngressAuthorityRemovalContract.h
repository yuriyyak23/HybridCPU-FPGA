#pragma once

#include "../../../Runtime/Capabilities/CapabilityBoundaryRequirement.h"
#include "../../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "CapabilityCallerMaskIngressAuthorityRemovalContract.h"
#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"
#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityCallerMaskIngressAuthorityRemovalContract final : public std::enable_shared_from_this<CapabilityCallerMaskIngressAuthorityRemovalContract>
	{
	public:
		static inline const std::wstring CapabilityBoundaryRequirementPath = L"Core/Runtime/Capabilities/CapabilityBoundaryRequirement.cs";

		static inline const std::wstring RuntimeBoundaryAdmissionPath = L"Core/Runtime/Services/RuntimeBoundaryAdmissionService.cs";

		static inline const std::wstring DomainRuntimeContextPath = L"Core/Runtime/Domains/Services/DomainRuntimeContext.cs";

		static inline const std::wstring DomainValidationResultPath = L"Core/Runtime/Domains/Validation/DomainValidationResult.cs";

		static inline const std::wstring DomainRuntimeAuthorityPath = L"Core/Runtime/Domains/Authority/DomainRuntimeAuthority.cs";

		static inline const std::wstring CapabilityPublicationPolicyPath = L"Core/Runtime/Capabilities/Publication/CapabilityPublicationPolicy.cs";

		static inline const std::wstring NestedDomainControllerPath = L"Core/VMX/Compatibility/Frontend/Projection/Nested/NestedDomainController.cs";

		static inline const std::vector<std::wstring> AdmissionAuthorityPaths = {RuntimeBoundaryAdmissionPath, DomainRuntimeContextPath, DomainValidationResultPath, DomainRuntimeAuthorityPath};

		static inline const std::vector<std::wstring> ForbiddenAdmissionFallbackMarkers = {L"HasEffectiveCapability(", L"CompatibilityCapsProjection", L"EffectiveCaps"};

		static inline const std::vector<std::wstring> ForbiddenNestedCallerPressureMarkers = {L"PublishedVmxCaps", L"PublishedCapabilityWord", L"FromCompatibilityMasks("};

		static inline const std::vector<std::wstring> RequiredNestedTypedProjectionMarkers = {L"CompatibilityProjectionWord", L"FromTypedDescriptorProofs", L"FromTypedGrant(OwnerDomainId)"};

		CapabilityCallerMaskIngressAuthorityViolation Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId);

		bool IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId);
	};
}
