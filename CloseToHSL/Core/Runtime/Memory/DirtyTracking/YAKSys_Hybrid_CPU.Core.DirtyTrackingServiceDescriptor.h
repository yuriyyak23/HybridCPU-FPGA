#pragma once

#include "DirtyTrackingServiceDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DirtyTrackingServiceDescriptor final : public std::enable_shared_from_this<DirtyTrackingServiceDescriptor>
	{
	private:
		const DirtyTrackingAuthority Authority;
		const DirtyTrackingVisibility Visibility;
		const bool Enabled;
		const unsigned int MaxTrackedRanges;
		const unsigned long long Epoch;
		const bool RequireFenceBeforeSnapshot;

	public:
		DirtyTrackingServiceDescriptor();

		DirtyTrackingServiceDescriptor(DirtyTrackingAuthority authority, DirtyTrackingVisibility visibility, bool enabled, unsigned int maxTrackedRanges, unsigned long long epoch, bool requireFenceBeforeSnapshot);

		DirtyTrackingAuthority getAuthority() const;

		DirtyTrackingVisibility getVisibility() const;

		bool getEnabled() const;

		unsigned int getMaxTrackedRanges() const;

		unsigned long long getEpoch() const;

		bool getRequireFenceBeforeSnapshot() const;

		bool isRuntimeAuthoritative() const;

		bool getCanTrackWrites() const;

		bool getCanPublishGuestSummary() const;

		bool getCanExposeHostOwnedEvidence() const;

		bool CanTrackRange(unsigned long long guestPhysicalAddress, unsigned long long sizeBytes);

		std::shared_ptr<DirtyTrackingServiceDescriptor> WithEpoch(unsigned long long epoch);
	};
}
