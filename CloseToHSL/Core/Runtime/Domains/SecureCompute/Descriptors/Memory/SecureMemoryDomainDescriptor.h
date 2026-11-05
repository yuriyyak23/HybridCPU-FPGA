#pragma once
#include "../../Authority/SecureRevocationEpoch.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureMemoryRegionClass : unsigned char
	{
		Private = 0,
		Shared = 1,
		Measured = 2,
		RuntimeMutable = 3
	};

	enum class SecureMemoryHostVisibility : unsigned char
	{
		Denied = 0,
		MetadataOnly = 1,
		ExplicitShared = 2
	};

	enum class SecureMemoryDmaPolicy : unsigned char
	{
		Denied = 0,
		ExplicitSharedBuffersWithTypedGrant = 1
	};

	enum class SecureRuntimeMutableDirtyPolicy : unsigned char
	{
		None = 0,
		TrackDirtyPages = 1,
		ReinitializeOnRestore = 2
	};

	enum class SecureRuntimeMutableMigrationClass : unsigned char
	{
		Unclassified = 0,
		DomainLocal = 1,
		RecomputeAfterRestore = 2,
		SealedPayloadRequired = 3
	};

struct SecureMemoryRegionDescriptor final {
    const SecureMemoryRegionClass RegionClass;
    const unsigned long long Start, Length;
    const SecureMemoryHostVisibility HostVisibility;
    const unsigned long long PolicyEpoch;
    const SecureRuntimeMutableDirtyPolicy RuntimeDirtyPolicy;
    const SecureRuntimeMutableMigrationClass RuntimeMigrationClass;
    const bool IsEmpty, IsPrivate, IsShared, IsMeasured, IsRuntimeMutable, IsHostReadable, HasRuntimeMutableClassification;
    SecureMemoryRegionDescriptor(SecureMemoryRegionClass regionClass = SecureMemoryRegionClass::Private, unsigned long long start = 0, unsigned long long length = 0, SecureMemoryHostVisibility visibility = SecureMemoryHostVisibility::Denied, unsigned long long epoch = 0, SecureRuntimeMutableDirtyPolicy dirty = SecureRuntimeMutableDirtyPolicy::None, SecureRuntimeMutableMigrationClass migration = SecureRuntimeMutableMigrationClass::Unclassified)
        : RegionClass(regionClass), Start(start), Length(length), HostVisibility(visibility), PolicyEpoch(epoch), RuntimeDirtyPolicy(dirty), RuntimeMigrationClass(migration),
          IsEmpty(length == 0), IsPrivate(regionClass == SecureMemoryRegionClass::Private), IsShared(regionClass == SecureMemoryRegionClass::Shared), IsMeasured(regionClass == SecureMemoryRegionClass::Measured), IsRuntimeMutable(regionClass == SecureMemoryRegionClass::RuntimeMutable), IsHostReadable(visibility == SecureMemoryHostVisibility::ExplicitShared),
          HasRuntimeMutableClassification(regionClass != SecureMemoryRegionClass::RuntimeMutable || (dirty != SecureRuntimeMutableDirtyPolicy::None && migration != SecureRuntimeMutableMigrationClass::Unclassified)) {}
    bool Contains(unsigned long long address, unsigned long long length) const {
        if (IsEmpty || length == 0 || address < Start) return false;
        const auto offset = address - Start; return offset < Length && length <= Length - offset;
    }
    bool IsCurrentFor(std::shared_ptr<SecureRevocationEpoch> epoch) const { return epoch && epoch->IsCurrent(PolicyEpoch); }
};
}