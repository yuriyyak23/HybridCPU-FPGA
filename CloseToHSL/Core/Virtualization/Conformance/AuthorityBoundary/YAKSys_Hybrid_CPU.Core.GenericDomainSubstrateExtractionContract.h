#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class GenericDomainSubstrateExtractionContract final : public std::enable_shared_from_this<GenericDomainSubstrateExtractionContract>
	{
	private:
		static inline const std::vector<std::wstring> RemovedGenericDomainSubstratePaths = {L"Core/VMX/Substrate/Descriptors/CompletionRoute/CompletionRouteDescriptor.cs", L"Core/VMX/Substrate/Descriptors/EventQueue/EventQueueDescriptor.cs", L"Core/VMX/Substrate/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.cs", L"Core/VMX/Substrate/Descriptors/ExecutionDomain/BundleLegalityDescriptor.cs", L"Core/VMX/Substrate/Descriptors/ExecutionDomain/ExecutionDomainDescriptor.cs", L"Core/VMX/Substrate/Descriptors/ExecutionDomain/ExecutionExtensionDescriptor.cs", L"Core/VMX/Substrate/Descriptors/ExecutionDomain/SchedulingBudgetDescriptor.cs", L"Core/VMX/Substrate/Descriptors/IoDomain/IoDomainDescriptor.cs", L"Core/VMX/Substrate/Descriptors/IoDomain/IoVirtualizationBlock.cs", L"Core/VMX/Substrate/Descriptors/Lane6Domain/Lane6DomainDescriptor.cs", L"Core/VMX/Substrate/Descriptors/Lane7Accelerator/Lane7AcceleratorDescriptor.cs", L"Core/VMX/Substrate/Descriptors/MemoryDomain/MemoryDomainDescriptor.cs", L"Core/VMX/Substrate/Descriptors/Observability/ObservabilityDescriptor.cs", L"Core/VMX/Substrate/Descriptors/TrapPolicy/TrapPolicyDescriptor.cs", L"Core/VMX/Substrate/Domains/Execution/ExecutionDomainRuntime.cs", L"Core/VMX/Substrate/Domains/IO/IoDomainRuntime.cs", L"Core/VMX/Substrate/Domains/Lane6/Lane6DomainRuntime.cs", L"Core/VMX/Substrate/Domains/Lane7/Lane7DomainRuntime.cs", L"Core/VMX/Substrate/Domains/Memory/MemoryDomainRuntime.cs", L"Core/VMX/Substrate/Domains/Nested/NestedDomainRuntime.cs", L"Core/VMX/Substrate/Domains/VectorStream/VectorStreamDomainRuntime.cs", L"Core/VMX/Substrate/Nested/Descriptors/NestedDomainDescriptor.cs", L"Core/VMX/Substrate/Nested/Projection/INestedProjectionService.cs", L"Core/VMX/Substrate/Nested/Projection/NestedProjectionService.cs", L"Core/VMX/Substrate/Nested/Validation/NestedValidationResult.cs"};
		static inline const std::vector<std::wstring> NeutralRuntimePaths = {L"Core/Runtime/Domains/Descriptors/CompletionRoute/CompletionRouteDescriptor.cs", L"Core/Runtime/Domains/Descriptors/EventQueue/EventQueueDescriptor.cs", L"Core/Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.cs", L"Core/Runtime/Domains/Descriptors/ExecutionDomain/BundleLegalityDescriptor.cs", L"Core/Runtime/Domains/Descriptors/ExecutionDomain/ExecutionDomainDescriptor.cs", L"Core/Runtime/Domains/Descriptors/ExecutionDomain/ExecutionExtensionDescriptor.cs", L"Core/Runtime/Domains/Descriptors/ExecutionDomain/SchedulingBudgetDescriptor.cs", L"Core/Runtime/Domains/Descriptors/IoDomain/IoDomainDescriptor.cs", L"Core/Runtime/Domains/Descriptors/IoDomain/IoVirtualizationBlock.cs", L"Core/Runtime/Domains/Descriptors/Lane6Domain/Lane6DomainDescriptor.cs", L"Core/Runtime/Domains/Descriptors/Lane7Accelerator/Lane7AcceleratorDescriptor.cs", L"Core/Runtime/Domains/Descriptors/MemoryDomain/MemoryDomainDescriptor.cs", L"Core/Runtime/Domains/Descriptors/Observability/ObservabilityDescriptor.cs", L"Core/Runtime/Domains/Descriptors/TrapPolicy/TrapPolicyDescriptor.cs", L"Core/Runtime/Domains/Admission/Execution/ExecutionDomainRuntime.cs", L"Core/Runtime/Domains/Admission/IO/IoDomainRuntime.cs", L"Core/Runtime/Domains/Admission/Lane6/Lane6DomainRuntime.cs", L"Core/Runtime/Domains/Admission/Lane7/Lane7DomainRuntime.cs", L"Core/Runtime/Domains/Admission/Memory/MemoryDomainRuntime.cs", L"Core/Runtime/Domains/Admission/Nested/NestedDomainRuntime.cs", L"Core/Runtime/Domains/Admission/VectorStream/VectorStreamDomainRuntime.cs", L"Core/Runtime/Nested/Descriptors/NestedDomainDescriptor.cs", L"Core/Runtime/Nested/Projection/INestedProjectionService.cs", L"Core/Runtime/Nested/Projection/NestedProjectionService.cs", L"Core/Runtime/Nested/Validation/NestedValidationResult.cs"};
		static inline const std::vector<std::wstring> ForbiddenNeutralRuntimeMarkers = {L"Vmcs", L"Vmx", L"VMCS", L"VMX", L"MemoryTranslationControl", L"VmcsField", L"INVVPID", L"VMFUNC", L"VmxRetireEffect", L"IOMMU.VmxCompatibilityAliases", L"CsrAddresses", L"VmExit", L"VmxCaps", L"ShadowVmcs"};
		static inline const std::vector<std::wstring> RequiredNeutralRuntimeMarkers = {L"ExecutionDomainDescriptor", L"MemoryDomainDescriptor", L"IoDomainDescriptor", L"Lane6DomainDescriptor", L"Lane7AcceleratorDescriptor", L"NestedDomainDescriptor", L"ExecutionDomainRuntime", L"MemoryDomainRuntime", L"IoDomainRuntime", L"Lane6DomainRuntime", L"Lane7DomainRuntime", L"NestedDomainRuntime", L"VectorStreamDomainRuntime", L"NestedProjectionService", L"NestedValidationResult"};
		static inline const std::vector<std::wstring> FrozenCompatibilityProjectionPaths = {L"Core/VMX/Compatibility/Frontend/Projection/Memory/MemoryTranslationControl.cs", L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs", L"Core/VMX/Compatibility/Adapters/IO/IommuVmxCompatibilityAliases.cs", L"Core/VMX/Compatibility/Generated/CsrProjection/VmxCapsProjection.cs"};
		static inline const std::vector<std::wstring> RequiredFrozenCompatibilityMarkers = {L"MemoryTranslationControl", L"VmcsFieldProjectionSchema", L"InvalidateVmxIotlbByVmid", L"VmxCapsProjection"};

	public:
		static std::vector<std::wstring> getRemovedGenericDomainSubstratePaths();

		static std::vector<std::wstring> getNeutralRuntimePaths();

		static std::vector<std::wstring> getForbiddenNeutralRuntimeMarkers();

		static std::vector<std::wstring> getRequiredNeutralRuntimeMarkers();

		static inline const std::wstring CapabilityDescriptorSetPath = L"Core/Runtime/Capabilities/Descriptors/CapabilityDescriptorSet.cs";

		static inline const std::wstring CapabilityGeneratedSchemaPath = L"Core/VMX/Compatibility/Generated/CapabilityProjection/CapabilityDescriptorSetSchema.cs";

		static std::vector<std::wstring> getFrozenCompatibilityProjectionPaths();

		static std::vector<std::wstring> getRequiredFrozenCompatibilityMarkers();
	};
}
