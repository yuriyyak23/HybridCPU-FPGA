#pragma once

#include <string>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	class VmcsV2BlocksDirtyVectorCheckpointAuthorityRemovalContract final
	{
	private:
		static inline const std::vector<std::wstring> ForbiddenVmcsBlocksHelperMarkerTable = {L"public ulong ConfigurePolicy(", L"public bool CanUseSaveMask(", L"public ulong BindStreamDescriptorTable(", L"public VectorStreamSnapshot CaptureSnapshot(", L"public bool TryRestoreSnapshot(", L"public void RestoreCheckpointSnapshot(", L"public ulong MarkVectorDirty(", L"public ulong AdvanceStreamReplayEpoch(", L"public ulong AdvanceStreamQueueEpoch(", L"public ulong AdvanceStreamCompletionEpoch(", L"public VmxVectorExceptionInfo RecordVectorException(", L"public VmxStreamDescriptorFaultInfo RecordStreamDescriptorFault(", L"public bool IsDescriptorAddressInRange(", L"private ulong AdvancePolicyEpoch(", L"private ulong AdvanceArchitecturalEpoch(", L"private ulong AdvanceStreamEpoch(", L"private ulong NextSequence(", L"private readonly HashSet<ulong> _dirtyPageIndices", L"public VmcsV2ValidationResult Configure(VmxDirtyLogConfiguration", L"public bool TryMarkDirtyRange(", L"public bool TrySnapshot(", L"public bool TryClear(", L"public void RestoreSnapshot(VmxDirtyLogSnapshot", L"private void RecordAcceptedWrite(", L"private void RecordOverflow(", L"private void AdvanceGeneration(", L"private static bool IsPowerOfTwo("};

		static inline const std::vector<std::wstring> ForbiddenDescriptorHelperMarkerTable = {L"DirtyLog.Configure(", L"TryCreateGuestCheckpoint", L"SnapshotMigratableScalarFields", L"RestoreGuestStateForMigration", L"RestoreScalarFieldForMigration"};

		static inline const std::vector<std::wstring> ForbiddenNewRuntimeOwnerMarkerTable = {L"VmcsV2DirtyLogRuntime", L"VmcsDirtyTrackingService", L"VmcsDirtyLogService", L"VmcsVectorStreamRuntime", L"VmcsVectorStreamService", L"VmcsCheckpointBlockService", L"VmcsProjectionRuntimeManager", L"VmcsV2RuntimeManager"};

	public:
		static inline const std::wstring VmcsBlocksPath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsV2Blocks.cs";

		static inline const std::wstring VmcsDescriptorPath = L"NonRTL/Core/System/Vmcs/V2/VmcsV2Descriptor.cs";

		static inline const std::wstring NeutralVectorStreamRuntimePath = L"Core/Runtime/Domains/Admission/VectorStream/VectorStreamDomainRuntime.cs";

		static inline const std::wstring NeutralDomainCheckpointImagePath = L"Core/Runtime/Migration/Checkpoint/DomainCheckpointImage.cs";

		static inline const std::wstring VmxCompatibilityAliasesPath = L"Core/VMX/Compatibility/Adapters/IO/IommuVmxCompatibilityAliases.cs";

		static bool getRemovedWithoutReplacement();

		static bool getRejectsVmcsBlocksDirtyLogMutationAuthority();

		static bool getRejectsVmcsBlocksVectorSaveRestoreAuthority();

		static bool getRejectsVmcsBlocksCheckpointRestoreAuthority();

		static bool getKeepsVmcsBlocksVocabularyProjectionOnly();

		static bool getGenericVectorStreamRuntimeRemainsNeutralOwner();

		static bool getGenericDomainCheckpointRemainsNeutralOwner();

		static bool getCompatibilityDirtyAccountingRemainsFailClosed();

		static const std::vector<std::wstring> &getForbiddenVmcsBlocksHelperMarkers();

		static const std::vector<std::wstring> &getForbiddenDescriptorHelperMarkers();

		static const std::vector<std::wstring> &getForbiddenNewRuntimeOwnerMarkers();
	};

}
