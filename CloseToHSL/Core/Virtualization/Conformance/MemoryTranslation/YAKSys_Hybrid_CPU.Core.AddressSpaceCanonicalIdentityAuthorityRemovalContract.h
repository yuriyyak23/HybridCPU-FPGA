#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../../Runtime/Memory/Translation/MemoryDomainTranslationControl.h"
#include "../../../Runtime/Memory/AddressSpaces/AddressSpaceId.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class AddressSpaceCanonicalIdentityAuthorityRemovalContract final : public std::enable_shared_from_this<AddressSpaceCanonicalIdentityAuthorityRemovalContract>
	{
	private:
		static inline const std::vector<std::wstring> ForbiddenCanonicalIdentityMarkers = {L"Vmid", L"Vpid", L"NptRootIdentity", L"EptEpoch", L"VpidEpoch", L"VmcsIdentity", L"CompatibilityProjectionIdentity", L"HostOwned", L"MatchesEptContext", L"MatchesVpid", L"FlushNestedByNptRoot", L"FlushNestedByVpid", L"FlushNestedByVmid"};
		static inline const std::vector<std::wstring> ForbiddenNestedCompositionMarkers = {L"L2Vpid", L"InvalidateByNestedVpid", L"InvalidateByCompositeRoot", L"TryWalkRawNpt", L"VpidEpochOrZero"};

	public:
		static inline const std::wstring AddressSpaceIdentityPath = L"Core/Runtime/Memory/AddressSpaces/AddressSpaceId.cs";

		static inline const std::wstring NestedTlbTagPath = L"Core/Runtime/Memory/Translation/NestedTlbTag.cs";

		static inline const std::wstring NestedCompositionPath = L"Core/Runtime/Nested/MemoryComposition/NestedMemoryCompositionService.cs";

		static inline const std::wstring MemoryDomainDescriptorPath = L"Core/Runtime/Domains/Descriptors/MemoryDomain/MemoryDomainDescriptor.cs";

		static inline const std::wstring CompatibilityTranslationPath = L"Core/VMX/Compatibility/Frontend/Projection/Memory/MemoryTranslationControl.cs";

		static inline const std::wstring TlbPath = L"Memory/MMU/TLB.cs";

		static inline const std::wstring GenericIommuPath = L"Memory/MMU/IOMMU.cs";

		static inline const std::wstring DomainBoundIommuPath = L"Memory/MMU/IOMMU.DomainBinding.cs";

		static inline const std::wstring PageWalkerPath = L"Core/Runtime/Memory/Translation/NestedPageWalker.cs";

		static inline const std::wstring PageWalkerTranslatePath = L"Core/Runtime/Memory/Translation/NestedPageWalker.Translate.partial.cs";

		static std::vector<std::wstring> getForbiddenCanonicalIdentityMarkers();

		static std::vector<std::wstring> getForbiddenNestedCompositionMarkers();

		bool IsNeutralRuntimeIdentity(std::shared_ptr<YAKSys_Hybrid_CPU::Core::MemoryDomainTranslationControl> control, unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId> identity);
	};
}
