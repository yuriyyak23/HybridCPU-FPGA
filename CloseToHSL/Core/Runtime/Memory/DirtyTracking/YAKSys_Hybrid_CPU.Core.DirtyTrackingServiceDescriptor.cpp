#include "YAKSys_Hybrid_CPU.Core.DirtyTrackingServiceDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DirtyTrackingServiceDescriptor::DirtyTrackingServiceDescriptor() : DirtyTrackingServiceDescriptor(DirtyTrackingAuthority::RuntimeOwned, DirtyTrackingVisibility::HostOwnedOnly, false, 0, 0, true)
	{
	}

	DirtyTrackingServiceDescriptor::DirtyTrackingServiceDescriptor(DirtyTrackingAuthority authority, DirtyTrackingVisibility visibility, bool enabled, unsigned int maxTrackedRanges, unsigned long long epoch, bool requireFenceBeforeSnapshot)
		: Authority(authority),
		  Visibility(visibility),
		  Enabled(enabled),
		  MaxTrackedRanges(maxTrackedRanges),
		  Epoch(epoch),
		  RequireFenceBeforeSnapshot(requireFenceBeforeSnapshot)
	{
	}

	DirtyTrackingAuthority DirtyTrackingServiceDescriptor::getAuthority() const
	{
		return Authority;
	}

	DirtyTrackingVisibility DirtyTrackingServiceDescriptor::getVisibility() const
	{
		return Visibility;
	}

	bool DirtyTrackingServiceDescriptor::getEnabled() const
	{
		return Enabled;
	}

	unsigned int DirtyTrackingServiceDescriptor::getMaxTrackedRanges() const
	{
		return MaxTrackedRanges;
	}

	unsigned long long DirtyTrackingServiceDescriptor::getEpoch() const
	{
		return Epoch;
	}

	bool DirtyTrackingServiceDescriptor::getRequireFenceBeforeSnapshot() const
	{
		return RequireFenceBeforeSnapshot;
	}

	bool DirtyTrackingServiceDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == DirtyTrackingAuthority::RuntimeOwned;
	}

	bool DirtyTrackingServiceDescriptor::getCanTrackWrites() const
	{
		return isRuntimeAuthoritative() && getEnabled() && getMaxTrackedRanges() != 0;
	}

	bool DirtyTrackingServiceDescriptor::getCanPublishGuestSummary() const
	{
		return getCanTrackWrites() && getVisibility() == DirtyTrackingVisibility::GuestVisibleSummary;
	}

	bool DirtyTrackingServiceDescriptor::getCanExposeHostOwnedEvidence() const
	{
		return false;
	}

	bool DirtyTrackingServiceDescriptor::CanTrackRange(unsigned long long guestPhysicalAddress, unsigned long long sizeBytes)
	{
		if (!getCanTrackWrites() || sizeBytes == 0)
		{
			return false;
		}

		unsigned long long end = guestPhysicalAddress + sizeBytes - 1;
		return end >= guestPhysicalAddress;
	}

	std::shared_ptr<DirtyTrackingServiceDescriptor> DirtyTrackingServiceDescriptor::WithEpoch(unsigned long long epoch)
	{
		return std::make_shared<DirtyTrackingServiceDescriptor>(getAuthority(), getVisibility(), getEnabled(), getMaxTrackedRanges(), epoch, getRequireFenceBeforeSnapshot());
	}
}
