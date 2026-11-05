#pragma once
#include "../../../Memory/Translation/MemoryDomainReadOnlyTranslationView.h"
#include "../../../Memory/Translation/MemoryDomainTranslationControl.h"

#include "../../../Memory/AddressSpaces/YAKSys_Hybrid_CPU.Core.AddressSpaceDescriptor.h"
#include "../../../Memory/Translation/YAKSys_Hybrid_CPU.Core.MemoryTranslationPolicy.h"
#include "../../../Memory/DirtyTracking/YAKSys_Hybrid_CPU.Core.DirtyTrackingServiceDescriptor.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class MemoryDomainDescriptor final : public std::enable_shared_from_this<MemoryDomainDescriptor>
	{
	private:
		const std::shared_ptr<AddressSpaceDescriptor> AddressSpace;
		const std::shared_ptr<MemoryTranslationPolicy> TranslationPolicy;
		const std::shared_ptr<MemoryDomainTranslationControl> TranslationControl;
		const std::shared_ptr<DirtyTrackingServiceDescriptor> DirtyTracking;
		const bool OwnsSecondStageTranslation;

	public:
		MemoryDomainDescriptor();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemoryDomainDescriptor(AddressSpaceDescriptor? addressSpace, System.Nullable<MemoryTranslationPolicy> translationPolicy, MemoryDomainTranslationControl translationControl, System.Nullable<DirtyTrackingServiceDescriptor> dirtyTracking, bool ownsSecondStageTranslation)
		MemoryDomainDescriptor(std::shared_ptr<AddressSpaceDescriptor> addressSpace, std::shared_ptr<MemoryTranslationPolicy> translationPolicy, std::shared_ptr<MemoryDomainTranslationControl> translationControl, std::shared_ptr<DirtyTrackingServiceDescriptor> dirtyTracking, bool ownsSecondStageTranslation);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AddressSpaceDescriptor? AddressSpace {get;}
		std::shared_ptr<AddressSpaceDescriptor> getAddressSpace() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemoryTranslationPolicy? TranslationPolicy {get;}
		std::shared_ptr<MemoryTranslationPolicy> getTranslationPolicy() const;

		std::shared_ptr<MemoryDomainTranslationControl> getTranslationControl() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DirtyTrackingServiceDescriptor? DirtyTracking {get;}
		std::shared_ptr<DirtyTrackingServiceDescriptor> getDirtyTracking() const;

		bool getOwnsSecondStageTranslation() const;

		bool isAuthoritativeMemoryStateOwner() const;

		bool getHasAddressSpace() const;

		bool getHasTranslationPolicy() const;

		bool getHasDirtyTracking() const;

		bool getHasValidTranslationControl() const;

		bool TryCreateReadOnlyTranslationView(std::shared_ptr<MemoryDomainReadOnlyTranslationView> &view, std::wstring &reason);

		std::shared_ptr<MemoryDomainDescriptor> WithTranslationControl(std::shared_ptr<MemoryDomainTranslationControl> translationControl);
	};
}
