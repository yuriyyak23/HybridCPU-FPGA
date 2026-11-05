#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	enum class EvidenceVisibilityClass : unsigned char;

	enum class GuestVisibleEvidenceProjectionDecision : unsigned char
	{
		Allowed = 0,
		DeniedByEvidencePolicy = 1,
		DeniedByObservabilityPolicy = 2,
		DeniedHostOwnedEvidence = 3,
		DeniedEmptyPayload = 4
	};

	struct GuestVisibleEvidenceRecord final
	{
		const unsigned long long SubjectId;
		const unsigned long long Sequence;
		const unsigned long long PayloadHash;
		const EvidenceVisibilityClass VisibilityClass;

		GuestVisibleEvidenceRecord(unsigned long long subjectId, unsigned long long sequence,
			unsigned long long payloadHash, EvidenceVisibilityClass visibilityClass)
			: SubjectId(subjectId), Sequence(sequence), PayloadHash(payloadHash), VisibilityClass(visibilityClass) {}
	};
}
