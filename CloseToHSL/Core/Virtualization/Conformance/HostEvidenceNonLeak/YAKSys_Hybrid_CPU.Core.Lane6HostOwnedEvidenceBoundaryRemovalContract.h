#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane6HostOwnedEvidenceBoundaryRemovalContract final : public std::enable_shared_from_this<Lane6HostOwnedEvidenceBoundaryRemovalContract>
	{
	private:
		static inline const std::vector<std::wstring> ForbiddenNeutralEvidenceStoreMarkers = {L"Vmcs", L"Vmx", L"Snapshot", L"Serialize", L"RestoreHostHandle", L"ImportHost"};
		static inline const std::vector<std::wstring> ForbiddenLane6StateOwnerMarkers = {L"VmxLane6QueueVirtualizer", L"QueueVirtualizer", L"_hostTokens"};
		static inline const std::vector<std::wstring> RequiredRebuildMarkers = {L"PrepareForRestore", L"NativeTokenEvidence", L"RestoreRequiresRecompute", L"_nativeTokenBindings.Clear()", L"RebuildAfterRestore"};
		static inline const std::vector<std::wstring> RequiredFailClosedEpochMarkers = {L"DmaFaultKind.EpochExhausted", L"TryAdvanceEpoch", L"epoch == ulong.MaxValue", L"refuses wraparound"};

	public:
		static inline const std::wstring RemovedVmxQueueVirtualizerPath = L"NonRTL/Core/Execution/DmaStreamCompute/VmxLane6QueueVirtualizer.cs";

		static inline const std::wstring Lane6StatePath = L"Core/Runtime/Lanes/Lane6/Lane6StateBlock.cs";

		static inline const std::wstring NeutralQueueRuntimePath = L"Core/Runtime/Lanes/Lane6/Lane6QueueRuntime.cs";

		static inline const std::wstring NeutralHostEvidenceStorePath = L"Core/Runtime/Lanes/Lane6/HostOwnedEvidence/Lane6HostOwnedEvidenceStore.cs";

		static inline const std::wstring MigrationDescriptorPath = L"Core/Runtime/Migration/Format/MigrationDescriptor.cs";

		static inline const std::wstring DomainCheckpointImagePath = L"Core/Runtime/Migration/Checkpoint/DomainCheckpointImage.cs";

		static bool getRemovedVmxShapedHostTokenCarrier();

		static bool getRequiresHostEvidenceRebuildAfterRestore();

		static bool getRejectsEpochWraparound();

		static std::vector<std::wstring> getForbiddenNeutralEvidenceStoreMarkers();

		static std::vector<std::wstring> getForbiddenLane6StateOwnerMarkers();

		static std::vector<std::wstring> getRequiredRebuildMarkers();

		static std::vector<std::wstring> getRequiredFailClosedEpochMarkers();
	};
}
