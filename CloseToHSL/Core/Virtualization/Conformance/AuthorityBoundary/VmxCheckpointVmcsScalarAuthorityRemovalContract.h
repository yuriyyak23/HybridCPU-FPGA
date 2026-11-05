#pragma once

#include <string>
#include <span>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	class VmxCheckpointVmcsScalarAuthorityRemovalContract final
	{
	private:
		static inline const std::vector<std::wstring> RemovedPathTable = {L"NonRTL/Core/System/Migration/VmxCheckpointImage.cs", L"NonRTL/Core/System/Vmcs/V2/VmcsV2Checkpoint.cs"};

		static inline const std::vector<std::wstring> ForbiddenProductionOwnerMarkerTable = {L"VmxCheckpointImage", L"VmxCheckpointWriter", L"VmxCheckpointReader", L"VmxCheckpointBlockKind", L"VmxRuntimeCapabilitySet", L"VmxMigrationSecurityPolicy", L"VmxCapabilityNegotiator", L"VmcsV2Checkpoint", L"VmcsV2ScalarFieldSnapshot", L"TryCreateGuestCheckpoint", L"SnapshotMigratableScalarFields", L"RestoreGuestStateForMigration", L"RestoreScalarFieldForMigration", L"ScalarVmcsFields", L"ApplyScalarFields"};

		static inline const std::vector<std::wstring> ForbiddenNewRuntimeOwnerMarkerTable = {L"VmcsCheckpointService", L"VmcsV2CheckpointService", L"VmcsScalarCheckpointService", L"VmcsProjectionCheckpointManager", L"VmcsV2Runtime", L"VmcsProjectionRuntimeManager", L"VmcsV2RuntimeManager", L"VmcsV2Descriptor as source of truth"};

	public:
		static inline const std::wstring RemovedCheckpointImagePath = L"NonRTL/Core/System/Migration/VmxCheckpointImage.cs";

		static inline const std::wstring RemovedVmcsCheckpointDtoPath = L"NonRTL/Core/System/Vmcs/V2/VmcsV2Checkpoint.cs";

		static inline const std::wstring VmcsDescriptorPath = L"NonRTL/Core/System/Vmcs/V2/VmcsV2Descriptor.cs";

		static inline const std::wstring DomainCheckpointImagePath = L"Core/Runtime/Migration/Checkpoint/DomainCheckpointImage.cs";

		static inline const std::wstring RestoreValidationServicePath = L"Core/Runtime/Migration/Restore/RestoreValidationService.cs";

		static bool getRemovedWithoutReplacement();

		static bool getRejectsVmcsScalarCheckpointImageAuthority();

		static bool getRejectsVmcsDescriptorScalarRestoreAuthority();

		static bool getKeepsVmcsVocabularyProjectionOnly();

		static bool getGenericDomainCheckpointRemainsNeutralOwner();

		static std::span<const std::wstring> getRemovedPaths();

		static std::span<const std::wstring> getForbiddenProductionOwnerMarkers();

		static std::span<const std::wstring> getForbiddenNewRuntimeOwnerMarkers();
	};

}
