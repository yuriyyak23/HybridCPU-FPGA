#pragma once
#include "../../Authority/SecureGrantHandle.h"
#include "../../Policies/Evidence/SecureEvidencePolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureIoDmaPolicy : unsigned char
	{
		Denied = 0,
		ExplicitSharedBuffersOnly = 1
	};

	enum class SecureSharedBufferDirection : unsigned char
	{
		None = 0,
		DomainToDevice = 1,
		DeviceToDomain = 2,
		Bidirectional = 3
	};

struct SecureSharedBufferDescriptor final {
    unsigned long long BufferId = 0, Start = 0, Length = 0;
    SecureSharedBufferDirection Direction = SecureSharedBufferDirection::None;
    SecureGrantHandle Grant;
    SecureEvidenceVisibilityClass EvidenceClass = SecureEvidenceVisibilityClass::Denied;
    unsigned long long OwnerDomainTag = 0, LifetimeEpoch = 0;
    bool isMaterialized() const noexcept { return BufferId != 0 && Length != 0 && Direction != SecureSharedBufferDirection::None && Grant.isMaterialized() && EvidenceClass != SecureEvidenceVisibilityClass::Denied && OwnerDomainTag != 0 && LifetimeEpoch != 0; }
    bool Contains(unsigned long long address, unsigned long long length) const noexcept { if (!Length || !length || address < Start) return false; auto offset = address - Start; return offset < Length && length <= Length - offset; }
    bool AllowsDirection(SecureSharedBufferDirection required) const noexcept { return required != SecureSharedBufferDirection::None && (Direction == SecureSharedBufferDirection::Bidirectional || Direction == required); }
    bool IsOwnedBy(unsigned long long domain) const noexcept { return OwnerDomainTag != 0 && OwnerDomainTag == domain; }
    bool HasCurrentLifetime(const SecureRevocationEpoch& epoch) const noexcept { return epoch.IsCurrent(LifetimeEpoch); }
};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureSharedBufferDescriptor(ulong BufferId, ulong Start, ulong Length, SecureSharedBufferDirection Direction, SecureGrantHandle Grant, SecureEvidenceVisibilityClass EvidenceClass, ulong OwnerDomainTag, ulong LifetimeEpoch)
//	{
//		public bool IsMaterialized
//		{
//			get
//			{
//				return BufferId != 0 && Length != 0 && Direction != SecureSharedBufferDirection.None && Grant.IsMaterialized && EvidenceClass != SecureEvidenceVisibilityClass.Denied && OwnerDomainTag != 0 && LifetimeEpoch != 0;
//			}
//		}
//
//		public bool Contains(ulong address, ulong length)
//		{
//			if (Length == 0 || length == 0 || address < Start)
//			{
//				return false;
//			}
//
//			ulong offset = address - Start;
//			return offset < Length && length <= Length - offset;
//		}
//
//		public bool AllowsDirection(SecureSharedBufferDirection requiredDirection)
//		{
//			return requiredDirection != SecureSharedBufferDirection.None && (Direction == SecureSharedBufferDirection.Bidirectional || Direction == requiredDirection);
//		}
//
//		public bool IsOwnedBy(ulong domainTag)
//		{
//			return OwnerDomainTag != 0 && OwnerDomainTag == domainTag;
//		}
//
//		public bool HasCurrentLifetime(SecureRevocationEpoch policyEpoch)
//		{
//			return policyEpoch.IsCurrent(LifetimeEpoch);
//		}
//	}


}
