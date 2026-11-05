#include "RuntimeCapabilityGrantLease.h"
#include <stdexcept>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	RuntimeCapabilityGrantLease::RuntimeCapabilityGrantLease(std::shared_ptr<const void> ownerSeal, unsigned long long grantIdentity, unsigned long long generation, std::shared_ptr<CapabilityGrant> grant) : GrantIdentity(grantIdentity), Generation(generation), Grant(std::move(grant)), _ownerSeal(std::move(ownerSeal)) {}

	unsigned long long RuntimeCapabilityGrantLease::getGrantIdentity() const
	{
		return GrantIdentity;
	}

	unsigned long long RuntimeCapabilityGrantLease::getGeneration() const
	{
		return Generation;
	}

	std::shared_ptr<CapabilityGrant> RuntimeCapabilityGrantLease::getGrant() const
	{
		return Grant;
	}

	bool RuntimeCapabilityGrantLease::WasIssuedBy(std::shared_ptr<const void> ownerSeal)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		return _ownerSeal == ownerSeal;
	}

	unsigned long long RuntimeCapabilityGrantOwner::getCurrentGeneration() const
	{
		std::lock_guard<std::mutex> lock(_sync); return _generation;
	}

	std::shared_ptr<RuntimeCapabilityGrantLease> RuntimeCapabilityGrantOwner::Issue(std::shared_ptr<CapabilityGrant> grant)
	{
		if (!grant) throw std::invalid_argument("grant");
        std::lock_guard<std::mutex> lock(_sync);
		unsigned long long identity = AllocateIdentity();
		std::shared_ptr<RuntimeCapabilityGrantLease> lease = std::make_shared<RuntimeCapabilityGrantLease>(_ownerSeal, identity, _generation, grant);
		for (auto it = _live.begin(); it != _live.end();) { if (it->first.expired()) it = _live.erase(it); else ++it; } _live.emplace(lease, LiveGrant{identity, _generation});
		return lease;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsLive(RuntimeCapabilityGrantLease? lease)
	bool RuntimeCapabilityGrantOwner::IsLive(std::shared_ptr<RuntimeCapabilityGrantLease> lease)
	{
		std::lock_guard<std::mutex> lock(_sync); if (!lease) return false; auto live = _live.find(lease); return lease->WasIssuedBy(_ownerSeal) && lease->getGrantIdentity() != 0 && lease->getGeneration() == _generation && live != _live.end() && live->second.Identity == lease->getGrantIdentity() && live->second.Generation == _generation;
	}

	void RuntimeCapabilityGrantOwner::RevokeAll()
	{
        std::lock_guard<std::mutex> lock(_sync);
//		unchecked
			_generation++;
			if (_generation == 0)
			{
				_generation = 1;
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	unsigned long long RuntimeCapabilityGrantOwner::AllocateIdentity()
	{
		unsigned long long identity = _nextIdentity++;
		if (identity == 0)
		{
			identity = _nextIdentity++;
		}
		return identity;
	}
}
