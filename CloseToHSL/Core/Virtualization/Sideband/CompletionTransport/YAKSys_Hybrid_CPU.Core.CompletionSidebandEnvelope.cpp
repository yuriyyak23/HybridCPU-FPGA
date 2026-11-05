#include "YAKSys_Hybrid_CPU.Core.CompletionSidebandEnvelope.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CompletionSidebandEnvelope::CompletionSidebandEnvelope() : CompletionSidebandEnvelope(CompletionRecord::getNone(), 0, 0, true, EvidenceVisibilityClass::HostOwnedRuntimeEvidence)
	{
	}

	CompletionSidebandEnvelope::CompletionSidebandEnvelope(std::shared_ptr<CompletionRecord> record, unsigned long long routeId, unsigned long long sequence, bool isHostOwnedEvidence, EvidenceVisibilityClass visibilityClass)
		: Record((record != nullptr) ? std::move(record) : CompletionRecord::getNone()),
		  RouteId(routeId),
		  Sequence(sequence),
		  IsHostOwnedEvidence(isHostOwnedEvidence),
		  VisibilityClass(visibilityClass)
	{
	}

	std::shared_ptr<CompletionSidebandEnvelope> CompletionSidebandEnvelope::getEmpty()
	{
		return Empty;
	}

	std::shared_ptr<CompletionRecord> CompletionSidebandEnvelope::getRecord() const
	{
		return Record;
	}

	unsigned long long CompletionSidebandEnvelope::getRouteId() const
	{
		return RouteId;
	}

	unsigned long long CompletionSidebandEnvelope::getSequence() const
	{
		return Sequence;
	}

	bool CompletionSidebandEnvelope::isHostOwnedEvidence() const
	{
		return IsHostOwnedEvidence;
	}

	EvidenceVisibilityClass CompletionSidebandEnvelope::getVisibilityClass() const
	{
		return VisibilityClass;
	}

	bool CompletionSidebandEnvelope::getHasPayload() const
	{
		return !getRecord()->isEmpty();
	}

	bool CompletionSidebandEnvelope::getRequiresHostHandling() const
	{
		return isHostOwnedEvidence() || ([&] { const auto codexPatternValue = getVisibilityClass(); return codexPatternValue == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || codexPatternValue == EvidenceVisibilityClass::SchedulerEvidence || codexPatternValue == EvidenceVisibilityClass::BackendBindingEvidence || codexPatternValue == EvidenceVisibilityClass::NativeTokenEvidence; }());
	}

	bool CompletionSidebandEnvelope::CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		return !isHostOwnedEvidence() && evidencePolicy != nullptr && evidencePolicy->CanExposeToGuest(getVisibilityClass());
	}
}
