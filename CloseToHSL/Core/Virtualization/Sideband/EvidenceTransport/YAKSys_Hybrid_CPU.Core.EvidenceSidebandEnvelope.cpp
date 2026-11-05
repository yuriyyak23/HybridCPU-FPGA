#include "YAKSys_Hybrid_CPU.Core.EvidenceSidebandEnvelope.h"

namespace YAKSys_Hybrid_CPU::Core
{

	EvidenceSidebandEnvelope::EvidenceSidebandEnvelope() : EvidenceSidebandEnvelope(0, 0, 0, EvidenceVisibilityClass::HostOwnedRuntimeEvidence, EvidenceRestorePolicy::RecomputeAfterRestore, true)
	{
	}

	EvidenceSidebandEnvelope::EvidenceSidebandEnvelope(unsigned long long subjectId, unsigned long long sequence, unsigned long long payloadHash, EvidenceVisibilityClass visibilityClass, EvidenceRestorePolicy restorePolicy, bool isHostOwned)
		: SubjectId(subjectId),
		  Sequence(sequence),
		  PayloadHash(payloadHash),
		  VisibilityClass(visibilityClass),
		  RestorePolicy(restorePolicy),
		  IsHostOwned(isHostOwned)
	{
	}

	std::shared_ptr<EvidenceSidebandEnvelope> EvidenceSidebandEnvelope::getEmpty()
	{
		return Empty;
	}

	unsigned long long EvidenceSidebandEnvelope::getSubjectId() const
	{
		return SubjectId;
	}

	unsigned long long EvidenceSidebandEnvelope::getSequence() const
	{
		return Sequence;
	}

	unsigned long long EvidenceSidebandEnvelope::getPayloadHash() const
	{
		return PayloadHash;
	}

	EvidenceVisibilityClass EvidenceSidebandEnvelope::getVisibilityClass() const
	{
		return VisibilityClass;
	}

	EvidenceRestorePolicy EvidenceSidebandEnvelope::getRestorePolicy() const
	{
		return RestorePolicy;
	}

	bool EvidenceSidebandEnvelope::isHostOwned() const
	{
		return IsHostOwned;
	}

	bool EvidenceSidebandEnvelope::getHasPayload() const
	{
		return getPayloadHash() != 0;
	}

	bool EvidenceSidebandEnvelope::getRequiresHostHandling() const
	{
		return isHostOwned() || ([&] { const auto codexPatternValue = getVisibilityClass(); return codexPatternValue == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || codexPatternValue == EvidenceVisibilityClass::SchedulerEvidence || codexPatternValue == EvidenceVisibilityClass::BackendBindingEvidence || codexPatternValue == EvidenceVisibilityClass::NativeTokenEvidence; }());
	}

	bool EvidenceSidebandEnvelope::MustRecomputeAfterRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		return evidencePolicy == nullptr || evidencePolicy->MustRecomputeAfterRestore(getVisibilityClass()) || getRestorePolicy() == EvidenceRestorePolicy::RecomputeAfterRestore;
	}

	bool EvidenceSidebandEnvelope::CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		return !isHostOwned() && evidencePolicy != nullptr && evidencePolicy->CanExposeToGuest(getVisibilityClass());
	}
}
