#include "ReplayEngine.Types.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::Core
{
	namespace
	{
		const wchar_t *ReplayEnvelopeKindName(ReplayEnvelopeKind kind)
		{
		switch (kind)
		{
		case ReplayEnvelopeKind::TimingNoise: return L"TimingNoise";
		case ReplayEnvelopeKind::ResourcePressure: return L"ResourcePressure";
		case ReplayEnvelopeKind::Mixed: return L"Mixed";
		default: return L"Unknown";
		}
		}

		const wchar_t *ReplayInvalidationReasonName(ReplayPhaseInvalidationReason reason)
		{
		switch (reason)
		{
		case ReplayPhaseInvalidationReason::None: return L"None";
		case ReplayPhaseInvalidationReason::Completed: return L"Completed";
		case ReplayPhaseInvalidationReason::PcMismatch: return L"PcMismatch";
		case ReplayPhaseInvalidationReason::Manual: return L"Manual";
		case ReplayPhaseInvalidationReason::CertificateMutation: return L"CertificateMutation";
		case ReplayPhaseInvalidationReason::PhaseMismatch: return L"PhaseMismatch";
		case ReplayPhaseInvalidationReason::InactivePhase: return L"InactivePhase";
		case ReplayPhaseInvalidationReason::DomainBoundary: return L"DomainBoundary";
		case ReplayPhaseInvalidationReason::ClassCapacityMismatch: return L"ClassCapacityMismatch";
		case ReplayPhaseInvalidationReason::ClassTemplateExpired: return L"ClassTemplateExpired";
		case ReplayPhaseInvalidationReason::SerializingEvent: return L"SerializingEvent";
		case ReplayPhaseInvalidationReason::MemoryFootprintOverlap: return L"MemoryFootprintOverlap";
		case ReplayPhaseInvalidationReason::DmaStreamComputeDescriptorMismatch: return L"DmaStreamComputeDescriptorMismatch";
		case ReplayPhaseInvalidationReason::DmaStreamComputeDescriptorPayloadLost: return L"DmaStreamComputeDescriptorPayloadLost";
		case ReplayPhaseInvalidationReason::DmaStreamComputeCarrierMismatch: return L"DmaStreamComputeCarrierMismatch";
		case ReplayPhaseInvalidationReason::DmaStreamComputeFootprintMismatch: return L"DmaStreamComputeFootprintMismatch";
		case ReplayPhaseInvalidationReason::DmaStreamComputeOwnerDomainMismatch: return L"DmaStreamComputeOwnerDomainMismatch";
		case ReplayPhaseInvalidationReason::DmaStreamComputeCertificateInputMismatch: return L"DmaStreamComputeCertificateInputMismatch";
		case ReplayPhaseInvalidationReason::DmaStreamComputeTokenEvidenceMismatch: return L"DmaStreamComputeTokenEvidenceMismatch";
		case ReplayPhaseInvalidationReason::DmaStreamComputeLanePlacementMismatch: return L"DmaStreamComputeLanePlacementMismatch";
		case ReplayPhaseInvalidationReason::DmaStreamComputeIncompleteEvidence: return L"DmaStreamComputeIncompleteEvidence";
		default: return L"Unknown";
		}
		}
	}

	ReplayDeterminismReport::ReplayDeterminismReport(bool isDeterministic, int comparedEvents, int comparedReplayEvents, int comparedTimelineSamples, int comparedInvalidationEvents, int comparedEpochs, int mismatchThreadId, long long mismatchCycle, const std::wstring &mismatchField, const std::wstring &expectedValue, const std::wstring &actualValue)
		: IsDeterministic(isDeterministic), ComparedEvents(comparedEvents), ComparedReplayEvents(comparedReplayEvents), ComparedTimelineSamples(comparedTimelineSamples), ComparedInvalidationEvents(comparedInvalidationEvents), ComparedEpochs(comparedEpochs), MismatchThreadId(mismatchThreadId), MismatchCycle(mismatchCycle), MismatchField(mismatchField), ExpectedValue(expectedValue), ActualValue(actualValue), TypedSlotAgreementValid(true)
	{
	}

	ReplayDeterminismReport::ReplayDeterminismReport(bool isDeterministic, int comparedEvents, int comparedReplayEvents, int comparedTimelineSamples, int comparedInvalidationEvents, int comparedEpochs, int mismatchThreadId, long long mismatchCycle, const std::wstring &mismatchField, const std::wstring &expectedValue, const std::wstring &actualValue, bool typedSlotAgreementValid)
		: IsDeterministic(isDeterministic), ComparedEvents(comparedEvents), ComparedReplayEvents(comparedReplayEvents), ComparedTimelineSamples(comparedTimelineSamples), ComparedInvalidationEvents(comparedInvalidationEvents), ComparedEpochs(comparedEpochs), MismatchThreadId(mismatchThreadId), MismatchCycle(mismatchCycle), MismatchField(mismatchField), ExpectedValue(expectedValue), ActualValue(actualValue), TypedSlotAgreementValid(typedSlotAgreementValid)
	{
	}

	bool ReplayDeterminismReport::isDeterministic() const
	{
		return IsDeterministic;
	}

	int ReplayDeterminismReport::getComparedEvents() const
	{
		return ComparedEvents;
	}

	int ReplayDeterminismReport::getComparedReplayEvents() const
	{
		return ComparedReplayEvents;
	}

	int ReplayDeterminismReport::getComparedTimelineSamples() const
	{
		return ComparedTimelineSamples;
	}

	int ReplayDeterminismReport::getComparedInvalidationEvents() const
	{
		return ComparedInvalidationEvents;
	}

	int ReplayDeterminismReport::getComparedEpochs() const
	{
		return ComparedEpochs;
	}

	int ReplayDeterminismReport::getMismatchThreadId() const
	{
		return MismatchThreadId;
	}

	long long ReplayDeterminismReport::getMismatchCycle() const
	{
		return MismatchCycle;
	}

	std::wstring ReplayDeterminismReport::getMismatchField() const
	{
		return MismatchField;
	}

	std::wstring ReplayDeterminismReport::getExpectedValue() const
	{
		return ExpectedValue;
	}

	std::wstring ReplayDeterminismReport::getActualValue() const
	{
		return ActualValue;
	}

	bool ReplayDeterminismReport::getTypedSlotAgreementValid() const
	{
		return TypedSlotAgreementValid;
	}

	std::wstring ReplayDeterminismReport::Describe() const
	{
		if (isDeterministic())
		{
			return std::format(L"Deterministic across {} events, {} replay-phase events, {} dense timeline samples and {} invalidation samples.", getComparedEvents(), getComparedReplayEvents(), getComparedTimelineSamples(), getComparedInvalidationEvents());
		}

		return std::format(L"Diverged at thread {}, cycle {}, field '{}': expected '{}', actual '{}'.", getMismatchThreadId(), getMismatchCycle(), getMismatchField(), getExpectedValue(), getActualValue());
	}

	ReplayEnvelopeConfiguration::ReplayEnvelopeConfiguration(ReplayEnvelopeKind kind, long long maxCycleDrift, long long maxMemorySubsystemCycleDrift, int maxActiveMemoryRequestDelta, int maxBankQueueDepthDelta, int maxReadyQueueDepthDelta)
		: Kind(kind), MaxCycleDrift(maxCycleDrift), MaxMemorySubsystemCycleDrift(maxMemorySubsystemCycleDrift), MaxActiveMemoryRequestDelta(maxActiveMemoryRequestDelta), MaxBankQueueDepthDelta(maxBankQueueDepthDelta), MaxReadyQueueDepthDelta(maxReadyQueueDepthDelta)
	{
	}

	ReplayEnvelopeKind ReplayEnvelopeConfiguration::getKind() const
	{
		return Kind;
	}

	long long ReplayEnvelopeConfiguration::getMaxCycleDrift() const
	{
		return MaxCycleDrift;
	}

	long long ReplayEnvelopeConfiguration::getMaxMemorySubsystemCycleDrift() const
	{
		return MaxMemorySubsystemCycleDrift;
	}

	int ReplayEnvelopeConfiguration::getMaxActiveMemoryRequestDelta() const
	{
		return MaxActiveMemoryRequestDelta;
	}

	int ReplayEnvelopeConfiguration::getMaxBankQueueDepthDelta() const
	{
		return MaxBankQueueDepthDelta;
	}

	int ReplayEnvelopeConfiguration::getMaxReadyQueueDepthDelta() const
	{
		return MaxReadyQueueDepthDelta;
	}

	ReplayEnvelopeConfiguration ReplayEnvelopeConfiguration::CreateTimingNoise(long long maxCycleDrift)
	{
		return ReplayEnvelopeConfiguration(ReplayEnvelopeKind::TimingNoise, maxCycleDrift, maxCycleDrift, 0, 0, 0);
	}

	ReplayEnvelopeConfiguration ReplayEnvelopeConfiguration::CreateResourcePressure(int maxActiveMemoryRequestDelta, int maxBankQueueDepthDelta, int maxReadyQueueDepthDelta)
	{
		return ReplayEnvelopeConfiguration(ReplayEnvelopeKind::ResourcePressure, 0, 0, maxActiveMemoryRequestDelta, maxBankQueueDepthDelta, maxReadyQueueDepthDelta);
	}

	ReplayEnvelopeConfiguration ReplayEnvelopeConfiguration::CreateMixed(long long maxCycleDrift, long long maxMemorySubsystemCycleDrift, int maxActiveMemoryRequestDelta, int maxBankQueueDepthDelta, int maxReadyQueueDepthDelta)
	{
		return ReplayEnvelopeConfiguration(ReplayEnvelopeKind::Mixed, maxCycleDrift, maxMemorySubsystemCycleDrift, maxActiveMemoryRequestDelta, maxBankQueueDepthDelta, maxReadyQueueDepthDelta);
	}

	ReplayEnvelopeReport::ReplayEnvelopeReport(bool isWithinEnvelope, ReplayEnvelopeKind kind, int comparedEvents, int comparedReplayEvents, int comparedEnvelopeFields, int mismatchThreadId, long long mismatchCycle, const std::wstring &mismatchField, const std::wstring &expectedValue, const std::wstring &actualValue, const std::wstring &allowedEnvelope)
		: IsWithinEnvelope(isWithinEnvelope), Kind(kind), ComparedEvents(comparedEvents), ComparedReplayEvents(comparedReplayEvents), ComparedEnvelopeFields(comparedEnvelopeFields), MismatchThreadId(mismatchThreadId), MismatchCycle(mismatchCycle), MismatchField(mismatchField), ExpectedValue(expectedValue), ActualValue(actualValue), AllowedEnvelope(allowedEnvelope)
	{
	}

	bool ReplayEnvelopeReport::isWithinEnvelope() const
	{
		return IsWithinEnvelope;
	}

	ReplayEnvelopeKind ReplayEnvelopeReport::getKind() const
	{
		return Kind;
	}

	int ReplayEnvelopeReport::getComparedEvents() const
	{
		return ComparedEvents;
	}

	int ReplayEnvelopeReport::getComparedReplayEvents() const
	{
		return ComparedReplayEvents;
	}

	int ReplayEnvelopeReport::getComparedEnvelopeFields() const
	{
		return ComparedEnvelopeFields;
	}

	int ReplayEnvelopeReport::getMismatchThreadId() const
	{
		return MismatchThreadId;
	}

	long long ReplayEnvelopeReport::getMismatchCycle() const
	{
		return MismatchCycle;
	}

	std::wstring ReplayEnvelopeReport::getMismatchField() const
	{
		return MismatchField;
	}

	std::wstring ReplayEnvelopeReport::getExpectedValue() const
	{
		return ExpectedValue;
	}

	std::wstring ReplayEnvelopeReport::getActualValue() const
	{
		return ActualValue;
	}

	std::wstring ReplayEnvelopeReport::getAllowedEnvelope() const
	{
		return AllowedEnvelope;
	}

	std::wstring ReplayEnvelopeReport::Describe() const
	{
		if (isWithinEnvelope())
		{
			return std::format(L"Within {} envelope across {} events, {} replay-phase events and {} bounded timing/resource comparisons.", ReplayEnvelopeKindName(getKind()), getComparedEvents(), getComparedReplayEvents(), getComparedEnvelopeFields());
		}

		return std::format(L"Out of {} envelope at thread {}, cycle {}, field '{}': expected '{}', actual '{}', allowed {}.", ReplayEnvelopeKindName(getKind()), getMismatchThreadId(), getMismatchCycle(), getMismatchField(), getExpectedValue(), getActualValue(), getAllowedEnvelope());
	}

	ReplayTraceEvidenceSummary::ReplayTraceEvidenceSummary(int totalEvents, int replayPhaseEvents, int denseTimelineSamples, int writeBackSamples, int replayEpochCount, unsigned long long totalEpochLength, unsigned long long stableDonorSlotSamples, unsigned long long totalReplaySlotSamples, long long phaseCertificateReadyHits, long long phaseCertificateReadyMisses, long long estimatedPhaseCertificateChecksSaved, long long phaseCertificateInvalidations, int invalidationObservations, int mutationInvalidationObservations, int phaseMismatchInvalidationObservations, long long eligibilityMaskedCycles, long long eligibilityMaskedReadyCandidates, int invalidationBursts, int longestInvalidationBurst)
		: TotalEvents(totalEvents), ReplayPhaseEvents(replayPhaseEvents), DenseTimelineSamples(denseTimelineSamples), WriteBackSamples(writeBackSamples), ReplayEpochCount(replayEpochCount), TotalEpochLength(totalEpochLength), StableDonorSlotSamples(stableDonorSlotSamples), TotalReplaySlotSamples(totalReplaySlotSamples), PhaseCertificateReadyHits(phaseCertificateReadyHits), PhaseCertificateReadyMisses(phaseCertificateReadyMisses), EstimatedPhaseCertificateChecksSaved(estimatedPhaseCertificateChecksSaved), PhaseCertificateInvalidations(phaseCertificateInvalidations), InvalidationObservations(invalidationObservations), MutationInvalidationObservations(mutationInvalidationObservations), PhaseMismatchInvalidationObservations(phaseMismatchInvalidationObservations), EligibilityMaskedCycles(eligibilityMaskedCycles), EligibilityMaskedReadyCandidates(eligibilityMaskedReadyCandidates), InvalidationBursts(invalidationBursts), LongestInvalidationBurst(longestInvalidationBurst)
	{
	}

	int ReplayTraceEvidenceSummary::getTotalEvents() const
	{
		return TotalEvents;
	}

	int ReplayTraceEvidenceSummary::getReplayPhaseEvents() const
	{
		return ReplayPhaseEvents;
	}

	int ReplayTraceEvidenceSummary::getDenseTimelineSamples() const
	{
		return DenseTimelineSamples;
	}

	int ReplayTraceEvidenceSummary::getWriteBackSamples() const
	{
		return WriteBackSamples;
	}

	int ReplayTraceEvidenceSummary::getReplayEpochCount() const
	{
		return ReplayEpochCount;
	}

	unsigned long long ReplayTraceEvidenceSummary::getTotalEpochLength() const
	{
		return TotalEpochLength;
	}

	unsigned long long ReplayTraceEvidenceSummary::getStableDonorSlotSamples() const
	{
		return StableDonorSlotSamples;
	}

	unsigned long long ReplayTraceEvidenceSummary::getTotalReplaySlotSamples() const
	{
		return TotalReplaySlotSamples;
	}

	long long ReplayTraceEvidenceSummary::getPhaseCertificateReadyHits() const
	{
		return PhaseCertificateReadyHits;
	}

	long long ReplayTraceEvidenceSummary::getPhaseCertificateReadyMisses() const
	{
		return PhaseCertificateReadyMisses;
	}

	long long ReplayTraceEvidenceSummary::getEstimatedPhaseCertificateChecksSaved() const
	{
		return EstimatedPhaseCertificateChecksSaved;
	}

	long long ReplayTraceEvidenceSummary::getPhaseCertificateInvalidations() const
	{
		return PhaseCertificateInvalidations;
	}

	int ReplayTraceEvidenceSummary::getInvalidationObservations() const
	{
		return InvalidationObservations;
	}

	int ReplayTraceEvidenceSummary::getMutationInvalidationObservations() const
	{
		return MutationInvalidationObservations;
	}

	int ReplayTraceEvidenceSummary::getPhaseMismatchInvalidationObservations() const
	{
		return PhaseMismatchInvalidationObservations;
	}

	long long ReplayTraceEvidenceSummary::getEligibilityMaskedCycles() const
	{
		return EligibilityMaskedCycles;
	}

	long long ReplayTraceEvidenceSummary::getEligibilityMaskedReadyCandidates() const
	{
		return EligibilityMaskedReadyCandidates;
	}

	int ReplayTraceEvidenceSummary::getInvalidationBursts() const
	{
		return InvalidationBursts;
	}

	int ReplayTraceEvidenceSummary::getLongestInvalidationBurst() const
	{
		return LongestInvalidationBurst;
	}

	double ReplayTraceEvidenceSummary::getAverageEpochLength() const
	{
		return getReplayEpochCount() == 0 ? 0.0 : static_cast<double>(getTotalEpochLength()) / getReplayEpochCount();
	}

	double ReplayTraceEvidenceSummary::getStableDonorSlotRatio() const
	{
		return getTotalReplaySlotSamples() == 0 ? 0.0 : static_cast<double>(getStableDonorSlotSamples()) / getTotalReplaySlotSamples();
	}

	double ReplayTraceEvidenceSummary::getDenseTimelineCoverage() const
	{
		return getReplayPhaseEvents() == 0 ? 0.0 : static_cast<double>(getDenseTimelineSamples()) / getReplayPhaseEvents();
	}

	double ReplayTraceEvidenceSummary::getPhaseCertificateReuseHitRate() const
	{
		long long totalReadinessChecks = getPhaseCertificateReadyHits() + getPhaseCertificateReadyMisses();
		if (totalReadinessChecks == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getPhaseCertificateReadyHits()) / totalReadinessChecks;
	}

	std::wstring ReplayTraceEvidenceSummary::Describe() const
	{
// C# TO C++ CONVERTER TASK: The '3:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '4:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '5:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '6:N0' format specifier is not converted to C++:
		return std::format(L"Replay-heavy trace: {} epochs, {} replay events, {} dense timeline samples, reuse hit-rate {:.2f}%, checks saved {}, eligibility-masked cycles {}, masked ready candidates {}, invalidation bursts {} (max {}).", getReplayEpochCount(), getReplayPhaseEvents(), getDenseTimelineSamples(), getPhaseCertificateReuseHitRate() * 100.0, getEstimatedPhaseCertificateChecksSaved(), getEligibilityMaskedCycles(), getEligibilityMaskedReadyCandidates(), getInvalidationBursts(), getLongestInvalidationBurst());
	}

	ReplayEpochEvidenceSummary::ReplayEpochEvidenceSummary(int threadId, unsigned long long epochId, unsigned long long epochLength, int eventCount, int denseTimelineSamples, int writeBackSamples, unsigned long long stableDonorSlotSamples, unsigned long long totalReplaySlotSamples, long long phaseCertificateReadyHits, long long phaseCertificateReadyMisses, long long estimatedPhaseCertificateChecksSaved, long long phaseCertificateInvalidations, int invalidationObservations, long long eligibilityMaskedCycles, long long eligibilityMaskedReadyCandidates, int longestInvalidationBurst, ReplayPhaseInvalidationReason terminalInvalidationReason)
		: ThreadId(threadId), EpochId(epochId), EpochLength(epochLength), EventCount(eventCount), DenseTimelineSamples(denseTimelineSamples), WriteBackSamples(writeBackSamples), StableDonorSlotSamples(stableDonorSlotSamples), TotalReplaySlotSamples(totalReplaySlotSamples), PhaseCertificateReadyHits(phaseCertificateReadyHits), PhaseCertificateReadyMisses(phaseCertificateReadyMisses), EstimatedPhaseCertificateChecksSaved(estimatedPhaseCertificateChecksSaved), PhaseCertificateInvalidations(phaseCertificateInvalidations), InvalidationObservations(invalidationObservations), EligibilityMaskedCycles(eligibilityMaskedCycles), EligibilityMaskedReadyCandidates(eligibilityMaskedReadyCandidates), LongestInvalidationBurst(longestInvalidationBurst), TerminalInvalidationReason(terminalInvalidationReason)
	{
	}

	int ReplayEpochEvidenceSummary::getThreadId() const
	{
		return ThreadId;
	}

	unsigned long long ReplayEpochEvidenceSummary::getEpochId() const
	{
		return EpochId;
	}

	unsigned long long ReplayEpochEvidenceSummary::getEpochLength() const
	{
		return EpochLength;
	}

	int ReplayEpochEvidenceSummary::getEventCount() const
	{
		return EventCount;
	}

	int ReplayEpochEvidenceSummary::getDenseTimelineSamples() const
	{
		return DenseTimelineSamples;
	}

	int ReplayEpochEvidenceSummary::getWriteBackSamples() const
	{
		return WriteBackSamples;
	}

	unsigned long long ReplayEpochEvidenceSummary::getStableDonorSlotSamples() const
	{
		return StableDonorSlotSamples;
	}

	unsigned long long ReplayEpochEvidenceSummary::getTotalReplaySlotSamples() const
	{
		return TotalReplaySlotSamples;
	}

	long long ReplayEpochEvidenceSummary::getPhaseCertificateReadyHits() const
	{
		return PhaseCertificateReadyHits;
	}

	long long ReplayEpochEvidenceSummary::getPhaseCertificateReadyMisses() const
	{
		return PhaseCertificateReadyMisses;
	}

	long long ReplayEpochEvidenceSummary::getEstimatedPhaseCertificateChecksSaved() const
	{
		return EstimatedPhaseCertificateChecksSaved;
	}

	long long ReplayEpochEvidenceSummary::getPhaseCertificateInvalidations() const
	{
		return PhaseCertificateInvalidations;
	}

	int ReplayEpochEvidenceSummary::getInvalidationObservations() const
	{
		return InvalidationObservations;
	}

	long long ReplayEpochEvidenceSummary::getEligibilityMaskedCycles() const
	{
		return EligibilityMaskedCycles;
	}

	long long ReplayEpochEvidenceSummary::getEligibilityMaskedReadyCandidates() const
	{
		return EligibilityMaskedReadyCandidates;
	}

	int ReplayEpochEvidenceSummary::getLongestInvalidationBurst() const
	{
		return LongestInvalidationBurst;
	}

	ReplayPhaseInvalidationReason ReplayEpochEvidenceSummary::getTerminalInvalidationReason() const
	{
		return TerminalInvalidationReason;
	}

	double ReplayEpochEvidenceSummary::getDenseTimelineCoverage() const
	{
		return getEventCount() == 0 ? 0.0 : static_cast<double>(getDenseTimelineSamples()) / getEventCount();
	}

	double ReplayEpochEvidenceSummary::getStableDonorSlotRatio() const
	{
		return getTotalReplaySlotSamples() == 0 ? 0.0 : static_cast<double>(getStableDonorSlotSamples()) / getTotalReplaySlotSamples();
	}

	double ReplayEpochEvidenceSummary::getPhaseCertificateReuseHitRate() const
	{
		long long totalReadinessChecks = getPhaseCertificateReadyHits() + getPhaseCertificateReadyMisses();
		if (totalReadinessChecks == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getPhaseCertificateReadyHits()) / totalReadinessChecks;
	}

	std::wstring ReplayEpochEvidenceSummary::Describe() const
	{
// C# TO C++ CONVERTER TASK: The '3:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '4:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '5:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '6:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '7:N0' format specifier is not converted to C++:
		return std::format(L"Epoch {} (thread {}): {} events, dense coverage {:.2f}%, reuse hit-rate {:.2f}%, checks saved {}, eligibility-masked cycles {}, masked ready candidates {}, invalidation burst max {}, terminal reason {}.", getEpochId(), getThreadId(), getEventCount(), getDenseTimelineCoverage() * 100.0, getPhaseCertificateReuseHitRate() * 100.0, getEstimatedPhaseCertificateChecksSaved(), getEligibilityMaskedCycles(), getEligibilityMaskedReadyCandidates(), getLongestInvalidationBurst(), ReplayInvalidationReasonName(getTerminalInvalidationReason()));
	}
}
