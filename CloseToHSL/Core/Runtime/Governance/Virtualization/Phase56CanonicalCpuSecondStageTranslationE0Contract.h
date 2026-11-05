#pragma once

#include <string>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Machine-readable governance description of the bounded Phase 56 contour.
	/// It grants no execution or compatibility authority.
	/// </summary>
	class Phase56CanonicalCpuSecondStageTranslationE0Contract final
	{
	public:
		static inline const std::wstring TranslationOwner = L"ExistingCpuInstructionTranslationOwner";
		static inline const std::wstring ConfigurationOwner = L"MemoryDomainRuntimeAndMemoryDomainDescriptorOnly";
		static inline const std::wstring AddressModel = L"ExplicitVirtualAddressToGuestPhysicalAddressToCpuPhysicalAddress";
		static inline const std::wstring Producer = L"CanonicalCpuSecondStageTranslationFaultProducer";
		static inline const std::wstring Arbitration = L"ExistingStageAwareOlderStageThenOrderedLaneWinner";
		static inline const std::wstring Freshness = L"IssuerSealedMemoryDomainOwnerBindingAddressSpaceGenerationAndRebindInvalidation";
		static inline const std::wstring NeutralFacts = L"TypedReasonQualificationGuestPhysicalAddressSecondStageViolationAuxiliary";
		static inline const std::wstring DefaultBehavior = L"IdentityAndSecondStageDisabled";
		static constexpr bool VmReadOpened = false;
		static constexpr bool VmWriteOpened = false;
		static constexpr bool VmxAuthorityGranted = false;
		static constexpr bool NestedOrIommuAuthorityUsed = false;
	};

}
