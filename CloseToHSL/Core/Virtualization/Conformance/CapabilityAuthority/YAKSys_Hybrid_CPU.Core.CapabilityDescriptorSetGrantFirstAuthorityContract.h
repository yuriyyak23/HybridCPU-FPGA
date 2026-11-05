#pragma once

#include "../../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "CapabilityDescriptorSetGrantFirstAuthorityContract.h"
#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityDescriptorSetGrantFirstAuthorityContract final : public std::enable_shared_from_this<CapabilityDescriptorSetGrantFirstAuthorityContract>
	{
	public:
		static inline const std::wstring DescriptorSetPath = L"Core/Runtime/Capabilities/Descriptors/CapabilityDescriptorSet.cs";

		static inline const std::wstring GrantCollectionPath = L"Core/Runtime/Capabilities/Grants/CapabilityGrant.cs";

		static inline const std::wstring CompatibilityProjectionPath = L"Core/VMX/Compatibility/Frontend/Projection/CapabilityCompatibilityProjection.cs";

		static inline const std::vector<std::wstring> ForbiddenDescriptorBackingMarkers = {L"GlobalHardwareCaps = globalHardwareCaps", L"RuntimeEnabledCaps = runtimeEnabledCaps", L"DomainGrantedCaps = domainGrantedCaps", L"GlobalHardwareCaps &", L"RuntimeEnabledCaps &", L"DomainGrantedCaps;", L"private readonly ulong _global", L"private readonly ulong _runtime", L"private readonly ulong _domain"};

		static inline const std::vector<std::wstring> RequiredGrantFirstMarkers = {L"public CapabilityDescriptorSet(CapabilityGrantCollection typedGrants)", L"TypedGrants.ProjectMask(CapabilityGrantScope.HardwareAvailable)", L"TypedGrants.ProjectMask(CapabilityGrantScope.RuntimeEnabled)", L"TypedGrants.ProjectMask(CapabilityGrantScope.DomainGranted)", L"TypedGrants.EffectiveCompatibilityMask", L"HasTypedGrant(capabilityMask, CapabilityGrantScope.CompatibilityProjection)"};

		static inline const std::vector<std::wstring> ForbiddenGrantCollectionMarkers = {L"FromMasks(", L"FromCompatibilityMasks(", L"CapabilityDescriptorSetSchema", L"VmxCompatibility", L"VmxCompatibilityBits"};

		CapabilityDescriptorSetGrantFirstAuthorityViolation Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask);

		bool IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask);
	};
}
