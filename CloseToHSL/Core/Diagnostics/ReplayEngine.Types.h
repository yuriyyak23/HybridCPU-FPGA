#pragma once

#include "../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include <string>
#include <format>

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::Core
{
	/// <summary>
	/// Result of repeated-run replay-phase determinism validation.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayDeterminismReport
	class ReplayDeterminismReport
	{
	private:
		const bool IsDeterministic;
		const int ComparedEvents;
		const int ComparedReplayEvents;
		const int ComparedTimelineSamples;
		const int ComparedInvalidationEvents;
		const int ComparedEpochs;
		const int MismatchThreadId;
		const long long MismatchCycle;
		const std::wstring MismatchField;
		const std::wstring ExpectedValue;
		const std::wstring ActualValue;
		const bool TypedSlotAgreementValid;

	public:
		ReplayDeterminismReport(bool isDeterministic, int comparedEvents, int comparedReplayEvents, int comparedTimelineSamples, int comparedInvalidationEvents, int comparedEpochs, int mismatchThreadId, long long mismatchCycle, const std::wstring &mismatchField, const std::wstring &expectedValue, const std::wstring &actualValue);

		/// <summary>
		/// Phase 09: extended constructor with typed-slot agreement validation field.
		/// </summary>
		ReplayDeterminismReport(bool isDeterministic, int comparedEvents, int comparedReplayEvents, int comparedTimelineSamples, int comparedInvalidationEvents, int comparedEpochs, int mismatchThreadId, long long mismatchCycle, const std::wstring &mismatchField, const std::wstring &expectedValue, const std::wstring &actualValue, bool typedSlotAgreementValid);

		bool isDeterministic() const;

		int getComparedEvents() const;

		int getComparedReplayEvents() const;

		int getComparedTimelineSamples() const;

		int getComparedInvalidationEvents() const;

		int getComparedEpochs() const;

		int getMismatchThreadId() const;

		long long getMismatchCycle() const;

		std::wstring getMismatchField() const;

		std::wstring getExpectedValue() const;

		std::wstring getActualValue() const;

		/// <summary>
		/// Phase 09: indicates whether typed-slot compiler/runtime agreement
		/// was valid across the replay comparison. <see langword="true"/> when
		/// compiler facts → runtime decisions → same lane bindings across runs.
		/// Defaults to <see langword="true"/> for backward compatibility.
		/// </summary>
		bool getTypedSlotAgreementValid() const;

		std::wstring Describe() const;

		ReplayDeterminismReport() = default;
	};

	enum class ReplayEnvelopeKind : unsigned char
	{
		TimingNoise,
		ResourcePressure,
		Mixed
	};

	/// <summary>
	/// Bounded perturbation envelope for repeated-run replay comparison.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayEnvelopeConfiguration
	class ReplayEnvelopeConfiguration
	{
	private:
		const ReplayEnvelopeKind Kind;
		const long long MaxCycleDrift;
		const long long MaxMemorySubsystemCycleDrift;
		const int MaxActiveMemoryRequestDelta;
		const int MaxBankQueueDepthDelta;
		const int MaxReadyQueueDepthDelta;

	public:
		ReplayEnvelopeConfiguration(ReplayEnvelopeKind kind, long long maxCycleDrift, long long maxMemorySubsystemCycleDrift, int maxActiveMemoryRequestDelta, int maxBankQueueDepthDelta, int maxReadyQueueDepthDelta);

		ReplayEnvelopeKind getKind() const;

		long long getMaxCycleDrift() const;

		long long getMaxMemorySubsystemCycleDrift() const;

		int getMaxActiveMemoryRequestDelta() const;

		int getMaxBankQueueDepthDelta() const;

		int getMaxReadyQueueDepthDelta() const;

		static ReplayEnvelopeConfiguration CreateTimingNoise(long long maxCycleDrift);

		static ReplayEnvelopeConfiguration CreateResourcePressure(int maxActiveMemoryRequestDelta, int maxBankQueueDepthDelta, int maxReadyQueueDepthDelta);

		static ReplayEnvelopeConfiguration CreateMixed(long long maxCycleDrift, long long maxMemorySubsystemCycleDrift, int maxActiveMemoryRequestDelta, int maxBankQueueDepthDelta, int maxReadyQueueDepthDelta);

		ReplayEnvelopeConfiguration() = default;
	};

	/// <summary>
	/// Result of bounded perturbation replay-envelope validation.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayEnvelopeReport
	class ReplayEnvelopeReport
	{
	private:
		const bool IsWithinEnvelope;
		const ReplayEnvelopeKind Kind;
		const int ComparedEvents;
		const int ComparedReplayEvents;
		const int ComparedEnvelopeFields;
		const int MismatchThreadId;
		const long long MismatchCycle;
		const std::wstring MismatchField;
		const std::wstring ExpectedValue;
		const std::wstring ActualValue;
		const std::wstring AllowedEnvelope;

	public:
		ReplayEnvelopeReport(bool isWithinEnvelope, ReplayEnvelopeKind kind, int comparedEvents, int comparedReplayEvents, int comparedEnvelopeFields, int mismatchThreadId, long long mismatchCycle, const std::wstring &mismatchField, const std::wstring &expectedValue, const std::wstring &actualValue, const std::wstring &allowedEnvelope);

		bool isWithinEnvelope() const;

		ReplayEnvelopeKind getKind() const;

		int getComparedEvents() const;

		int getComparedReplayEvents() const;

		int getComparedEnvelopeFields() const;

		int getMismatchThreadId() const;

		long long getMismatchCycle() const;

		std::wstring getMismatchField() const;

		std::wstring getExpectedValue() const;

		std::wstring getActualValue() const;

		std::wstring getAllowedEnvelope() const;

		std::wstring Describe() const;

		ReplayEnvelopeReport() = default;
	};

	/// <summary>
	/// Aggregated replay-heavy evidence extracted from a single trace.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayTraceEvidenceSummary
	class ReplayTraceEvidenceSummary
	{
	private:
		const int TotalEvents;
		const int ReplayPhaseEvents;
		const int DenseTimelineSamples;
		const int WriteBackSamples;
		const int ReplayEpochCount;
		const unsigned long long TotalEpochLength;
		const unsigned long long StableDonorSlotSamples;
		const unsigned long long TotalReplaySlotSamples;
		const long long PhaseCertificateReadyHits;
		const long long PhaseCertificateReadyMisses;
		const long long EstimatedPhaseCertificateChecksSaved;
		const long long PhaseCertificateInvalidations;
		const int InvalidationObservations;
		const int MutationInvalidationObservations;
		const int PhaseMismatchInvalidationObservations;
		const long long EligibilityMaskedCycles;
		const long long EligibilityMaskedReadyCandidates;
		const int InvalidationBursts;
		const int LongestInvalidationBurst;

	public:
		ReplayTraceEvidenceSummary(int totalEvents, int replayPhaseEvents, int denseTimelineSamples, int writeBackSamples, int replayEpochCount, unsigned long long totalEpochLength, unsigned long long stableDonorSlotSamples, unsigned long long totalReplaySlotSamples, long long phaseCertificateReadyHits, long long phaseCertificateReadyMisses, long long estimatedPhaseCertificateChecksSaved, long long phaseCertificateInvalidations, int invalidationObservations, int mutationInvalidationObservations, int phaseMismatchInvalidationObservations, long long eligibilityMaskedCycles, long long eligibilityMaskedReadyCandidates, int invalidationBursts, int longestInvalidationBurst);

		int getTotalEvents() const;

		int getReplayPhaseEvents() const;

		int getDenseTimelineSamples() const;

		int getWriteBackSamples() const;

		int getReplayEpochCount() const;

		unsigned long long getTotalEpochLength() const;

		unsigned long long getStableDonorSlotSamples() const;

		unsigned long long getTotalReplaySlotSamples() const;

		long long getPhaseCertificateReadyHits() const;

		long long getPhaseCertificateReadyMisses() const;

		long long getEstimatedPhaseCertificateChecksSaved() const;

		long long getPhaseCertificateInvalidations() const;

		int getInvalidationObservations() const;

		int getMutationInvalidationObservations() const;

		int getPhaseMismatchInvalidationObservations() const;

		long long getEligibilityMaskedCycles() const;

		long long getEligibilityMaskedReadyCandidates() const;

		int getInvalidationBursts() const;

		int getLongestInvalidationBurst() const;

		double getAverageEpochLength() const;

		double getStableDonorSlotRatio() const;

		double getDenseTimelineCoverage() const;

		double getPhaseCertificateReuseHitRate() const;

		std::wstring Describe() const;

		ReplayTraceEvidenceSummary() = default;
	};

	/// <summary>
	/// Per-epoch replay-heavy evidence for long practical kernel traces.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayEpochEvidenceSummary
	class ReplayEpochEvidenceSummary
	{
	private:
		const int ThreadId;
		const unsigned long long EpochId;
		const unsigned long long EpochLength;
		const int EventCount;
		const int DenseTimelineSamples;
		const int WriteBackSamples;
		const unsigned long long StableDonorSlotSamples;
		const unsigned long long TotalReplaySlotSamples;
		const long long PhaseCertificateReadyHits;
		const long long PhaseCertificateReadyMisses;
		const long long EstimatedPhaseCertificateChecksSaved;
		const long long PhaseCertificateInvalidations;
		const int InvalidationObservations;
		const long long EligibilityMaskedCycles;
		const long long EligibilityMaskedReadyCandidates;
		const int LongestInvalidationBurst;
		const ReplayPhaseInvalidationReason TerminalInvalidationReason;

	public:
		ReplayEpochEvidenceSummary(int threadId, unsigned long long epochId, unsigned long long epochLength, int eventCount, int denseTimelineSamples, int writeBackSamples, unsigned long long stableDonorSlotSamples, unsigned long long totalReplaySlotSamples, long long phaseCertificateReadyHits, long long phaseCertificateReadyMisses, long long estimatedPhaseCertificateChecksSaved, long long phaseCertificateInvalidations, int invalidationObservations, long long eligibilityMaskedCycles, long long eligibilityMaskedReadyCandidates, int longestInvalidationBurst, ReplayPhaseInvalidationReason terminalInvalidationReason);

		int getThreadId() const;

		unsigned long long getEpochId() const;

		unsigned long long getEpochLength() const;

		int getEventCount() const;

		int getDenseTimelineSamples() const;

		int getWriteBackSamples() const;

		unsigned long long getStableDonorSlotSamples() const;

		unsigned long long getTotalReplaySlotSamples() const;

		long long getPhaseCertificateReadyHits() const;

		long long getPhaseCertificateReadyMisses() const;

		long long getEstimatedPhaseCertificateChecksSaved() const;

		long long getPhaseCertificateInvalidations() const;

		int getInvalidationObservations() const;

		long long getEligibilityMaskedCycles() const;

		long long getEligibilityMaskedReadyCandidates() const;

		int getLongestInvalidationBurst() const;

		ReplayPhaseInvalidationReason getTerminalInvalidationReason() const;

		double getDenseTimelineCoverage() const;

		double getStableDonorSlotRatio() const;

		double getPhaseCertificateReuseHitRate() const;

		std::wstring Describe() const;

		ReplayEpochEvidenceSummary() = default;
	};
}
