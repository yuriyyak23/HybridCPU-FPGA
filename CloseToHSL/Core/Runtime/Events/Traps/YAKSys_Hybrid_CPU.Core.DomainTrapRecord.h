#pragma once

#include "DomainTrapRecord.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainTrapRecord final : public std::enable_shared_from_this<DomainTrapRecord>
	{
	private:
		const DomainTrapRecordAuthority Authority;
		const unsigned long long Sequence;
		const unsigned long long DomainId;
		const bool ShouldExit;
		const unsigned long long DescriptorEpoch;
		const unsigned long long EvidenceHash;
		const bool AllowsCompatibilityProjection;

	public:
		DomainTrapRecord();

		DomainTrapRecord(DomainTrapRecordAuthority authority, unsigned long long sequence, unsigned long long domainId, bool shouldExit, unsigned long long descriptorEpoch, unsigned long long evidenceHash, bool allowsCompatibilityProjection);

		DomainTrapRecordAuthority getAuthority() const;

		unsigned long long getSequence() const;

		unsigned long long getDomainId() const;

		bool getShouldExit() const;

		unsigned long long getDescriptorEpoch() const;

		unsigned long long getEvidenceHash() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasDomainBinding() const;

		bool getHasEvidenceHash() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<DomainTrapRecord> WithSequence(unsigned long long sequence);
	};
}
