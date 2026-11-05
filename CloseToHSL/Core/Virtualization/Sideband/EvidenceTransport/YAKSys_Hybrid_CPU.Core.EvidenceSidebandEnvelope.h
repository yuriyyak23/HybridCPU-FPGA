#pragma once

#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class EvidenceSidebandEnvelope final : public std::enable_shared_from_this<EvidenceSidebandEnvelope>
	{
	private:
		static inline const std::shared_ptr<EvidenceSidebandEnvelope> Empty = std::make_shared<EvidenceSidebandEnvelope>();
		const unsigned long long SubjectId;
		const unsigned long long Sequence;
		const unsigned long long PayloadHash;
		const EvidenceVisibilityClass VisibilityClass;
		const EvidenceRestorePolicy RestorePolicy;
		const bool IsHostOwned;

	public:
		EvidenceSidebandEnvelope();

		EvidenceSidebandEnvelope(unsigned long long subjectId, unsigned long long sequence, unsigned long long payloadHash, EvidenceVisibilityClass visibilityClass, EvidenceRestorePolicy restorePolicy, bool isHostOwned);

		static std::shared_ptr<EvidenceSidebandEnvelope> getEmpty();

		unsigned long long getSubjectId() const;

		unsigned long long getSequence() const;

		unsigned long long getPayloadHash() const;

		EvidenceVisibilityClass getVisibilityClass() const;

		EvidenceRestorePolicy getRestorePolicy() const;

		bool isHostOwned() const;

		bool getHasPayload() const;

		bool getRequiresHostHandling() const;

		bool MustRecomputeAfterRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

		bool CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);
	};
}
