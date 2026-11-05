#pragma once

#include "../../../Runtime/Nested/NestedDomainProjectionCheckpointService.h"
#include "../../../System/VectorStreamExceptionInfo.h"
#include "../../../System/VectorStreamDescriptorFaultInfo.h"
#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VectorAcceleratorNestedIdentityPressureRemovalContract final : public std::enable_shared_from_this<VectorAcceleratorNestedIdentityPressureRemovalContract>
	{
	private:
		static inline const std::vector<std::wstring> RemovedVectorIdentityPaths = {L"NonRTL/Core/System/VmxVectorExceptionInfo.cs", L"NonRTL/Core/System/VmxStreamDescriptorFaultInfo.cs"};
		static inline const std::vector<std::wstring> RemovedAcceleratorWrapperPaths = {L"NonRTL/Core/Execution/ExternalAccelerators/VmxAcceleratorHandleMap.cs", L"NonRTL/Core/Execution/ExternalAccelerators/VmxLane7TokenVirtualizer.cs", L"NonRTL/Core/Execution/ExternalAccelerators/VmxAcceleratorCompletionRouter.cs", L"NonRTL/Core/Execution/ExternalAccelerators/VmxAcceleratorPolicy.cs"};
		static inline const std::vector<std::wstring> NeutralVectorIdentityPaths = {L"Core/System/VectorStreamExceptionInfo.cs", L"Core/System/VectorStreamDescriptorFaultInfo.cs"};
		static inline const std::vector<std::wstring> NeutralAcceleratorNamespacePaths = {L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorHandleNamespace.cs", L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorTokenNamespace.cs", L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorCompletionRouter.cs", L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorAdmissionPolicy.cs"};
		static inline const std::vector<std::wstring> ForbiddenVectorIdentityMarkers = {L"VmxVectorExceptionInfo", L"VmxStreamDescriptorFaultInfo", L"Vmid", L"Vpid", L"vmid", L"vpid", L"CausesVmExit", L"ReflectAsVmExit"};
		static inline const std::vector<std::wstring> ForbiddenAcceleratorNamespaceMarkers = {L"VmxAcceleratorHandleMap", L"VmxLane7TokenVirtualizer", L"VmxAcceleratorCompletionRouter", L"VmxAcceleratorPolicy", L"ushort vmid", L"vmid", L"Vmid", L"Vpid"};
		static inline const std::vector<std::wstring> ForbiddenNestedIntentMarkers = {L"Vmcs12Pointer", L"TryVmRead", L"TryVmWrite", L"VmcsField.Vpid", L"VmcsField.", L"ushort vmid", L"ushort vpid"};
		static inline const std::vector<std::wstring> RequiredNeutralNestedIntentMarkers = {L"ChildDomainIntentFieldIds.AddressSpaceTag", L"SecondStageRootPointer", L"SecondStageViolationQualification", L"ChildIntentPointer"};

	public:
		static inline const std::wstring RemovedVectorExceptionInfoPath = L"NonRTL/Core/System/VmxVectorExceptionInfo.cs";

		static inline const std::wstring RemovedStreamDescriptorFaultInfoPath = L"NonRTL/Core/System/VmxStreamDescriptorFaultInfo.cs";

		static inline const std::wstring NeutralVectorExceptionInfoPath = L"NonRTL/Core/System/VectorStreamExceptionInfo.cs";

		static inline const std::wstring NeutralStreamDescriptorFaultInfoPath = L"NonRTL/Core/System/VectorStreamDescriptorFaultInfo.cs";

		static inline const std::wstring VmcsBlocksPath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsV2Blocks.cs";

		static inline const std::wstring VmcsDescriptorPath = L"NonRTL/Core/System/Vmcs/V2/VmcsV2Descriptor.cs";

		static inline const std::wstring RemovedAcceleratorHandleMapPath = L"NonRTL/Core/Execution/ExternalAccelerators/VmxAcceleratorHandleMap.cs";

		static inline const std::wstring RemovedAcceleratorTokenVirtualizerPath = L"NonRTL/Core/Execution/ExternalAccelerators/VmxLane7TokenVirtualizer.cs";

		static inline const std::wstring RemovedAcceleratorCompletionRouterPath = L"NonRTL/Core/Execution/ExternalAccelerators/VmxAcceleratorCompletionRouter.cs";

		static inline const std::wstring RemovedAcceleratorPolicyPath = L"NonRTL/Core/Execution/ExternalAccelerators/VmxAcceleratorPolicy.cs";

		static inline const std::wstring NeutralAcceleratorHandleNamespacePath = L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorHandleNamespace.cs";

		static inline const std::wstring NeutralAcceleratorTokenNamespacePath = L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorTokenNamespace.cs";

		static inline const std::wstring NeutralAcceleratorCompletionRouterPath = L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorCompletionRouter.cs";

		static inline const std::wstring NeutralAcceleratorAdmissionPolicyPath = L"Core/Runtime/Lanes/Lane7/Accelerators/Lane7AcceleratorAdmissionPolicy.cs";

		static inline const std::wstring Lane7StatePath = L"Core/Runtime/Lanes/Lane7/Lane7StateBlock.cs";

		static inline const std::wstring ChildDomainIntentPath = L"Core/VMX/Compatibility/Frontend/Projection/Nested/ChildDomainIntentDescriptor.cs";

		static inline const std::wstring NestedProjectionCheckpointOwnerPath = L"Core/Runtime/Nested/NestedDomainProjectionCheckpointService.cs";

		static std::vector<std::wstring> getRemovedVectorIdentityPaths();

		static std::vector<std::wstring> getRemovedAcceleratorWrapperPaths();

		static std::vector<std::wstring> getNeutralVectorIdentityPaths();

		static std::vector<std::wstring> getNeutralAcceleratorNamespacePaths();

		static std::vector<std::wstring> getForbiddenVectorIdentityMarkers();

		static std::vector<std::wstring> getForbiddenAcceleratorNamespaceMarkers();

		static std::vector<std::wstring> getForbiddenNestedIntentMarkers();

		static std::vector<std::wstring> getRequiredNeutralNestedIntentMarkers();

		bool IsNeutralVectorExceptionIdentity(std::shared_ptr<VectorStreamExceptionInfo> info, unsigned short executionDomainTag, unsigned short addressSpaceTag);

		bool IsNeutralStreamFaultIdentity(std::shared_ptr<VectorStreamDescriptorFaultInfo> info, unsigned short executionDomainTag, unsigned short addressSpaceTag);

		bool RejectsHostOwnedNestedRestore(std::shared_ptr<NestedDomainProjectionCheckpointService> service, std::shared_ptr<NestedDomainProjectionCheckpointRequest> request);
	};
}
