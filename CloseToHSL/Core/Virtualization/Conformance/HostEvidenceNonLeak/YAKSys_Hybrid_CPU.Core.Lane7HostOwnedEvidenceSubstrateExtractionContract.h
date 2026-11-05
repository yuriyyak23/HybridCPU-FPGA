#pragma once

#include "../../../Runtime/Lanes/Lane7/YAKSys_Hybrid_CPU.Core.Lane7Checkpoint.h"
#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7HostOwnedEvidenceSubstrateExtractionContract final : public std::enable_shared_from_this<Lane7HostOwnedEvidenceSubstrateExtractionContract>
	{
	private:
		static inline const std::vector<std::wstring> ForbiddenLane7StateCacheMarkers = {L"_hostTokenByVirtualValue", L"_virtualTokenByHostHandle", L"_backendBindings", L"_submitPollCount", L"public ulong BackendBindingEpoch { get; private set; }", L"public ulong PressureEpoch { get; private set; }", L"public Lane7PressureSnapshot LastPressure { get; private set; }"};
		static inline const std::vector<std::wstring> RequiredLane7StateDelegationMarkers = {L"public Lane7HostOwnedEvidenceStore HostEvidence", L"BackendBindingEpoch => HostEvidence.BackendBindingEpoch", L"PressureEpoch => HostEvidence.PressureEpoch", L"LastPressure => HostEvidence.LastPressure", L"PrepareHostEvidenceForRestore", L"RebuildHostTokenAfterRestore", L"RebuildBackendBindingAfterRestore"};
		static inline const std::vector<std::wstring> ForbiddenNeutralEvidenceStoreMarkers = {L"Vmcs", L"Vmx", L"VMCS", L"VMX", L"MemoryTranslationControl", L"VmcsField", L"INVVPID", L"VMFUNC", L"VmxFunctionLeaf", L"Serialize", L"RestoreHostHandle", L"ImportHost"};
		static inline const std::vector<std::wstring> RequiredNeutralEvidenceStoreMarkers = {L"EvidenceVisibilityClass.NativeTokenEvidence", L"EvidenceVisibilityClass.BackendBindingEvidence", L"EvidenceVisibilityClass.SchedulerEvidence", L"EvidenceRestorePolicy.RecomputeAfterRestore", L"HostOwnedEvidenceBoundary", L"_nativeTokenByVirtualToken.Clear()", L"_backendBindings.Clear()", L"ResetSchedulerEvidence", L"RebuildTokenAfterRestore", L"RebuildBackendAfterRestore", L"TryAdvanceEpoch", L"epoch == ulong.MaxValue"};
		static inline const std::vector<std::wstring> ForbiddenCheckpointEvidenceLeakMarkers = {L"ExposesHostTokenHandle(hostHandle)", L"hostHandle.Value", L"_hostTokenByVirtualValue", L"_virtualTokenByHostHandle", L"_backendBindings"};
		static inline const std::vector<std::wstring> RequiredFrozenCompatibilityMarkers = {L"MemoryTranslationControl", L"VmcsFieldProjectionSchema", L"InvalidateVmxIotlbByVmid"};

	public:
		static inline const std::wstring Lane7StatePath = L"Core/Runtime/Lanes/Lane7/Lane7StateBlock.cs";

		static inline const std::wstring Lane7CheckpointPath = L"Core/Runtime/Lanes/Lane7/Lane7StateBlock.Checkpoint.partial.cs";

		static inline const std::wstring Lane7CheckpointEvidencePath = L"Core/Runtime/Lanes/Lane7/Lane7Checkpoint.Evidence.partial.cs";

		static inline const std::wstring Lane7VirtualTokenEvidencePath = L"Core/Runtime/Lanes/Lane7/Lane7VirtualToken.Evidence.partial.cs";

		static inline const std::wstring NeutralHostEvidenceStorePath = L"Core/Runtime/Lanes/Lane7/HostOwnedEvidence/Lane7HostOwnedEvidenceStore.cs";

		static inline const std::wstring FrozenMemoryTranslationControlPath = L"Core/VMX/Compatibility/Frontend/Projection/Memory/MemoryTranslationControl.cs";

		static inline const std::wstring FrozenVmcsFieldProjectionPath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs";

		static inline const std::wstring FrozenIotlbCompatibilityAliasPath = L"Core/VMX/Compatibility/Adapters/IO/IommuVmxCompatibilityAliases.cs";

		static bool getExtractsLane7HostEvidenceToNeutralRuntimeNamespace();

		static bool getRequiresTokenBackendAndSchedulerEvidenceRebuildAfterRestore();

		static bool getKeepsVmxTranslationVmcsAndIotlbNamesFrozenCompatibilityOnly();

		static std::vector<std::wstring> getForbiddenLane7StateCacheMarkers();

		static std::vector<std::wstring> getRequiredLane7StateDelegationMarkers();

		static std::vector<std::wstring> getForbiddenNeutralEvidenceStoreMarkers();

		static std::vector<std::wstring> getRequiredNeutralEvidenceStoreMarkers();

		static std::vector<std::wstring> getForbiddenCheckpointEvidenceLeakMarkers();

		static std::vector<std::wstring> getRequiredFrozenCompatibilityMarkers();

		bool RejectsCheckpointNativeHandleProjection(std::shared_ptr<Lane7Checkpoint> checkpoint, std::shared_ptr<Execution::ExternalAccelerators::Tokens::AcceleratorTokenHandle> hostHandle);
	};
}
