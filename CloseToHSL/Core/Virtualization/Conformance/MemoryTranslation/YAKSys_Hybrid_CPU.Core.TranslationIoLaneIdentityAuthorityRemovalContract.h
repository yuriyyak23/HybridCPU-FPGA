#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../../Runtime/IO/Dma/DmaDomainBinding.h"
#include "../../../../Memory/MMU/IotlbTag.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	class TranslationIoLaneIdentityAuthorityRemovalContract final : public std::enable_shared_from_this<TranslationIoLaneIdentityAuthorityRemovalContract>
	{
	private:
		static inline const std::vector<std::wstring> IoIdentityPaths = {L"Core/Runtime/IO/Dma/DmaDomainBinding.cs", L"Memory/MMU/IotlbTag.cs", L"Memory/MMU/IOMMU.DomainBinding.cs", L"Memory/MMU/IoVirtualizationHostBackend.cs", L"Core/Runtime/Domains/Descriptors/IoDomain/IoVirtualizationBlock.cs", L"Core/Runtime/IO/Iotlb/IotlbInvalidationService.cs"};
		static inline const std::vector<std::wstring> LaneIdentityPaths = {L"Core/Runtime/Lanes/Lane6/Lane6StateBlock.cs", L"Core/Runtime/Lanes/Lane6/Lane6QueueRuntime.cs", L"NonRTL/Core/Execution/DmaStreamCompute/VmxDmaDescriptorValidator.cs", L"Core/Runtime/Lanes/Lane7/Lane7StateBlock.cs", L"Core/Runtime/Lanes/Lane7/Lane7StateBlock.Checkpoint.partial.cs", L"Core/Runtime/Completion/Routing/LaneCompletionRouting.cs"};
		static inline const std::vector<std::wstring> ForbiddenExecutableIdentityMarkers = {L"Vmid", L"Vpid", L"VMID", L"VPID"};
		static inline const std::vector<std::wstring> ForbiddenNestedResultMarkers = {L"NptViolation", L"NptMisconfiguration", L"FromNptFault", L"CausesVmExit"};
		static inline const std::vector<std::wstring> ForbiddenCompatibilityFactoryMarkers = {L"CreateSecondStageControl", L"CreateRuntimeProjection", L"FromDomainControl", L"ToCompatibilityControl"};

	public:
		static inline const std::wstring IommuDomainBindingPath = L"Core/Runtime/IO/Dma/DmaDomainBinding.cs";

		static inline const std::wstring IotlbTagPath = L"Memory/MMU/IotlbTag.cs";

		static inline const std::wstring DomainBoundIommuPath = L"Memory/MMU/IOMMU.DomainBinding.cs";

		static inline const std::wstring IoHostBackendPath = L"Memory/MMU/IoVirtualizationHostBackend.cs";

		static inline const std::wstring IoVirtualizationBlockPath = L"Core/Runtime/Domains/Descriptors/IoDomain/IoVirtualizationBlock.cs";

		static inline const std::wstring IotlbInvalidationServicePath = L"Core/Runtime/IO/Iotlb/IotlbInvalidationService.cs";

		static inline const std::wstring NestedTranslationResultPath = L"Core/Runtime/Memory/Translation/NestedTranslationResult.cs";

		static inline const std::wstring CompatibilityTranslationControlPath = L"Core/VMX/Compatibility/Frontend/Projection/Memory/MemoryTranslationControl.cs";

		static inline const std::wstring RemovedNestedProjectionComposerPath = L"Core/VMX/Substrate/Nested/Projection/ComposedDomainProjectionComposer.cs";

		static inline const std::wstring Lane6StatePath = L"Core/Runtime/Lanes/Lane6/Lane6StateBlock.cs";

		static inline const std::wstring Lane6QueueRuntimePath = L"Core/Runtime/Lanes/Lane6/Lane6QueueRuntime.cs";

		static inline const std::wstring DmaValidationPath = L"NonRTL/Core/Execution/DmaStreamCompute/VmxDmaDescriptorValidator.cs";

		static inline const std::wstring Lane7StatePath = L"Core/Runtime/Lanes/Lane7/Lane7StateBlock.cs";

		static inline const std::wstring Lane7CheckpointPath = L"Core/Runtime/Lanes/Lane7/Lane7StateBlock.Checkpoint.partial.cs";

		static inline const std::wstring LaneCompletionRoutingPath = L"Core/Runtime/Completion/Routing/LaneCompletionRouting.cs";

		static inline const std::wstring CompatibilityIoAliasPath = L"Core/VMX/Compatibility/Adapters/IO/IommuVmxCompatibilityAliases.cs";

		static std::vector<std::wstring> getIoIdentityPaths();

		static std::vector<std::wstring> getLaneIdentityPaths();

		static std::vector<std::wstring> getForbiddenExecutableIdentityMarkers();

		static std::vector<std::wstring> getForbiddenNestedResultMarkers();

		static std::vector<std::wstring> getForbiddenCompatibilityFactoryMarkers();

		bool IsNeutralIotlbIdentity(std::shared_ptr<IommuDomainBinding> binding, std::shared_ptr<IotlbTag> tag);
	};
}
