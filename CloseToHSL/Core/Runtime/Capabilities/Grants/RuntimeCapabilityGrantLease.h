#pragma once

#include "YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include <map>
#include <mutex>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Non-serializable identity for one live typed grant. The contained grant remains
	/// policy data; liveness belongs exclusively to its neutral runtime owner.
	/// </summary>
	class RuntimeCapabilityGrantLease final : public std::enable_shared_from_this<RuntimeCapabilityGrantLease>
	{
	private:
		const unsigned long long GrantIdentity;
		const unsigned long long Generation;
		const std::shared_ptr<CapabilityGrant> Grant;

		const std::shared_ptr<const void> _ownerSeal;

	public:
		RuntimeCapabilityGrantLease(std::shared_ptr<const void> ownerSeal, unsigned long long grantIdentity, unsigned long long generation, std::shared_ptr<CapabilityGrant> grant);

		unsigned long long getGrantIdentity() const;
		unsigned long long getGeneration() const;
		std::shared_ptr<CapabilityGrant> getGrant() const;

		bool WasIssuedBy(std::shared_ptr<const void> ownerSeal);
	};

	/// <summary>
	/// Neutral lifecycle owner for generation-bearing typed capability grants.
	/// Revocation advances the generation and invalidates every prior lease.
	/// </summary>
	class RuntimeCapabilityGrantOwner final : public std::enable_shared_from_this<RuntimeCapabilityGrantOwner>
	{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private sealed record LiveGrant(ulong Identity, ulong Generation);

	private:
		const std::shared_ptr<const void> _ownerSeal = std::make_shared<int>(0);
		struct LiveGrant final { unsigned long long Identity, Generation; }; std::map<std::weak_ptr<RuntimeCapabilityGrantLease>, LiveGrant, std::owner_less<std::weak_ptr<RuntimeCapabilityGrantLease>>> _live; mutable std::mutex _sync;
		unsigned long long _generation = 1;
		unsigned long long _nextIdentity = 1;

	public:
		unsigned long long getCurrentGeneration() const;

		std::shared_ptr<RuntimeCapabilityGrantLease> Issue(std::shared_ptr<CapabilityGrant> grant);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsLive(RuntimeCapabilityGrantLease? lease)
		bool IsLive(std::shared_ptr<RuntimeCapabilityGrantLease> lease);

		void RevokeAll();

	private:
		unsigned long long AllocateIdentity();
	};

}
