#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityRuntimeSubstrateExtractionContract final : public std::enable_shared_from_this<CapabilityRuntimeSubstrateExtractionContract>
	{
	private:
		static inline const std::vector<std::wstring> RemovedCapabilitySubstratePaths = {L"Core/VMX/Substrate/Descriptors/Capability/CapabilityDescriptorSet.cs", L"Core/VMX/Substrate/Capabilities/Grants/CapabilityGrant.cs", L"Core/VMX/Substrate/Capabilities/Negotiation/CapabilityNegotiationService.cs", L"Core/VMX/Substrate/Capabilities/Publication/CapabilityPublicationPolicy.cs"};
		static inline const std::vector<std::wstring> NeutralCapabilityRuntimePaths = {L"Core/Runtime/Capabilities/Descriptors/CapabilityDescriptorSet.cs", L"Core/Runtime/Capabilities/Grants/CapabilityGrant.cs", L"Core/Runtime/Capabilities/Negotiation/CapabilityNegotiationService.cs", L"Core/Runtime/Capabilities/Publication/CapabilityPublicationPolicy.cs"};
		static inline const std::vector<std::wstring> ForbiddenNeutralCapabilityRuntimeMarkers = {L"Vmcs", L"VMCS", L"Vmx", L"VMX", L"CapabilityDescriptorSetSchema", L"FromCompatibilityMasks(", L"KnownVmxV2CompatibilityMask", L"VmxCompatibility", L"VmxCompatibilityBits", L"VmxV2InstructionCaps", L"PublishedVmxCaps", L"PublishedCapabilityWord"};
		static inline const std::vector<std::wstring> RequiredNeutralCapabilityRuntimeMarkers = {L"CapabilityGrantCollection", L"CapabilityDescriptorSet", L"CapabilityNegotiationService", L"CapabilityPublicationPolicy", L"TypedGrants"};
		static inline const std::vector<std::wstring> RequiredCompatibilityProjectionMarkers = {L"CapabilityGrantCollection.FromCompatibilityMasks", L"CapabilityDescriptorSetSchema.VmxCompatibility", L"CapabilityDescriptorSetSchema.VmxCompatibilityBits", L"CreateInternalGrant"};
		static inline const std::vector<std::wstring> RemovedProjectFolderMarkers = {L"Core\\VMX\\Substrate\\Capabilities\\Grants\\", L"Core\\VMX\\Substrate\\Capabilities\\Negotiation\\", L"Core\\VMX\\Substrate\\Capabilities\\Publication\\", L"Core\\VMX\\Substrate\\Descriptors\\Capability\\"};

	public:
		static std::vector<std::wstring> getRemovedCapabilitySubstratePaths();

		static std::vector<std::wstring> getNeutralCapabilityRuntimePaths();

		static inline const std::wstring CompatibilityCapabilityProjectionPath = L"Core/VMX/Compatibility/Frontend/Projection/CapabilityCompatibilityProjection.cs";

		static std::vector<std::wstring> getForbiddenNeutralCapabilityRuntimeMarkers();

		static std::vector<std::wstring> getRequiredNeutralCapabilityRuntimeMarkers();

		static std::vector<std::wstring> getRequiredCompatibilityProjectionMarkers();

		static std::vector<std::wstring> getRemovedProjectFolderMarkers();

		bool getMovesTypedCapabilityRuntimeOutOfVmxSubstrate() const;

		bool getKeepsMaskIngressCompatibilityProjectionOnly() const;

		bool getDoesNotCreateVmxCapabilityOwner() const;

		bool IsSatisfied();
	};
}
