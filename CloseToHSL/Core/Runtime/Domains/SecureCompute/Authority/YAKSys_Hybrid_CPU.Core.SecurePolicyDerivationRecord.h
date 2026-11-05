#pragma once

#include "SecureRevocationEpoch.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecurePolicyDerivationRecord final
	{
	public:
		static const std::shared_ptr<SecurePolicyDerivationRecord> None;

		const unsigned long long ParentPolicyDigest;
		const unsigned long long ChildPolicyDigest;
		const unsigned long long ProvenanceHash;
		const unsigned long long ParentEpoch;
		const unsigned long long ChildEpoch;
		const bool HasProvenance;
		const bool IsSameEpochOrDerived;

		SecurePolicyDerivationRecord(unsigned long long parentPolicyDigest,
			unsigned long long childPolicyDigest,
			unsigned long long provenanceHash,
			unsigned long long parentEpoch,
			unsigned long long childEpoch)
			: ParentPolicyDigest(parentPolicyDigest), ChildPolicyDigest(childPolicyDigest),
			  ProvenanceHash(provenanceHash), ParentEpoch(parentEpoch), ChildEpoch(childEpoch),
			  HasProvenance(parentPolicyDigest != 0 && childPolicyDigest != 0 && provenanceHash != 0),
			  IsSameEpochOrDerived(parentEpoch != 0 && childEpoch >= parentEpoch) {}

		bool MatchesEpoch(const std::shared_ptr<SecureRevocationEpoch> &epoch) const
		{
			return epoch && epoch->IsCurrent(ParentEpoch) && epoch->IsCurrent(ChildEpoch);
		}
	};
}
