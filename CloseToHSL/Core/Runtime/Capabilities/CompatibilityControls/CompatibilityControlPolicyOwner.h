#pragma once

#include "YAKSys_Hybrid_CPU.Core.CompatibilityControlDescriptor.h"
#include "CompatibilityControlDescriptor.h"
#include <string>
#include <stdexcept>
#include <any>
#include <mutex>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatibilityControlPolicyConstructionProfile
//	{
//		public CompatibilityControlPolicyConstructionProfile(ulong domainIdentity, CompatibilityEventRoutingPolicy eventRoutingPolicy)
//		{
//			if (domainIdentity == 0)
//				throw new ArgumentOutOfRangeException(nameof(domainIdentity));
//			if (eventRoutingPolicy == CompatibilityEventRoutingPolicy.None || (eventRoutingPolicy & ~CompatibilityControlPolicyOwner.KnownEventRoutingPolicyMask) != 0)
//			{
//				throw new ArgumentOutOfRangeException(nameof(eventRoutingPolicy));
//			}
//
//			DomainIdentity = domainIdentity;
//			EventRoutingPolicy = eventRoutingPolicy;
//			IsConfigured = true;
//		}
//
//		public ulong DomainIdentity
//		{
//			get;
//		}
//		public CompatibilityEventRoutingPolicy EventRoutingPolicy
//		{
//			get;
//		}
//		public bool IsConfigured
//		{
//			get;
//		}
//
//		public static CompatibilityControlPolicyConstructionProfile FailClosed(ulong domainIdentity)
//		{
//			return new(domainIdentity, CompatibilityEventRoutingPolicy.RuntimeTrapPolicyRequired | CompatibilityEventRoutingPolicy.NeutralTrapResultRequired | CompatibilityEventRoutingPolicy.PublicationFenceRequired);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatibilityControlPolicyIdentity(ulong OwnerIdentity, ulong OwnerEpoch, ulong DomainIdentity, ulong PolicyGeneration)
//	{
//		public bool IsMaterialized
//		{
//			get
//			{
//				return OwnerIdentity != 0 && OwnerEpoch != 0 && DomainIdentity != 0 && PolicyGeneration != 0;
//			}
//		}
//	}

	struct CompatibilityControlPolicyConstructionProfile final {
    const unsigned long long DomainIdentity;
    const CompatibilityEventRoutingPolicy EventRoutingPolicy;
    const bool IsConfigured;
    CompatibilityControlPolicyConstructionProfile(unsigned long long domain, CompatibilityEventRoutingPolicy policy) : DomainIdentity(domain), EventRoutingPolicy(policy), IsConfigured(true) {
        const auto bits = static_cast<unsigned long long>(policy);
        if (!domain || bits == 0 || (bits & ~7ULL) != 0) throw std::invalid_argument("Invalid compatibility control profile");
    }
    static CompatibilityControlPolicyConstructionProfile FailClosed(unsigned long long domain) { return {domain, static_cast<CompatibilityEventRoutingPolicy>(7)}; }
};
struct CompatibilityControlPolicyIdentity final {
    unsigned long long OwnerIdentity = 0, OwnerEpoch = 0, DomainIdentity = 0, PolicyGeneration = 0;
    bool isMaterialized() const noexcept { return OwnerIdentity != 0 && OwnerEpoch != 0 && DomainIdentity != 0 && PolicyGeneration != 0; }
    bool operator==(const CompatibilityControlPolicyIdentity&) const = default;
};
class CompatibilityControlPolicySnapshot final : public std::enable_shared_from_this<CompatibilityControlPolicySnapshot>
	{
	private:
		const std::shared_ptr<CompatibilityControlPolicyIdentity> Identity;
		const std::shared_ptr<CompatibilityControlDescriptor> Descriptor;

		const std::any _issuerSeal;

	public:
		CompatibilityControlPolicySnapshot(std::any issuerSeal, std::shared_ptr<CompatibilityControlPolicyIdentity> identity, std::shared_ptr<CompatibilityControlDescriptor> descriptor);

		std::shared_ptr<CompatibilityControlPolicyIdentity> getIdentity() const;
		std::shared_ptr<CompatibilityControlDescriptor> getDescriptor() const;
		CompatibilityEventRoutingPolicy getEventRoutingPolicy() const;
		bool isMaterialized() const;
		bool getRuntimeAuthorityGranted() const;
		bool isCompatibilityValue() const;

		bool WasIssuedBy(std::any issuerSeal);
	};

	class CompatibilityControlPolicyOwner final : public std::enable_shared_from_this<CompatibilityControlPolicyOwner>
	{
	public:
		static constexpr unsigned long long CanonicalOwnerIdentity = 0x4343'504F'4C4F'574EULL;
		static constexpr CompatibilityEventRoutingPolicy KnownEventRoutingPolicyMask = static_cast<CompatibilityEventRoutingPolicy>(7);

	private:
		mutable std::mutex _sync;
		const std::any _issuerSeal = std::make_shared<const unsigned char>(0);
		unsigned long long _ownerEpoch = 1;
		unsigned long long _domainIdentity = 0;
		unsigned long long _policyGeneration = 0;
		std::shared_ptr<CompatibilityControlPolicySnapshot> _current;

	public:
		CompatibilityControlPolicyOwner(std::shared_ptr<CompatibilityControlPolicyConstructionProfile> profile);

		std::shared_ptr<CompatibilityControlPolicySnapshot> CaptureCurrent();

		unsigned long long getCurrentDomainIdentity() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool IsCurrent(CompatibilityControlPolicySnapshot? snapshot)
		bool IsCurrent(std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot);

		std::shared_ptr<CompatibilityControlPolicySnapshot> ReplacePolicy(CompatibilityEventRoutingPolicy eventRoutingPolicy);

		std::shared_ptr<CompatibilityControlPolicySnapshot> RebindDomain(unsigned long long domainIdentity);

		std::shared_ptr<CompatibilityControlPolicySnapshot> InvalidateAfterRestore();

		std::shared_ptr<CompatibilityControlPolicySnapshot> ReplaceAfterArchitecturalStateReplacement();

	private:
		std::shared_ptr<CompatibilityControlPolicySnapshot> IssueSnapshot(CompatibilityEventRoutingPolicy eventRoutingPolicy);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool IsCurrentUnderLock(CompatibilityControlPolicySnapshot? snapshot)
		bool IsCurrentUnderLock(std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot);

		static void ValidatePolicy(CompatibilityEventRoutingPolicy policy);

		static void AdvanceNonZero(unsigned long long &value);
	};

	enum class NeutralInterruptRoutingPolicyDecision : unsigned char
	{
		Allowed = 0,
		DeniedStaleOrForeignSnapshot = 1,
		DeniedDomainMismatch = 2,
		DeniedRuntimeTrapPolicy = 3,
		DeniedNeutralTrapResult = 4,
		DeniedPublicationFence = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralInterruptRoutingPolicyResult(NeutralInterruptRoutingPolicyDecision Decision, string Reason)
//	{
//		internal bool IsAllowed
//		{
//			get
//			{
//				return Decision == NeutralInterruptRoutingPolicyDecision.Allowed;
//			}
//		}
//	}

	struct NeutralInterruptRoutingPolicyResult final {
    const NeutralInterruptRoutingPolicyDecision Decision;
    const std::wstring Reason;
    NeutralInterruptRoutingPolicyResult(NeutralInterruptRoutingPolicyDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == NeutralInterruptRoutingPolicyDecision::Allowed; }
    bool IsAllowed() const noexcept { return isAllowed(); }
};
class NeutralInterruptRoutingPolicyConsumer final : public std::enable_shared_from_this<NeutralInterruptRoutingPolicyConsumer>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal NeutralInterruptRoutingPolicyResult Validate(CompatibilityControlPolicyOwner owner, CompatibilityControlPolicySnapshot? snapshot, ulong expectedDomainIdentity)
		std::shared_ptr<NeutralInterruptRoutingPolicyResult> Validate(std::shared_ptr<CompatibilityControlPolicyOwner> owner, std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot, unsigned long long expectedDomainIdentity);

	private:
		static std::shared_ptr<NeutralInterruptRoutingPolicyResult> Denied(NeutralInterruptRoutingPolicyDecision decision, const std::wstring &reason);
	};

}
