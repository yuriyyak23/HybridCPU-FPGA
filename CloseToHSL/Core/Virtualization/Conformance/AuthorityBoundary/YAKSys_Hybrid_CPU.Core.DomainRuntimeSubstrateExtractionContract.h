#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainRuntimeSubstrateExtractionContract final : public std::enable_shared_from_this<DomainRuntimeSubstrateExtractionContract>
	{
	private:
		static inline const std::vector<std::wstring> RemovedDomainRuntimeSubstratePaths = {L"Core/VMX/Substrate/Runtime/Services/DomainRuntimeContext.cs", L"Core/VMX/Substrate/Runtime/Services/DomainRuntimeOperation.cs", L"Core/VMX/Substrate/Runtime/Authority/DomainRuntimeAuthority.cs", L"Core/VMX/Substrate/Runtime/Authority/RootAuthorityDescriptor.cs", L"Core/VMX/Substrate/Runtime/Legality/DomainLegalityService.cs", L"Core/VMX/Substrate/Runtime/Validation/DomainValidationResult.cs", L"Core/VMX/Substrate/Runtime/Scheduling/DomainSchedulingAdmission.cs", L"Core/VMX/Substrate/Runtime/Binding/DomainBindingTable.cs"};
		static inline const std::vector<std::wstring> NeutralDomainRuntimePaths = {L"Core/Runtime/Domains/Services/DomainRuntimeContext.cs", L"Core/Runtime/Domains/Services/DomainRuntimeOperation.cs", L"Core/Runtime/Domains/Authority/DomainRuntimeAuthority.cs", L"Core/Runtime/Domains/Authority/RootAuthorityDescriptor.cs", L"Core/Runtime/Domains/Legality/DomainLegalityService.cs", L"Core/Runtime/Domains/Validation/DomainValidationResult.cs", L"Core/Runtime/Domains/Scheduling/DomainSchedulingAdmission.cs", L"Core/Runtime/Domains/Binding/DomainBindingTable.cs"};
		static inline const std::vector<std::wstring> FrozenCompatibilityVocabularyPaths = {L"Core/VMX/Compatibility/Frontend/Projection/Memory/MemoryTranslationControl.cs", L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs", L"Core/VMX/Compatibility/Adapters/IO/IommuVmxCompatibilityAliases.cs", L"Core/Common/CPU_Core.Enums.cs"};
		static inline const std::vector<std::wstring> ForbiddenNeutralDomainRuntimeMarkers = {L"Vmcs", L"Vmx", L"VMCS", L"VMX", L"MemoryTranslationControl", L"VmcsField", L"INVVPID", L"VMFUNC", L"VmxRetireEffect", L"IOMMU.VmxCompatibilityAliases"};
		static inline const std::vector<std::wstring> RequiredNeutralDomainRuntimeMarkers = {L"DomainRuntimeContext", L"DomainRuntimeOperation", L"DomainRuntimeAuthority", L"RootAuthorityDescriptor", L"DomainLegalityService", L"DomainValidationResult", L"DomainSchedulingAdmission", L"DomainBindingTable"};
		static inline const std::vector<std::wstring> RequiredRuntimeAdmissionMarkers = {L"DomainRuntimeAuthority", L"RuntimeBoundaryAdmissionService", L"CapabilityBoundaryRequirement", L"EvidenceBoundaryRequirement", L"FrontendAuthoritativeMutationDenied"};
		static inline const std::vector<std::wstring> RequiredFrozenCompatibilityMarkers = {L"MemoryTranslationControl", L"VmcsFieldProjectionSchema", L"InvalidateVmxIotlbByVmid", L"VMFUNC"};

	public:
		static inline const std::wstring OldDomainRuntimeContextPath = L"Core/VMX/Substrate/Runtime/Services/DomainRuntimeContext.cs";

		static inline const std::wstring OldDomainRuntimeOperationPath = L"Core/VMX/Substrate/Runtime/Services/DomainRuntimeOperation.cs";

		static inline const std::wstring OldDomainRuntimeAuthorityPath = L"Core/VMX/Substrate/Runtime/Authority/DomainRuntimeAuthority.cs";

		static inline const std::wstring OldRootAuthorityDescriptorPath = L"Core/VMX/Substrate/Runtime/Authority/RootAuthorityDescriptor.cs";

		static inline const std::wstring OldDomainLegalityServicePath = L"Core/VMX/Substrate/Runtime/Legality/DomainLegalityService.cs";

		static inline const std::wstring OldDomainValidationResultPath = L"Core/VMX/Substrate/Runtime/Validation/DomainValidationResult.cs";

		static inline const std::wstring OldDomainSchedulingAdmissionPath = L"Core/VMX/Substrate/Runtime/Scheduling/DomainSchedulingAdmission.cs";

		static inline const std::wstring OldDomainBindingTablePath = L"Core/VMX/Substrate/Runtime/Binding/DomainBindingTable.cs";

		static inline const std::wstring NewDomainRuntimeContextPath = L"Core/Runtime/Domains/Services/DomainRuntimeContext.cs";

		static inline const std::wstring NewDomainRuntimeOperationPath = L"Core/Runtime/Domains/Services/DomainRuntimeOperation.cs";

		static inline const std::wstring NewDomainRuntimeAuthorityPath = L"Core/Runtime/Domains/Authority/DomainRuntimeAuthority.cs";

		static inline const std::wstring NewRootAuthorityDescriptorPath = L"Core/Runtime/Domains/Authority/RootAuthorityDescriptor.cs";

		static inline const std::wstring NewDomainLegalityServicePath = L"Core/Runtime/Domains/Legality/DomainLegalityService.cs";

		static inline const std::wstring NewDomainValidationResultPath = L"Core/Runtime/Domains/Validation/DomainValidationResult.cs";

		static inline const std::wstring NewDomainSchedulingAdmissionPath = L"Core/Runtime/Domains/Scheduling/DomainSchedulingAdmission.cs";

		static inline const std::wstring NewDomainBindingTablePath = L"Core/Runtime/Domains/Binding/DomainBindingTable.cs";

		static inline const std::wstring RuntimeBoundaryAdmissionPath = L"Core/Runtime/Services/RuntimeBoundaryAdmissionService.cs";

		static inline const std::wstring FrozenMemoryTranslationControlPath = L"Core/VMX/Compatibility/Frontend/Projection/Memory/MemoryTranslationControl.cs";

		static inline const std::wstring FrozenVmcsFieldProjectionPath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs";

		static inline const std::wstring FrozenIotlbCompatibilityAliasPath = L"Core/VMX/Compatibility/Adapters/IO/IommuVmxCompatibilityAliases.cs";

		static inline const std::wstring FrozenOpcodeVocabularyPath = L"Core/Common/CPU_Core.Enums.cs";

		static std::vector<std::wstring> getRemovedDomainRuntimeSubstratePaths();

		static std::vector<std::wstring> getNeutralDomainRuntimePaths();

		static std::vector<std::wstring> getFrozenCompatibilityVocabularyPaths();

		static std::vector<std::wstring> getForbiddenNeutralDomainRuntimeMarkers();

		static std::vector<std::wstring> getRequiredNeutralDomainRuntimeMarkers();

		static std::vector<std::wstring> getRequiredRuntimeAdmissionMarkers();

		static std::vector<std::wstring> getRequiredFrozenCompatibilityMarkers();
	};
}
