#include "YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	MemoryDomainDescriptor::MemoryDomainDescriptor() : MemoryDomainDescriptor(nullptr, nullptr, MemoryDomainTranslationControl::Disabled, nullptr, true)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemoryDomainDescriptor(AddressSpaceDescriptor? addressSpace, System.Nullable<MemoryTranslationPolicy> translationPolicy, MemoryDomainTranslationControl translationControl, System.Nullable<DirtyTrackingServiceDescriptor> dirtyTracking, bool ownsSecondStageTranslation)
	MemoryDomainDescriptor::MemoryDomainDescriptor(std::shared_ptr<AddressSpaceDescriptor> addressSpace, std::shared_ptr<MemoryTranslationPolicy> translationPolicy, std::shared_ptr<MemoryDomainTranslationControl> translationControl, std::shared_ptr<DirtyTrackingServiceDescriptor> dirtyTracking, bool ownsSecondStageTranslation) : AddressSpace(addressSpace), TranslationPolicy(translationPolicy), TranslationControl(translationControl), DirtyTracking(dirtyTracking), OwnsSecondStageTranslation(ownsSecondStageTranslation) {}

	std::shared_ptr<AddressSpaceDescriptor> MemoryDomainDescriptor::getAddressSpace() const
	{
		return AddressSpace;
	}

	std::shared_ptr<MemoryTranslationPolicy> MemoryDomainDescriptor::getTranslationPolicy() const
	{
		return TranslationPolicy;
	}

	std::shared_ptr<MemoryDomainTranslationControl> MemoryDomainDescriptor::getTranslationControl() const
	{
		return TranslationControl;
	}

	std::shared_ptr<DirtyTrackingServiceDescriptor> MemoryDomainDescriptor::getDirtyTracking() const
	{
		return DirtyTracking;
	}

	bool MemoryDomainDescriptor::getOwnsSecondStageTranslation() const
	{
		return OwnsSecondStageTranslation;
	}

	bool MemoryDomainDescriptor::isAuthoritativeMemoryStateOwner() const
	{
		return true;
	}

	bool MemoryDomainDescriptor::getHasAddressSpace() const
	{
		return getAddressSpace() != nullptr;
	}

	bool MemoryDomainDescriptor::getHasTranslationPolicy() const
	{
		return getTranslationPolicy() != nullptr;
	}

	bool MemoryDomainDescriptor::getHasDirtyTracking() const
	{
		return getDirtyTracking() != nullptr;
	}

	bool MemoryDomainDescriptor::getHasValidTranslationControl() const
	{
		return getTranslationControl()->isValid();
	}

	bool MemoryDomainDescriptor::TryCreateReadOnlyTranslationView(std::shared_ptr<MemoryDomainReadOnlyTranslationView> &view, std::wstring &reason)
	{
		if (!getHasValidTranslationControl())
		{
			view = nullptr;
			reason = L"Memory domain translation control is not valid.";
			return false;
		}

		view = MemoryDomainReadOnlyTranslationView::From(getTranslationControl(), getOwnsSecondStageTranslation());
		reason = L"Memory domain descriptor exposed a read-only translation view.";
		return true;
	}

	std::shared_ptr<MemoryDomainDescriptor> MemoryDomainDescriptor::WithTranslationControl(std::shared_ptr<MemoryDomainTranslationControl> translationControl)
	{
		return std::make_shared<MemoryDomainDescriptor>(getAddressSpace(), getTranslationPolicy(), translationControl, getDirtyTracking(), getOwnsSecondStageTranslation());
	}
}
