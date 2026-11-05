#pragma once

#include <string>
#include <span>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	class VmcsV2ScalarWriteAuthorityRemovalContract final
	{
	private:
		static inline const std::vector<std::wstring> ForbiddenDescriptorMutationMarkerTable = {L"public bool TryWriteScalarField(", L"ValidateScalarWrite(", L"descriptor.AccessPolicy.CanWriteViaVmWrite", L"VMWRITE is not permitted for this VMCSv2 field.", L"Boolean VMCSv2 fields accept only 0 or 1.", L"Reserved VMCSv2 bits must be zero."};

		static inline const std::vector<std::wstring> ForbiddenNewRuntimeOwnerMarkerTable = {L"VmcsV2ScalarWriteRuntime", L"VmcsV2ScalarWriteService", L"VmcsScalarFieldStore", L"VmcsV2FieldStore", L"VmcsFieldStore", L"VmcsProjectionRuntimeManager", L"VmcsV2RuntimeManager", L"VmcsScalarMutationManager"};

	public:
		static inline const std::wstring VmcsDescriptorPath = L"NonRTL/Core/System/Vmcs/V2/VmcsV2Descriptor.cs";

		static inline const std::wstring ProjectionGatePath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsV2DescriptorProjection.cs";

		static inline const std::wstring FieldProjectionSchemaPath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs";

		static bool getRemovedWithoutReplacement();

		static bool getRejectsVmcsV2PublicScalarWriteAuthority();

		static bool getKeepsVmcsV2DescriptorReadProjectionOnly();

		static bool getRejectsVmcsFieldStoreReplacementOwner();

		static bool getKeepsFrozenFieldSchemaVocabularyOnly();

		static std::span<const std::wstring> getForbiddenDescriptorMutationMarkers();

		static std::span<const std::wstring> getForbiddenNewRuntimeOwnerMarkers();
	};

}
