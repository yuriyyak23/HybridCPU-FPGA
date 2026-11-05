#pragma once

#include <string>
#include <span>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	class VmcsV2VectorStreamProjectionAuthorityRemovalContract final
	{
	private:
		static inline const std::vector<std::wstring> RemovedPathTable = {L"NonRTL/Core/Execution/VectorStream/VmxVectorStreamStateManager.cs", L"NonRTL/Core/Execution/VectorStream/VmxStreamDescriptorValidator.cs"};

		static inline const std::vector<std::wstring> ForbiddenProductionOwnerMarkerTable = {L"VmxVectorStreamStateManager", L"VmxStreamDescriptorValidator", L"VmxStreamDescriptorValidationRequest", L"VmxStreamDescriptorValidationResult", L"VmxStreamDescriptorValidationEvidence", L"VmxGuestStreamDescriptorParser", L"VmxGuestStreamDescriptor", L"SaveGuestState(VmcsV2Descriptor", L"RestoreGuestState(VmcsV2Descriptor", L"MarkDirty(VmcsV2Descriptor"};

		static inline const std::vector<std::wstring> ForbiddenNewRuntimeOwnerMarkerTable = {L"VmcsV2Runtime", L"VmcsProjectionRuntimeManager", L"VmcsV2RuntimeManager", L"ActiveV2Descriptor", L"TryMarkVectorStreamDirty"};

	public:
		static inline const std::wstring RemovedStateManagerPath = L"NonRTL/Core/Execution/VectorStream/VmxVectorStreamStateManager.cs";

		static inline const std::wstring RemovedDescriptorValidatorPath = L"NonRTL/Core/Execution/VectorStream/VmxStreamDescriptorValidator.cs";

		static inline const std::wstring NeutralRuntimePath = L"Core/Runtime/Domains/Admission/VectorStream/VectorStreamDomainRuntime.cs";

		static inline const std::wstring ProjectionGatePath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsV2DescriptorProjection.cs";

		static bool getRemovedWithoutReplacement();

		static bool getRejectsVmcsDescriptorOwnedVectorSaveRestore();

		static bool getRejectsVmcsDescriptorOwnedStreamValidationEvidence();

		static bool getKeepsFrozenVectorStreamVocabularyProjectionOnly();

		static bool getGenericVectorStreamRuntimeRemainsNeutralOwner();

		static std::span<const std::wstring> getRemovedPaths();

		static std::span<const std::wstring> getForbiddenProductionOwnerMarkers();

		static std::span<const std::wstring> getForbiddenNewRuntimeOwnerMarkers();
	};

}
