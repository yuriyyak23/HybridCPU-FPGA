#pragma once

#include <string>
#include <span>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	class VmcsV2ScalarProjectionCacheAuthorityRemovalContract final
	{
	private:
		static inline const std::vector<std::wstring> ForbiddenDescriptorCacheMarkerTable = {L"_scalarValues", L"_scalarWritten", L"WriteKnownScalar(", L"HasScalarFieldValue(", L"TryGetScalarFieldValue(", L"Array.Clear(_scalarValues)", L"Array.Clear(_scalarWritten)", L"value = _scalarWritten"};

		static inline const std::vector<std::wstring> ForbiddenNewRuntimeOwnerMarkerTable = {L"VmcsV2ScalarProjectionCache", L"VmcsV2ScalarProjectionStore", L"VmcsScalarProjectionCache", L"VmcsScalarProjectionStore", L"VmcsScalarReadStore", L"VmcsV2ReadCacheRuntime", L"VmcsProjectionRuntimeManager", L"VmcsV2RuntimeManager", L"VmcsFieldStore"};

	public:
		static inline const std::wstring VmcsDescriptorPath = L"NonRTL/Core/System/Vmcs/V2/VmcsV2Descriptor.cs";

		static inline const std::wstring ProjectionGatePath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsV2DescriptorProjection.cs";

		static inline const std::wstring FieldProjectionSchemaPath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs";

		static bool getRemovedWithoutReplacement();

		static bool getRejectsDescriptorOwnedScalarProjectionCache();

		static bool getKeepsTryReadScalarFieldAsDeniedCompatibilityAbi();

		static bool getRequiresGeneratedReadOnlyProjectionOverNeutralOwners();

		static bool getRejectsScalarProjectionStoreReplacementOwner();

		static std::span<const std::wstring> getForbiddenDescriptorCacheMarkers();

		static std::span<const std::wstring> getForbiddenNewRuntimeOwnerMarkers();
	};

}
