#include "HybridCPU_ISE.Core.ReplayEngine.h"
#include "../Architecture/Common/BinaryIOCompat.h"
#include <algorithm>
#include <cstdlib>
#include <string_view>
#include <tuple>

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using System::IO::FileMode;
using System::IO::FileStream;

namespace HybridCPU_ISE::Core
{
	namespace
	{
		int ReadBoundedCount(const std::shared_ptr<BinaryReader> &reader, int maximum, int bytesPerItem)
		{
			const int count = reader->ReadInt32();
			if (count < 0 || count > maximum || (bytesPerItem > 0 && static_cast<long long>(count) * bytesPerItem > reader->getRemaining()))
				throw InvalidDataException(L"Invalid or oversized trace count.");
			return count;
		}
	}

	ReplayEnvelopeReport ReplayEngine::CompareReplayPhaseBehaviorWithinEnvelope(std::shared_ptr<ReplayEngine> other, ReplayEnvelopeConfiguration configuration)
	{
		ArgumentNullException::ThrowIfNull(other);

		int comparedEvents = 0;
		int comparedReplayEvents = 0;
		int comparedEnvelopeFields = 0;

		for (int threadId = 0; threadId < 16; threadId++)
		{
			std::vector<FullStateTraceEvent> baselineTrace = _perThreadTraces[threadId];
			std::vector<FullStateTraceEvent> candidateTrace = other->_perThreadTraces[threadId];

			if (baselineTrace.size() != candidateTrace.size())
			{
				return ReplayEnvelopeReport(false, configuration.getKind(), comparedEvents, comparedReplayEvents, comparedEnvelopeFields, threadId, -1, L"EventCount", std::to_wstring(baselineTrace.size()), std::to_wstring(candidateTrace.size()), L"exact match");
			}

			for (int index = 0; index < baselineTrace.size(); index++)
			{
				FullStateTraceEvent baselineEvent = baselineTrace[index];
				FullStateTraceEvent candidateEvent = candidateTrace[index];

				std::wstring mismatchField;
				std::wstring expectedValue;
				std::wstring actualValue;
				if (!TryCompareReplayContractEvent(baselineEvent, candidateEvent, mismatchField, expectedValue, actualValue))
				{
					return ReplayEnvelopeReport(false, configuration.getKind(), comparedEvents, comparedReplayEvents, comparedEnvelopeFields, threadId, baselineEvent.CycleNumber, mismatchField, expectedValue, actualValue, L"exact match");
				}

				std::wstring allowedEnvelope;
				int envelopeChecks;
				if (!TryCompareEventWithinEnvelope(baselineEvent, candidateEvent, configuration, mismatchField, expectedValue, actualValue, allowedEnvelope, envelopeChecks))
				{
					return ReplayEnvelopeReport(false, configuration.getKind(), comparedEvents, comparedReplayEvents, comparedEnvelopeFields, threadId, baselineEvent.CycleNumber, mismatchField, expectedValue, actualValue, allowedEnvelope);
				}

				comparedEvents++;
				comparedEnvelopeFields += envelopeChecks;
				if (baselineEvent.ReplayEpochId != 0)
				{
					comparedReplayEvents++;
				}
			}
		}

		return ReplayEnvelopeReport(true, configuration.getKind(), comparedEvents, comparedReplayEvents, comparedEnvelopeFields, -1, -1, L"", L"", L"", L"");
	}

	ReplayTraceEvidenceSummary ReplayEngine::SummarizeReplayPhaseEvidence()
	{
		int totalEvents = 0;
		int replayPhaseEvents = 0;
		int denseTimelineSamples = 0;
		int writeBackSamples = 0;
		int replayEpochCount = 0;
		unsigned long long totalEpochLength = 0;
		unsigned long long stableDonorSlotSamples = 0;
		unsigned long long totalReplaySlotSamples = 0;
		long long phaseCertificateReadyHits = 0;
		long long phaseCertificateReadyMisses = 0;
		long long estimatedPhaseCertificateChecksSaved = 0;
		long long phaseCertificateInvalidations = 0;
		int invalidationObservations = 0;
		int mutationInvalidationObservations = 0;
		int phaseMismatchInvalidationObservations = 0;
		long long eligibilityMaskedCycles = 0;
		long long eligibilityMaskedReadyCandidates = 0;
		int invalidationBursts = 0;
		int longestInvalidationBurst = 0;

		for (const auto &[threadId, trace] : _perThreadTraces)
		{
			unsigned long long lastEpochId = 0;
			int currentInvalidationBurst = 0;

			for (auto evt : trace)
			{
				totalEvents++;

				if (evt.ReplayEpochId != 0)
				{
					replayPhaseEvents++;
					stableDonorSlotSamples += static_cast<unsigned long long>(CountBits(evt.StableDonorMask));
					totalReplaySlotSamples += 8;

					if (evt.ReplayEpochId != lastEpochId)
					{
						replayEpochCount++;
						totalEpochLength += evt.ReplayEpochLength;
						lastEpochId = evt.ReplayEpochId;
					}
				}
				else
				{
					lastEpochId = 0;
				}

				if (IsDensePhaseTimelineSample(evt))
				{
					denseTimelineSamples++;
				}

				if (evt.PipelineStage == L"WB")
				{
					writeBackSamples++;
				}

				phaseCertificateReadyHits = std::max(phaseCertificateReadyHits, evt.PhaseCertificateReadyHits);
				phaseCertificateReadyMisses = std::max(phaseCertificateReadyMisses, evt.PhaseCertificateReadyMisses);
				estimatedPhaseCertificateChecksSaved = std::max(estimatedPhaseCertificateChecksSaved, evt.EstimatedPhaseCertificateChecksSaved);
				phaseCertificateInvalidations = std::max(phaseCertificateInvalidations, evt.PhaseCertificateInvalidations);
				eligibilityMaskedCycles = std::max(eligibilityMaskedCycles, evt.EligibilityMaskedCycles);
				eligibilityMaskedReadyCandidates = std::max(eligibilityMaskedReadyCandidates, evt.EligibilityMaskedReadyCandidates);

				bool hasInvalidation = HasInvalidationSignal(evt);
				if (hasInvalidation)
				{
					invalidationObservations++;
					currentInvalidationBurst++;

					ReplayPhaseInvalidationReason dominantReason = evt.PhaseCertificateInvalidationReason != ReplayPhaseInvalidationReason::None ? evt.PhaseCertificateInvalidationReason : evt.ReplayInvalidationReason;

					if (dominantReason == ReplayPhaseInvalidationReason::CertificateMutation)
					{
						mutationInvalidationObservations++;
					}
					else if (dominantReason == ReplayPhaseInvalidationReason::PhaseMismatch)
					{
						phaseMismatchInvalidationObservations++;
					}
				}
				else if (currentInvalidationBurst > 0)
				{
					invalidationBursts++;
					longestInvalidationBurst = std::max(longestInvalidationBurst, currentInvalidationBurst);
					currentInvalidationBurst = 0;
				}
			}

			if (currentInvalidationBurst > 0)
			{
				invalidationBursts++;
				longestInvalidationBurst = std::max(longestInvalidationBurst, currentInvalidationBurst);
			}
		}

		return ReplayTraceEvidenceSummary(totalEvents, replayPhaseEvents, denseTimelineSamples, writeBackSamples, replayEpochCount, totalEpochLength, stableDonorSlotSamples, totalReplaySlotSamples, phaseCertificateReadyHits, phaseCertificateReadyMisses, estimatedPhaseCertificateChecksSaved, phaseCertificateInvalidations, invalidationObservations, mutationInvalidationObservations, phaseMismatchInvalidationObservations, eligibilityMaskedCycles, eligibilityMaskedReadyCandidates, invalidationBursts, longestInvalidationBurst);
	}

	std::vector<ReplayEpochEvidenceSummary> ReplayEngine::SummarizeReplayEpochEvidence()
	{
		std::vector<ReplayEpochEvidenceSummary> epochSummaries;

		for (auto kvp : _perThreadTraces)
		{
			int threadId = kvp.first;
			std::vector<FullStateTraceEvent> trace = kvp.second;

			unsigned long long currentEpochId = 0;
			unsigned long long currentEpochLength = 0;
			int eventCount = 0;
			int denseTimelineSamples = 0;
			int writeBackSamples = 0;
			unsigned long long stableDonorSlotSamples = 0;
			unsigned long long totalReplaySlotSamples = 0;
			long long phaseCertificateReadyHits = 0;
			long long phaseCertificateReadyMisses = 0;
			long long estimatedPhaseCertificateChecksSaved = 0;
			long long phaseCertificateInvalidations = 0;
			int invalidationObservations = 0;
			long long eligibilityMaskedCycles = 0;
			long long eligibilityMaskedReadyCandidates = 0;
			int longestInvalidationBurst = 0;
			int currentInvalidationBurst = 0;
			ReplayPhaseInvalidationReason terminalInvalidationReason = ReplayPhaseInvalidationReason::None;

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//			void FinalizeEpoch()
	//				{
	//					if (currentEpochId == 0)
	//					{
	//						return;
	//					}
	//
	//					if (currentInvalidationBurst > 0)
	//					{
	//						longestInvalidationBurst = Math.Max(longestInvalidationBurst, currentInvalidationBurst);
	//						currentInvalidationBurst = 0;
	//					}
	//
	//					epochSummaries.Add(new ReplayEpochEvidenceSummary(threadId, currentEpochId, currentEpochLength, eventCount, denseTimelineSamples, writeBackSamples, stableDonorSlotSamples, totalReplaySlotSamples, phaseCertificateReadyHits, phaseCertificateReadyMisses, estimatedPhaseCertificateChecksSaved, phaseCertificateInvalidations, invalidationObservations, eligibilityMaskedCycles, eligibilityMaskedReadyCandidates, longestInvalidationBurst, terminalInvalidationReason));
	//
	//					currentEpochId = 0;
	//					currentEpochLength = 0;
	//					eventCount = 0;
	//					denseTimelineSamples = 0;
	//					writeBackSamples = 0;
	//					stableDonorSlotSamples = 0;
	//					totalReplaySlotSamples = 0;
	//					phaseCertificateReadyHits = 0;
	//					phaseCertificateReadyMisses = 0;
	//					estimatedPhaseCertificateChecksSaved = 0;
	//					phaseCertificateInvalidations = 0;
	//					invalidationObservations = 0;
	//					eligibilityMaskedCycles = 0;
	//					eligibilityMaskedReadyCandidates = 0;
	//					longestInvalidationBurst = 0;
	//					terminalInvalidationReason = ReplayPhaseInvalidationReason.None;
	//				}

			auto finalizeEpoch = [&]()
			{
				if (currentEpochId == 0)
					return;
				if (currentInvalidationBurst > 0)
				{
					longestInvalidationBurst = std::max(longestInvalidationBurst, currentInvalidationBurst);
					currentInvalidationBurst = 0;
				}
				epochSummaries.emplace_back(threadId, currentEpochId, currentEpochLength, eventCount, denseTimelineSamples, writeBackSamples, stableDonorSlotSamples, totalReplaySlotSamples, phaseCertificateReadyHits, phaseCertificateReadyMisses, estimatedPhaseCertificateChecksSaved, phaseCertificateInvalidations, invalidationObservations, eligibilityMaskedCycles, eligibilityMaskedReadyCandidates, longestInvalidationBurst, terminalInvalidationReason);
				currentEpochId = 0;
				currentEpochLength = 0;
				eventCount = 0;
				denseTimelineSamples = 0;
				writeBackSamples = 0;
				stableDonorSlotSamples = 0;
				totalReplaySlotSamples = 0;
				phaseCertificateReadyHits = 0;
				phaseCertificateReadyMisses = 0;
				estimatedPhaseCertificateChecksSaved = 0;
				phaseCertificateInvalidations = 0;
				invalidationObservations = 0;
				eligibilityMaskedCycles = 0;
				eligibilityMaskedReadyCandidates = 0;
				longestInvalidationBurst = 0;
				terminalInvalidationReason = ReplayPhaseInvalidationReason::None;
			};

			for (auto evt : trace)
			{
				if (evt.ReplayEpochId == 0)
				{
					finalizeEpoch();
					continue;
				}

				if (currentEpochId != evt.ReplayEpochId)
				{
					finalizeEpoch();
					currentEpochId = evt.ReplayEpochId;
					currentEpochLength = evt.ReplayEpochLength;
				}

				eventCount++;
				stableDonorSlotSamples += static_cast<unsigned long long>(CountBits(evt.StableDonorMask));
				totalReplaySlotSamples += 8;

				if (IsDensePhaseTimelineSample(evt))
				{
					denseTimelineSamples++;
				}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
				if (evt.PipelineStage == L"WB")
				{
					writeBackSamples++;
				}

				phaseCertificateReadyHits = std::max(phaseCertificateReadyHits, evt.PhaseCertificateReadyHits);
				phaseCertificateReadyMisses = std::max(phaseCertificateReadyMisses, evt.PhaseCertificateReadyMisses);
				estimatedPhaseCertificateChecksSaved = std::max(estimatedPhaseCertificateChecksSaved, evt.EstimatedPhaseCertificateChecksSaved);
				phaseCertificateInvalidations = std::max(phaseCertificateInvalidations, evt.PhaseCertificateInvalidations);
				eligibilityMaskedCycles = std::max(eligibilityMaskedCycles, evt.EligibilityMaskedCycles);
				eligibilityMaskedReadyCandidates = std::max(eligibilityMaskedReadyCandidates, evt.EligibilityMaskedReadyCandidates);

				if (HasInvalidationSignal(evt))
				{
					invalidationObservations++;
					currentInvalidationBurst++;
					terminalInvalidationReason = GetDominantInvalidationReason(evt);
				}
				else if (currentInvalidationBurst > 0)
				{
					longestInvalidationBurst = std::max(longestInvalidationBurst, currentInvalidationBurst);
					currentInvalidationBurst = 0;
				}
			}

			finalizeEpoch();
		}

		return epochSummaries;
	}

	bool ReplayEngine::HasInvalidationSignal(FullStateTraceEvent evt)
	{
		return evt.ReplayInvalidationReason != ReplayPhaseInvalidationReason::None || evt.PhaseCertificateInvalidationReason != ReplayPhaseInvalidationReason::None;
	}

	ReplayPhaseInvalidationReason ReplayEngine::GetDominantInvalidationReason(FullStateTraceEvent evt)
	{
		return evt.PhaseCertificateInvalidationReason != ReplayPhaseInvalidationReason::None ? evt.PhaseCertificateInvalidationReason : evt.ReplayInvalidationReason;
	}

	bool ReplayEngine::IsDensePhaseTimelineSample(FullStateTraceEvent evt)
	{
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		return HasBasePolicy(evt.CurrentFSPPolicy, DenseTimelinePolicyName) && (evt.PipelineStage == L"CYCLE" || evt.PipelineStage == L"STALL");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool HasBasePolicy(string? policy, string expectedPolicy)
	bool ReplayEngine::HasBasePolicy(const std::wstring &policy, const std::wstring &expectedPolicy)
	{
		if (policy.empty())
		{
			return false;
		}

		const std::size_t separatorIndex = policy.find(L'|');
		const std::wstring_view basePolicy(policy.data(), separatorIndex == std::wstring::npos ? policy.size() : separatorIndex);
		return basePolicy == std::wstring_view(expectedPolicy);
	}

	bool ReplayEngine::TryCompareReplayContractEvent(FullStateTraceEvent expected, FullStateTraceEvent actual, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue)
	{
		if (expected.ThreadId != actual.ThreadId)
		{
			return Mismatch(L"ThreadId", expected.ThreadId, actual.ThreadId, mismatchField, expectedValue, actualValue);
		}

		if (expected.PC != actual.PC)
		{
			return Mismatch(L"PC", expected.PC, actual.PC, mismatchField, expectedValue, actualValue);
		}

		if (expected.BundleId != actual.BundleId)
		{
			return Mismatch(L"BundleId", expected.BundleId, actual.BundleId, mismatchField, expectedValue, actualValue);
		}

		if (expected.OpIndex != actual.OpIndex)
		{
			return Mismatch(L"OpIndex", expected.OpIndex, actual.OpIndex, mismatchField, expectedValue, actualValue);
		}

		if (expected.Opcode != actual.Opcode)
		{
			return Mismatch(L"Opcode", expected.Opcode, actual.Opcode, mismatchField, expectedValue, actualValue);
		}

		if (expected.WasStolenSlot != actual.WasStolenSlot)
		{
			return Mismatch(L"WasStolenSlot", expected.WasStolenSlot, actual.WasStolenSlot, mismatchField, expectedValue, actualValue);
		}

		if (expected.OriginalThreadId != actual.OriginalThreadId)
		{
			return Mismatch(L"OriginalThreadId", expected.OriginalThreadId, actual.OriginalThreadId, mismatchField, expectedValue, actualValue);
		}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (expected.PipelineStage != actual.PipelineStage)
		{
			return Mismatch(L"PipelineStage", expected.PipelineStage, actual.PipelineStage, mismatchField, expectedValue, actualValue);
		}

		if (expected.Stalled != actual.Stalled)
		{
			return Mismatch(L"Stalled", expected.Stalled, actual.Stalled, mismatchField, expectedValue, actualValue);
		}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (expected.StallReason != actual.StallReason)
		{
			return Mismatch(L"StallReason", expected.StallReason, actual.StallReason, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleStateOwnerKind != actual.DecodedBundleStateOwnerKind)
		{
			return Mismatch(L"DecodedBundleStateOwnerKind", expected.DecodedBundleStateOwnerKind, actual.DecodedBundleStateOwnerKind, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleStateEpoch != actual.DecodedBundleStateEpoch)
		{
			return Mismatch(L"DecodedBundleStateEpoch", expected.DecodedBundleStateEpoch, actual.DecodedBundleStateEpoch, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleStateVersion != actual.DecodedBundleStateVersion)
		{
			return Mismatch(L"DecodedBundleStateVersion", expected.DecodedBundleStateVersion, actual.DecodedBundleStateVersion, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleStateKind != actual.DecodedBundleStateKind)
		{
			return Mismatch(L"DecodedBundleStateKind", expected.DecodedBundleStateKind, actual.DecodedBundleStateKind, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleStateOrigin != actual.DecodedBundleStateOrigin)
		{
			return Mismatch(L"DecodedBundleStateOrigin", expected.DecodedBundleStateOrigin, actual.DecodedBundleStateOrigin, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundlePc != actual.DecodedBundlePc)
		{
			return Mismatch(L"DecodedBundlePc", expected.DecodedBundlePc, actual.DecodedBundlePc, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleValidMask != actual.DecodedBundleValidMask)
		{
			return Mismatch(L"DecodedBundleValidMask", expected.DecodedBundleValidMask, actual.DecodedBundleValidMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleNopMask != actual.DecodedBundleNopMask)
		{
			return Mismatch(L"DecodedBundleNopMask", expected.DecodedBundleNopMask, actual.DecodedBundleNopMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleHasCanonicalDecode != actual.DecodedBundleHasCanonicalDecode)
		{
			return Mismatch(L"DecodedBundleHasCanonicalDecode", expected.DecodedBundleHasCanonicalDecode, actual.DecodedBundleHasCanonicalDecode, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleHasCanonicalLegality != actual.DecodedBundleHasCanonicalLegality)
		{
			return Mismatch(L"DecodedBundleHasCanonicalLegality", expected.DecodedBundleHasCanonicalLegality, actual.DecodedBundleHasCanonicalLegality, mismatchField, expectedValue, actualValue);
		}

		if (expected.DecodedBundleHasDecodeFault != actual.DecodedBundleHasDecodeFault)
		{
			return Mismatch(L"DecodedBundleHasDecodeFault", expected.DecodedBundleHasDecodeFault, actual.DecodedBundleHasDecodeFault, mismatchField, expectedValue, actualValue);
		}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (!(expected.CurrentFSPPolicy == actual.CurrentFSPPolicy))
		{
			return Mismatch(L"CurrentFSPPolicy", expected.CurrentFSPPolicy, actual.CurrentFSPPolicy, mismatchField, expectedValue, actualValue);
		}

		if (expected.ReplayEpochId != actual.ReplayEpochId)
		{
			return Mismatch(L"ReplayEpochId", expected.ReplayEpochId, actual.ReplayEpochId, mismatchField, expectedValue, actualValue);
		}

		if (expected.ReplayPhaseCachedPc != actual.ReplayPhaseCachedPc)
		{
			return Mismatch(L"ReplayPhaseCachedPc", expected.ReplayPhaseCachedPc, actual.ReplayPhaseCachedPc, mismatchField, expectedValue, actualValue);
		}

		if (expected.ReplayEpochLength != actual.ReplayEpochLength)
		{
			return Mismatch(L"ReplayEpochLength", expected.ReplayEpochLength, actual.ReplayEpochLength, mismatchField, expectedValue, actualValue);
		}

		if (expected.ReplayPhaseValidSlotCount != actual.ReplayPhaseValidSlotCount)
		{
			return Mismatch(L"ReplayPhaseValidSlotCount", expected.ReplayPhaseValidSlotCount, actual.ReplayPhaseValidSlotCount, mismatchField, expectedValue, actualValue);
		}

		if (expected.StableDonorMask != actual.StableDonorMask)
		{
			return Mismatch(L"StableDonorMask", expected.StableDonorMask, actual.StableDonorMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.ReplayInvalidationReason != actual.ReplayInvalidationReason)
		{
			return Mismatch(L"ReplayInvalidationReason", expected.ReplayInvalidationReason, actual.ReplayInvalidationReason, mismatchField, expectedValue, actualValue);
		}

		if (expected.PhaseCertificateTemplateReusable != actual.PhaseCertificateTemplateReusable)
		{
			return Mismatch(L"PhaseCertificateTemplateReusable", expected.PhaseCertificateTemplateReusable, actual.PhaseCertificateTemplateReusable, mismatchField, expectedValue, actualValue);
		}

		if (expected.PhaseCertificateReadyHits != actual.PhaseCertificateReadyHits)
		{
			return Mismatch(L"PhaseCertificateReadyHits", expected.PhaseCertificateReadyHits, actual.PhaseCertificateReadyHits, mismatchField, expectedValue, actualValue);
		}

		if (expected.PhaseCertificateReadyMisses != actual.PhaseCertificateReadyMisses)
		{
			return Mismatch(L"PhaseCertificateReadyMisses", expected.PhaseCertificateReadyMisses, actual.PhaseCertificateReadyMisses, mismatchField, expectedValue, actualValue);
		}

		if (expected.EstimatedPhaseCertificateChecksSaved != actual.EstimatedPhaseCertificateChecksSaved)
		{
			return Mismatch(L"EstimatedPhaseCertificateChecksSaved", expected.EstimatedPhaseCertificateChecksSaved, actual.EstimatedPhaseCertificateChecksSaved, mismatchField, expectedValue, actualValue);
		}

		if (expected.PhaseCertificateInvalidations != actual.PhaseCertificateInvalidations)
		{
			return Mismatch(L"PhaseCertificateInvalidations", expected.PhaseCertificateInvalidations, actual.PhaseCertificateInvalidations, mismatchField, expectedValue, actualValue);
		}

		if (expected.PhaseCertificateInvalidationReason != actual.PhaseCertificateInvalidationReason)
		{
			return Mismatch(L"PhaseCertificateInvalidationReason", expected.PhaseCertificateInvalidationReason, actual.PhaseCertificateInvalidationReason, mismatchField, expectedValue, actualValue);
		}

		if (expected.DeterminismReferenceOpportunitySlots != actual.DeterminismReferenceOpportunitySlots)
		{
			return Mismatch(L"DeterminismReferenceOpportunitySlots", expected.DeterminismReferenceOpportunitySlots, actual.DeterminismReferenceOpportunitySlots, mismatchField, expectedValue, actualValue);
		}

		if (expected.DeterminismReplayEligibleSlots != actual.DeterminismReplayEligibleSlots)
		{
			return Mismatch(L"DeterminismReplayEligibleSlots", expected.DeterminismReplayEligibleSlots, actual.DeterminismReplayEligibleSlots, mismatchField, expectedValue, actualValue);
		}

		if (expected.DeterminismMaskedSlots != actual.DeterminismMaskedSlots)
		{
			return Mismatch(L"DeterminismMaskedSlots", expected.DeterminismMaskedSlots, actual.DeterminismMaskedSlots, mismatchField, expectedValue, actualValue);
		}

		if (expected.DeterminismEstimatedLostSlots != actual.DeterminismEstimatedLostSlots)
		{
			return Mismatch(L"DeterminismEstimatedLostSlots", expected.DeterminismEstimatedLostSlots, actual.DeterminismEstimatedLostSlots, mismatchField, expectedValue, actualValue);
		}

		if (expected.DeterminismConstrainedCycles != actual.DeterminismConstrainedCycles)
		{
			return Mismatch(L"DeterminismConstrainedCycles", expected.DeterminismConstrainedCycles, actual.DeterminismConstrainedCycles, mismatchField, expectedValue, actualValue);
		}

		if (expected.DomainIsolationProbeAttempts != actual.DomainIsolationProbeAttempts)
		{
			return Mismatch(L"DomainIsolationProbeAttempts", expected.DomainIsolationProbeAttempts, actual.DomainIsolationProbeAttempts, mismatchField, expectedValue, actualValue);
		}

		if (expected.DomainIsolationBlockedAttempts != actual.DomainIsolationBlockedAttempts)
		{
			return Mismatch(L"DomainIsolationBlockedAttempts", expected.DomainIsolationBlockedAttempts, actual.DomainIsolationBlockedAttempts, mismatchField, expectedValue, actualValue);
		}

		if (expected.DomainIsolationCrossDomainBlocks != actual.DomainIsolationCrossDomainBlocks)
		{
			return Mismatch(L"DomainIsolationCrossDomainBlocks", expected.DomainIsolationCrossDomainBlocks, actual.DomainIsolationCrossDomainBlocks, mismatchField, expectedValue, actualValue);
		}

		if (expected.DomainIsolationKernelToUserBlocks != actual.DomainIsolationKernelToUserBlocks)
		{
			return Mismatch(L"DomainIsolationKernelToUserBlocks", expected.DomainIsolationKernelToUserBlocks, actual.DomainIsolationKernelToUserBlocks, mismatchField, expectedValue, actualValue);
		}

		if (expected.EligibilityMaskedCycles != actual.EligibilityMaskedCycles)
		{
			return Mismatch(L"EligibilityMaskedCycles", expected.EligibilityMaskedCycles, actual.EligibilityMaskedCycles, mismatchField, expectedValue, actualValue);
		}

		if (expected.EligibilityMaskedReadyCandidates != actual.EligibilityMaskedReadyCandidates)
		{
			return Mismatch(L"EligibilityMaskedReadyCandidates", expected.EligibilityMaskedReadyCandidates, actual.EligibilityMaskedReadyCandidates, mismatchField, expectedValue, actualValue);
		}

		if (expected.LastEligibilityRequestedMask != actual.LastEligibilityRequestedMask)
		{
			return Mismatch(L"LastEligibilityRequestedMask", expected.LastEligibilityRequestedMask, actual.LastEligibilityRequestedMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.LastEligibilityNormalizedMask != actual.LastEligibilityNormalizedMask)
		{
			return Mismatch(L"LastEligibilityNormalizedMask", expected.LastEligibilityNormalizedMask, actual.LastEligibilityNormalizedMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.LastEligibilityReadyPortMask != actual.LastEligibilityReadyPortMask)
		{
			return Mismatch(L"LastEligibilityReadyPortMask", expected.LastEligibilityReadyPortMask, actual.LastEligibilityReadyPortMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.LastEligibilityVisibleReadyMask != actual.LastEligibilityVisibleReadyMask)
		{
			return Mismatch(L"LastEligibilityVisibleReadyMask", expected.LastEligibilityVisibleReadyMask, actual.LastEligibilityVisibleReadyMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.LastEligibilityMaskedReadyMask != actual.LastEligibilityMaskedReadyMask)
		{
			return Mismatch(L"LastEligibilityMaskedReadyMask", expected.LastEligibilityMaskedReadyMask, actual.LastEligibilityMaskedReadyMask, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistNominations != actual.AssistNominations)
		{
			return Mismatch(L"AssistNominations", expected.AssistNominations, actual.AssistNominations, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInjections != actual.AssistInjections)
		{
			return Mismatch(L"AssistInjections", expected.AssistInjections, actual.AssistInjections, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistRejects != actual.AssistRejects)
		{
			return Mismatch(L"AssistRejects", expected.AssistRejects, actual.AssistRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistBoundaryRejects != actual.AssistBoundaryRejects)
		{
			return Mismatch(L"AssistBoundaryRejects", expected.AssistBoundaryRejects, actual.AssistBoundaryRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInvalidations != actual.AssistInvalidations)
		{
			return Mismatch(L"AssistInvalidations", expected.AssistInvalidations, actual.AssistInvalidations, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreNominations != actual.AssistInterCoreNominations)
		{
			return Mismatch(L"AssistInterCoreNominations", expected.AssistInterCoreNominations, actual.AssistInterCoreNominations, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreInjections != actual.AssistInterCoreInjections)
		{
			return Mismatch(L"AssistInterCoreInjections", expected.AssistInterCoreInjections, actual.AssistInterCoreInjections, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreRejects != actual.AssistInterCoreRejects)
		{
			return Mismatch(L"AssistInterCoreRejects", expected.AssistInterCoreRejects, actual.AssistInterCoreRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreDomainRejects != actual.AssistInterCoreDomainRejects)
		{
			return Mismatch(L"AssistInterCoreDomainRejects", expected.AssistInterCoreDomainRejects, actual.AssistInterCoreDomainRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCorePodLocalInjections != actual.AssistInterCorePodLocalInjections)
		{
			return Mismatch(L"AssistInterCorePodLocalInjections", expected.AssistInterCorePodLocalInjections, actual.AssistInterCorePodLocalInjections, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreCrossPodInjections != actual.AssistInterCoreCrossPodInjections)
		{
			return Mismatch(L"AssistInterCoreCrossPodInjections", expected.AssistInterCoreCrossPodInjections, actual.AssistInterCoreCrossPodInjections, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCorePodLocalRejects != actual.AssistInterCorePodLocalRejects)
		{
			return Mismatch(L"AssistInterCorePodLocalRejects", expected.AssistInterCorePodLocalRejects, actual.AssistInterCorePodLocalRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreCrossPodRejects != actual.AssistInterCoreCrossPodRejects)
		{
			return Mismatch(L"AssistInterCoreCrossPodRejects", expected.AssistInterCoreCrossPodRejects, actual.AssistInterCoreCrossPodRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCorePodLocalDomainRejects != actual.AssistInterCorePodLocalDomainRejects)
		{
			return Mismatch(L"AssistInterCorePodLocalDomainRejects", expected.AssistInterCorePodLocalDomainRejects, actual.AssistInterCorePodLocalDomainRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreCrossPodDomainRejects != actual.AssistInterCoreCrossPodDomainRejects)
		{
			return Mismatch(L"AssistInterCoreCrossPodDomainRejects", expected.AssistInterCoreCrossPodDomainRejects, actual.AssistInterCoreCrossPodDomainRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistQuotaRejects != actual.AssistQuotaRejects)
		{
			return Mismatch(L"AssistQuotaRejects", expected.AssistQuotaRejects, actual.AssistQuotaRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistQuotaIssueRejects != actual.AssistQuotaIssueRejects)
		{
			return Mismatch(L"AssistQuotaIssueRejects", expected.AssistQuotaIssueRejects, actual.AssistQuotaIssueRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistQuotaLineRejects != actual.AssistQuotaLineRejects)
		{
			return Mismatch(L"AssistQuotaLineRejects", expected.AssistQuotaLineRejects, actual.AssistQuotaLineRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistQuotaLinesReserved != actual.AssistQuotaLinesReserved)
		{
			return Mismatch(L"AssistQuotaLinesReserved", expected.AssistQuotaLinesReserved, actual.AssistQuotaLinesReserved, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistBackpressureRejects != actual.AssistBackpressureRejects)
		{
			return Mismatch(L"AssistBackpressureRejects", expected.AssistBackpressureRejects, actual.AssistBackpressureRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistBackpressureOuterCapRejects != actual.AssistBackpressureOuterCapRejects)
		{
			return Mismatch(L"AssistBackpressureOuterCapRejects", expected.AssistBackpressureOuterCapRejects, actual.AssistBackpressureOuterCapRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistBackpressureMshrRejects != actual.AssistBackpressureMshrRejects)
		{
			return Mismatch(L"AssistBackpressureMshrRejects", expected.AssistBackpressureMshrRejects, actual.AssistBackpressureMshrRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistBackpressureDmaSrfRejects != actual.AssistBackpressureDmaSrfRejects)
		{
			return Mismatch(L"AssistBackpressureDmaSrfRejects", expected.AssistBackpressureDmaSrfRejects, actual.AssistBackpressureDmaSrfRejects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistDonorPrefetchInjects != actual.AssistDonorPrefetchInjects)
		{
			return Mismatch(L"AssistDonorPrefetchInjects", expected.AssistDonorPrefetchInjects, actual.AssistDonorPrefetchInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistLdsaInjects != actual.AssistLdsaInjects)
		{
			return Mismatch(L"AssistLdsaInjects", expected.AssistLdsaInjects, actual.AssistLdsaInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistVdsaInjects != actual.AssistVdsaInjects)
		{
			return Mismatch(L"AssistVdsaInjects", expected.AssistVdsaInjects, actual.AssistVdsaInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistSameVtInjects != actual.AssistSameVtInjects)
		{
			return Mismatch(L"AssistSameVtInjects", expected.AssistSameVtInjects, actual.AssistSameVtInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistDonorVtInjects != actual.AssistDonorVtInjects)
		{
			return Mismatch(L"AssistDonorVtInjects", expected.AssistDonorVtInjects, actual.AssistDonorVtInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreSameVtVectorInjects != actual.AssistInterCoreSameVtVectorInjects)
		{
			return Mismatch(L"AssistInterCoreSameVtVectorInjects", expected.AssistInterCoreSameVtVectorInjects, actual.AssistInterCoreSameVtVectorInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreDonorVtVectorInjects != actual.AssistInterCoreDonorVtVectorInjects)
		{
			return Mismatch(L"AssistInterCoreDonorVtVectorInjects", expected.AssistInterCoreDonorVtVectorInjects, actual.AssistInterCoreDonorVtVectorInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreSameVtVectorWritebackInjects != actual.AssistInterCoreSameVtVectorWritebackInjects)
		{
			return Mismatch(L"AssistInterCoreSameVtVectorWritebackInjects", expected.AssistInterCoreSameVtVectorWritebackInjects, actual.AssistInterCoreSameVtVectorWritebackInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreDonorVtVectorWritebackInjects != actual.AssistInterCoreDonorVtVectorWritebackInjects)
		{
			return Mismatch(L"AssistInterCoreDonorVtVectorWritebackInjects", expected.AssistInterCoreDonorVtVectorWritebackInjects, actual.AssistInterCoreDonorVtVectorWritebackInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreLane6DefaultStoreDonorPrefetchInjects != actual.AssistInterCoreLane6DefaultStoreDonorPrefetchInjects)
		{
			return Mismatch(L"AssistInterCoreLane6DefaultStoreDonorPrefetchInjects", expected.AssistInterCoreLane6DefaultStoreDonorPrefetchInjects, actual.AssistInterCoreLane6DefaultStoreDonorPrefetchInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreLane6HotLoadDonorPrefetchInjects != actual.AssistInterCoreLane6HotLoadDonorPrefetchInjects)
		{
			return Mismatch(L"AssistInterCoreLane6HotLoadDonorPrefetchInjects", expected.AssistInterCoreLane6HotLoadDonorPrefetchInjects, actual.AssistInterCoreLane6HotLoadDonorPrefetchInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreLane6HotStoreDonorPrefetchInjects != actual.AssistInterCoreLane6HotStoreDonorPrefetchInjects)
		{
			return Mismatch(L"AssistInterCoreLane6HotStoreDonorPrefetchInjects", expected.AssistInterCoreLane6HotStoreDonorPrefetchInjects, actual.AssistInterCoreLane6HotStoreDonorPrefetchInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreLane6ColdStoreLdsaInjects != actual.AssistInterCoreLane6ColdStoreLdsaInjects)
		{
			return Mismatch(L"AssistInterCoreLane6ColdStoreLdsaInjects", expected.AssistInterCoreLane6ColdStoreLdsaInjects, actual.AssistInterCoreLane6ColdStoreLdsaInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreLane6LdsaInjects != actual.AssistInterCoreLane6LdsaInjects)
		{
			return Mismatch(L"AssistInterCoreLane6LdsaInjects", expected.AssistInterCoreLane6LdsaInjects, actual.AssistInterCoreLane6LdsaInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInterCoreLane6DonorPrefetchInjects != actual.AssistInterCoreLane6DonorPrefetchInjects)
		{
			return Mismatch(L"AssistInterCoreLane6DonorPrefetchInjects", expected.AssistInterCoreLane6DonorPrefetchInjects, actual.AssistInterCoreLane6DonorPrefetchInjects, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistInvalidationReason != actual.AssistInvalidationReason)
		{
			return Mismatch(L"AssistInvalidationReason", expected.AssistInvalidationReason, actual.AssistInvalidationReason, mismatchField, expectedValue, actualValue);
		}

		if (expected.AssistOwnershipSignature != actual.AssistOwnershipSignature)
		{
			return Mismatch(L"AssistOwnershipSignature", std::format(L"0x{:016X}", expected.AssistOwnershipSignature), std::format(L"0x{:016X}", actual.AssistOwnershipSignature), mismatchField, expectedValue, actualValue);
		}

		mismatchField = L"";
		expectedValue = L"";
		actualValue = L"";
		return true;
	}

	bool ReplayEngine::TryCompareReplayEvent(FullStateTraceEvent expected, FullStateTraceEvent actual, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue)
	{
		if (!TryCompareReplayContractEvent(expected, actual, mismatchField, expectedValue, actualValue))
		{
			return false;
		}

		if (expected.CycleNumber != actual.CycleNumber)
		{
			return Mismatch(L"CycleNumber", expected.CycleNumber, actual.CycleNumber, mismatchField, expectedValue, actualValue);
		}

		mismatchField = L"";
		expectedValue = L"";
		actualValue = L"";
		return true;
	}

	bool ReplayEngine::TryCompareEventWithinEnvelope(FullStateTraceEvent expected, FullStateTraceEvent actual, ReplayEnvelopeConfiguration configuration, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue, std::wstring &allowedEnvelope, int &envelopeChecks)
	{
		envelopeChecks = 0;

		if (!TryCompareWithinDelta(L"CycleNumber", expected.CycleNumber, actual.CycleNumber, configuration.getMaxCycleDrift(), mismatchField, expectedValue, actualValue, allowedEnvelope))
		{
			return false;
		}
		envelopeChecks++;

		if (!TryCompareWithinDelta(L"ActiveMemoryRequests", expected.ActiveMemoryRequests, actual.ActiveMemoryRequests, configuration.getMaxActiveMemoryRequestDelta(), mismatchField, expectedValue, actualValue, allowedEnvelope))
		{
			return false;
		}
		envelopeChecks++;

		if (!TryCompareWithinDelta(L"MemorySubsystemCycle", expected.MemorySubsystemCycle, actual.MemorySubsystemCycle, configuration.getMaxMemorySubsystemCycleDrift(), mismatchField, expectedValue, actualValue, allowedEnvelope))
		{
			return false;
		}
		envelopeChecks++;

		int bankChecks;
		if (!TryCompareArrayWithinDelta(L"BankQueueDepths", expected.BankQueueDepths, actual.BankQueueDepths, configuration.getMaxBankQueueDepthDelta(), mismatchField, expectedValue, actualValue, allowedEnvelope, bankChecks))
		{
			return false;
		}
		envelopeChecks += bankChecks;

		int queueChecks;
		if (!TryCompareArrayWithinDelta(L"ThreadReadyQueueDepths", expected.ThreadReadyQueueDepths, actual.ThreadReadyQueueDepths, configuration.getMaxReadyQueueDepthDelta(), mismatchField, expectedValue, actualValue, allowedEnvelope, queueChecks))
		{
			return false;
		}
		envelopeChecks += queueChecks;

		mismatchField = L"";
		expectedValue = L"";
		actualValue = L"";
		allowedEnvelope = L"";
		return true;
	}

	bool ReplayEngine::TryCompareWithinDelta(const std::wstring &field, long long expected, long long actual, long long maxDelta, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue, std::wstring &allowedEnvelope)
	{
		if (std::abs(expected - actual) > maxDelta)
		{
			mismatchField = field;
			expectedValue = std::to_wstring(expected);
			actualValue = std::to_wstring(actual);
			allowedEnvelope = L"+/-" + std::to_wstring(maxDelta);
			return false;
		}

		mismatchField = L"";
		expectedValue = L"";
		actualValue = L"";
		allowedEnvelope = L"";
		return true;
	}

	bool ReplayEngine::TryCompareArrayWithinDelta(const std::wstring &field, std::shared_ptr<std::vector<int>> expected, std::shared_ptr<std::vector<int>> actual, int maxDelta, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue, std::wstring &allowedEnvelope, int &envelopeChecks)
	{
		envelopeChecks = 0;

		if (expected == nullptr || actual == nullptr)
		{
			if (expected == actual)
			{
				mismatchField = L"";
				expectedValue = L"";
				actualValue = L"";
				allowedEnvelope = L"";
				return true;
			}

			mismatchField = field;
			expectedValue = FormatArray(expected);
			actualValue = FormatArray(actual);
			allowedEnvelope = L"matching shape";
			return false;
		}

		if (expected->size() != actual->size())
		{
			mismatchField = field;
			expectedValue = FormatArray(expected);
			actualValue = FormatArray(actual);
			allowedEnvelope = L"matching shape";
			return false;
		}

		for (std::size_t i = 0; i < expected->size(); i++)
		{
			envelopeChecks++;
			if (std::llabs(static_cast<long long>((*expected)[i]) - (*actual)[i]) > maxDelta)
			{
				mismatchField = field + L"[" + std::to_wstring(i) + L"]";
				expectedValue = std::to_wstring((*expected)[i]);
				actualValue = std::to_wstring((*actual)[i]);
				allowedEnvelope = L"+/-" + std::to_wstring(maxDelta);
				return false;
			}
		}

		mismatchField = L"";
		expectedValue = L"";
		actualValue = L"";
		allowedEnvelope = L"";
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool Mismatch(string field, Object? expected, Object? actual, out string mismatchField, out string expectedValue, out string actualValue)
	bool ReplayEngine::Mismatch(const std::wstring &field, std::any expected, std::any actual, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue)
	{
		mismatchField = field;
		expectedValue = FormatValue(expected);
		actualValue = FormatValue(actual);
		return false;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static string FormatValue(Object? value)
	std::wstring ReplayEngine::FormatValue(std::any value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
		if (!value.has_value())
			return L"<null>";
		if (const auto *text = std::any_cast<std::wstring>(&value))
			return *text;
		if (const auto *text = std::any_cast<std::string>(&value))
			return std::wstring(text->begin(), text->end());
		if (const auto *boolean = std::any_cast<bool>(&value))
			return *boolean ? L"True" : L"False";
		if (const auto *number = std::any_cast<int>(&value))
			return std::to_wstring(*number);
		if (const auto *number = std::any_cast<unsigned int>(&value))
			return std::to_wstring(*number);
		if (const auto *number = std::any_cast<long long>(&value))
			return std::to_wstring(*number);
		if (const auto *number = std::any_cast<unsigned long long>(&value))
			return std::to_wstring(*number);
		if (const auto *number = std::any_cast<short>(&value))
			return std::to_wstring(*number);
		if (const auto *number = std::any_cast<unsigned short>(&value))
			return std::to_wstring(*number);
		if (const auto *number = std::any_cast<unsigned char>(&value))
			return std::to_wstring(*number);
		if (const auto *enumeration = std::any_cast<DecodedBundleStateOwnerKind>(&value))
			return std::to_wstring(static_cast<unsigned char>(*enumeration));
		if (const auto *enumeration = std::any_cast<DecodedBundleStateKind>(&value))
			return std::to_wstring(static_cast<unsigned char>(*enumeration));
		if (const auto *enumeration = std::any_cast<DecodedBundleStateOrigin>(&value))
			return std::to_wstring(static_cast<unsigned char>(*enumeration));
		if (const auto *enumeration = std::any_cast<ReplayPhaseInvalidationReason>(&value))
			return std::to_wstring(static_cast<unsigned char>(*enumeration));
		if (const auto *enumeration = std::any_cast<AssistInvalidationReason>(&value))
			return std::to_wstring(static_cast<unsigned char>(*enumeration));
		return L"<unsupported>";
	}

	std::wstring ReplayEngine::FormatArray(std::shared_ptr<std::vector<int>> values)
	{
		if (!values)
			return L"<null>";
		std::wstring result = L"[";
		for (std::size_t i = 0; i < values->size(); ++i)
		{
			if (i != 0)
				result += L",";
			result += std::to_wstring((*values)[i]);
		}
		result += L"]";
		return result;
	}

	int ReplayEngine::CountBits(unsigned char value)
	{
		int count = 0;
		unsigned char remaining = value;
		while (remaining != 0)
		{
			count += remaining & 1;
			remaining >>= 1;
		}

		return count;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static CounterexampleEvidenceSummary ExtractCounterexampleEvidence(IReadOnlyList<OracleCounterexample> counterexamples)
	CounterexampleEvidenceSummary ReplayEngine::ExtractCounterexampleEvidence(const std::vector<OracleCounterexample> &counterexamples)
	{
		if (counterexamples.empty())
		{
			return CounterexampleEvidenceSummary(0, 0, 0, 0, 0, 0, 0);
		}

		int donor = 0, fairness = 0, legality = 0, domain = 0, specBudget = 0, classCapacity = 0;
		long long totalMissed = 0;

		for (auto cx : counterexamples)
		{
			totalMissed += cx.getMissedSlots();
			switch (cx.getCategory())
			{
				case OracleGapCategory::DonorRestriction:
					donor++;
					break;
				case OracleGapCategory::FairnessOrdering:
					fairness++;
					break;
				case OracleGapCategory::LegalityConservatism:
					legality++;
					break;
				case OracleGapCategory::DomainIsolation:
					domain++;
					break;
				case OracleGapCategory::SpeculationBudget:
					specBudget++;
					break;
				case OracleGapCategory::ClassCapacityDivergence:
					classCapacity++;
					break;
			}
		}

		return CounterexampleEvidenceSummary(counterexamples.size(), donor, fairness, legality, domain, specBudget, totalMissed, classCapacity);
	}

	const std::vector<std::wstring> ReplayEngine::GenerateRefinementGuidance(CounterexampleEvidenceSummary evidence)
	{
		std::vector<std::wstring> guidance;

		if (evidence.getTotalCounterexamples() == 0)
		{
			guidance.push_back(L"No refinement guidance needed — no counterexamples detected.");
			return guidance;
		}

		if (evidence.getDonorRestrictionCount() > 0)
		{
			guidance.push_back(std::format(L"DONOR MASK: {} counterexample(s) show ", evidence.getDonorRestrictionCount()) + L"replay-stable donor mask is over-restrictive. Consider widening stable donor mask during long replay epochs.");
		}

		if (evidence.getFairnessOrderingCount() > 0)
		{
			guidance.push_back(std::format(L"FAIRNESS: {} counterexample(s) show ", evidence.getFairnessOrderingCount()) + L"credit-based ordering skipped ready candidates. Consider adjusting FAIRNESS_CREDIT_CAP or tie-break heuristic.");
		}

		if (evidence.getLegalityConservatismCount() > 0)
		{
			guidance.push_back(std::format(L"LEGALITY: {} counterexample(s) show ", evidence.getLegalityConservatismCount()) + L"safety mask or certificate conservatism rejected oracle-legal candidates. Consider auditing RAR-aware hazard detection for false positives.");
		}

		if (evidence.getDomainIsolationCount() > 0)
		{
			guidance.push_back(std::format(L"DOMAIN: {} counterexample(s) show ", evidence.getDomainIsolationCount()) + L"domain isolation enforcement blocked otherwise-legal injections. Review kernel-to-user enforcement scope if workload is single-domain.");
		}

		if (evidence.getSpeculationBudgetCount() > 0)
		{
			guidance.push_back(std::format(L"SPEC BUDGET: {} counterexample(s) show ", evidence.getSpeculationBudgetCount()) + L"speculation budget exhaustion prevented valid injections. Consider increasing SpeculationBudgetMax for workloads with high memory ILP.");
		}

		if (evidence.getClassCapacityDivergenceCount() > 0)
		{
			guidance.push_back(std::format(L"CLASS CAPACITY: {} counterexample(s) show ", evidence.getClassCapacityDivergenceCount()) + L"class-capacity template divergence between runs. Review class-level replay template stability and domain scoping correctness.");
		}

		return guidance;
	}

	bool ReplayEngine::IsSupportedBinaryTraceVersion(unsigned short version)
	{
		// Live replay intentionally keeps only the current schema plus two
		// prior revisions. Older artifacts must be normalized offline before
		// they enter the deterministic replay path.
		constexpr unsigned short liveReplayVersionWindow = 3;
		unsigned short currentVersion = TraceSink::BinaryTraceVersion;
		unsigned short oldestSupportedVersion = currentVersion >= liveReplayVersionWindow ? static_cast<unsigned short>(currentVersion - liveReplayVersionWindow + 1) : static_cast<unsigned short>(1);

		return version >= oldestSupportedVersion && version <= currentVersion;
	}

	ReplayEngine::ReplayEngine(const std::wstring &traceFilePath) : _perThreadEventIndex(std::unordered_map<int, int>()), _checkpoints(std::vector<StateCheckpoint>()), _perThreadTraces(std::unordered_map<int, std::vector<FullStateTraceEvent>>())
	{
		_traceFilePath = traceFilePath;

		// Initialize 16 thread indices
		for (int i = 0; i < 16; i++)
		{
			_perThreadEventIndex[i] = 0;
			_perThreadTraces[i] = std::vector<FullStateTraceEvent>();
		}

		if (!traceFilePath.empty() && FileSystem::fileExists(traceFilePath))
		{
			LoadBinaryTrace(traceFilePath);
		}
	}

	void ReplayEngine::LoadFromTraceSink(std::shared_ptr<TraceSink> traceSink)
	{
		_trace = traceSink;

		// Load per-thread traces
		for (int threadId = 0; threadId < 16; threadId++)
		{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<FullStateTraceEvent> threadTrace = traceSink.GetThreadTrace(threadId);
			const std::vector<FullStateTraceEvent> threadTrace = traceSink->GetThreadTrace(threadId);
			_perThreadTraces[threadId] = std::vector<FullStateTraceEvent>(threadTrace);
		}
	}

	void ReplayEngine::LoadBinaryTrace(const std::wstring &filePath)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using FileStream fs = new FileStream(filePath, FileMode.Open);
		{
			auto fs = std::make_shared<FileStream>(filePath, FileMode::OpenRead);
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using BinaryReader reader = new BinaryReader(fs);
			{
				auto reader = std::make_shared<BinaryReader>(fs);
    
				// Read header
				if (reader->getLength() > 512LL * 1024 * 1024) throw InvalidDataException(L"Trace file exceeds the replay bound.");
				unsigned int magic = reader->ReadUInt32();
				if (magic != 0x54524143) // "TRAC"
				{
					throw InvalidDataException(L"Invalid trace file magic number");
				}
    
				unsigned short version = reader->ReadUInt16();
				if (!IsSupportedBinaryTraceVersion(version))
				{
					throw InvalidDataException(std::format(L"Unsupported trace version: {}", version));
				}
    
				int threadCount = ReadBoundedCount(reader, 16, 0);
    
				// Read checkpoints
				int checkpointCount = ReadBoundedCount(reader, 1000000, 24);
				for (int i = 0; i < checkpointCount; i++)
				{
					_checkpoints.push_back(DeserializeCheckpoint(reader));
				}
    
				// Read per-thread traces
				for (int i = 0; i < threadCount; i++)
				{
					int threadId = reader->ReadInt32();
					if (threadId < 0 || threadId >= 16) throw InvalidDataException(L"Invalid trace thread id.");
					int eventCount = ReadBoundedCount(reader, 1000000, 83);
    
					if (!_perThreadTraces.contains(threadId))
					{
						_perThreadTraces[threadId] = std::vector<FullStateTraceEvent>();
					}
    
					for (int j = 0; j < eventCount; j++)
					{
						_perThreadTraces[threadId].push_back(DeserializeFullStateEvent(reader, version));
					}
				}
			}
		}
	}

	StateCheckpoint ReplayEngine::DeserializeCheckpoint(std::shared_ptr<BinaryReader> reader)
	{
		StateCheckpoint checkpoint = {.CycleNumber = reader->ReadInt64()};

		int procStateLen = ReadBoundedCount(reader, 64 * 1024 * 1024, 1);
		if (procStateLen > 0)
		{
			checkpoint.ProcessorState = reader->ReadBytesExact(procStateLen, 64u * 1024u * 1024u);
		}
		else
		{
			checkpoint.ProcessorState = {};
		}

		int memSnapLen = ReadBoundedCount(reader, 64 * 1024 * 1024, 1);
		if (memSnapLen > 0)
		{
			checkpoint.MemorySnapshot = reader->ReadBytesExact(memSnapLen, 64u * 1024u * 1024u);
		}
		else
		{
			checkpoint.MemorySnapshot = {};
		}

		checkpoint.RandomState = reader->ReadUInt64();

		return checkpoint;
	}

	FullStateTraceEvent ReplayEngine::DeserializeFullStateEvent(std::shared_ptr<BinaryReader> reader, unsigned short version)
	{
		FullStateTraceEvent evt = {.PC = reader->ReadInt64(), .BundleId = reader->ReadInt32(), .OpIndex = reader->ReadInt32(), .Opcode = reader->ReadUInt32(), .ThreadId = reader->ReadInt32(), .CycleNumber = reader->ReadInt64()};

		// Register file
		int regFileLen = ReadBoundedCount(reader, 4096, 8);
		if (regFileLen > 0)
		{
			evt.RegisterFile = std::make_shared<std::vector<unsigned long long>>(regFileLen);
			for (int i = 0; i < regFileLen; i++)
			{
				(*evt.RegisterFile)[i] = reader->ReadUInt64();
			}
		}

		// Vector registers
		int vregLen = ReadBoundedCount(reader, 65536, 8);
		if (vregLen > 0)
		{
			evt.VectorRegisters = std::make_shared<std::vector<unsigned long long>>(vregLen);
			for (int i = 0; i < vregLen; i++)
			{
				(*evt.VectorRegisters)[i] = reader->ReadUInt64();
			}
		}

		// Predicate registers
		int pregLen = ReadBoundedCount(reader, 65536, 2);
		if (pregLen > 0)
		{
			evt.PredicateRegisters = std::make_shared<std::vector<unsigned short>>(pregLen);
			for (int i = 0; i < pregLen; i++)
			{
				(*evt.PredicateRegisters)[i] = reader->ReadUInt16();
			}
		}

		// FP context
		evt.FPContext.RoundingMode = reader->ReadInt32();
		evt.FPContext.FlushToZero = reader->ReadBoolean();
		evt.FPContext.DefaultNaN = reader->ReadBoolean();

		// Memory writes
		int memWriteCount = ReadBoundedCount(reader, 1000000, 16);
		if (memWriteCount > 0)
		{
			evt.MemoryWrites = std::make_shared<std::vector<MemoryDelta>>(memWriteCount);
			for (int i = 0; i < memWriteCount; i++)
			{
				unsigned long long addr = reader->ReadUInt64();
				int oldLen = ReadBoundedCount(reader, 64 * 1024 * 1024, 1);
				std::vector<unsigned char> oldData = oldLen > 0 ? reader->ReadBytesExact(oldLen, 64u * 1024u * 1024u) : std::vector<unsigned char>{};
				int newLen = ReadBoundedCount(reader, 64 * 1024 * 1024, 1);
				std::vector<unsigned char> newData = newLen > 0 ? reader->ReadBytesExact(newLen, 64u * 1024u * 1024u) : std::vector<unsigned char>{};
				(*evt.MemoryWrites)[i] = MemoryDelta(addr, oldData, newData);
			}
		}

		// FSP state
		evt.WasStolenSlot = reader->ReadBoolean();
		evt.OriginalThreadId = reader->ReadInt32();

		// Pipeline state
		evt.PipelineStage = reader->ReadString();
		evt.Stalled = reader->ReadBoolean();
		evt.StallReason = reader->ReadString();
		if (version >= 23)
		{
			evt.DecodedBundleStateOwnerKind = static_cast<DecodedBundleStateOwnerKind>(reader->ReadByte());
			evt.DecodedBundleStateEpoch = reader->ReadUInt64();
			evt.DecodedBundleStateVersion = reader->ReadUInt64();
		}
		if (version >= 22)
		{
			evt.DecodedBundleStateKind = static_cast<DecodedBundleStateKind>(reader->ReadByte());
			evt.DecodedBundleStateOrigin = static_cast<DecodedBundleStateOrigin>(reader->ReadByte());
			evt.DecodedBundlePc = reader->ReadUInt64();
			evt.DecodedBundleValidMask = reader->ReadByte();
			evt.DecodedBundleNopMask = reader->ReadByte();
			evt.DecodedBundleHasCanonicalDecode = reader->ReadBoolean();
			evt.DecodedBundleHasCanonicalLegality = reader->ReadBoolean();
			evt.DecodedBundleHasDecodeFault = reader->ReadBoolean();
		}

		// Memory subsystem
		int bankQueueLen = ReadBoundedCount(reader, 4096, 4);
		if (bankQueueLen > 0)
		{
			evt.BankQueueDepths = std::make_shared<std::vector<int>>(bankQueueLen);
			for (int i = 0; i < bankQueueLen; i++)
			{
				(*evt.BankQueueDepths)[i] = reader->ReadInt32();
			}
		}
		evt.ActiveMemoryRequests = reader->ReadInt32();
		evt.MemorySubsystemCycle = reader->ReadInt64();

		// Scheduler state
		int threadQueueLen = ReadBoundedCount(reader, 65536, 4);
		if (threadQueueLen > 0)
		{
			evt.ThreadReadyQueueDepths = std::make_shared<std::vector<int>>(threadQueueLen);
			for (int i = 0; i < threadQueueLen; i++)
			{
				(*evt.ThreadReadyQueueDepths)[i] = reader->ReadInt32();
			}
		}
		evt.CurrentFSPPolicy = reader->ReadString();

		if (version >= 2)
		{
			evt.ReplayEpochId = reader->ReadUInt64();
			evt.ReplayPhaseCachedPc = reader->ReadUInt64();
			evt.ReplayEpochLength = reader->ReadUInt64();
			evt.ReplayPhaseValidSlotCount = reader->ReadInt32();
			evt.StableDonorMask = reader->ReadByte();
			evt.ReplayInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(reader->ReadByte());
			evt.PhaseCertificateTemplateReusable = reader->ReadBoolean();
			evt.PhaseCertificateReadyHits = reader->ReadInt64();
			evt.PhaseCertificateReadyMisses = reader->ReadInt64();
			if (version >= 3)
			{
				evt.EstimatedPhaseCertificateChecksSaved = reader->ReadInt64();
			}
			evt.PhaseCertificateInvalidations = reader->ReadInt64();
			evt.PhaseCertificateInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(reader->ReadByte());
			if (version >= 4)
			{
				evt.DeterminismReferenceOpportunitySlots = reader->ReadInt64();
				evt.DeterminismReplayEligibleSlots = reader->ReadInt64();
				evt.DeterminismMaskedSlots = reader->ReadInt64();
				evt.DeterminismEstimatedLostSlots = reader->ReadInt64();
				evt.DeterminismConstrainedCycles = reader->ReadInt64();
				evt.DomainIsolationProbeAttempts = reader->ReadInt64();
				evt.DomainIsolationBlockedAttempts = reader->ReadInt64();
				evt.DomainIsolationCrossDomainBlocks = reader->ReadInt64();
				evt.DomainIsolationKernelToUserBlocks = reader->ReadInt64();
			}

			if (version >= 5)
			{
				evt.EligibilityMaskedCycles = reader->ReadInt64();
				evt.EligibilityMaskedReadyCandidates = reader->ReadInt64();
				evt.LastEligibilityRequestedMask = reader->ReadByte();
				evt.LastEligibilityNormalizedMask = reader->ReadByte();
				evt.LastEligibilityReadyPortMask = reader->ReadByte();
				evt.LastEligibilityVisibleReadyMask = reader->ReadByte();
				evt.LastEligibilityMaskedReadyMask = reader->ReadByte();
			}

			if (version >= 6)
			{
				evt.AssistNominations = reader->ReadInt64();
				evt.AssistInjections = reader->ReadInt64();
				evt.AssistRejects = reader->ReadInt64();
				evt.AssistBoundaryRejects = reader->ReadInt64();
				evt.AssistInvalidations = reader->ReadInt64();
				evt.AssistInterCoreNominations = reader->ReadInt64();
				evt.AssistInterCoreInjections = reader->ReadInt64();
				evt.AssistInterCoreRejects = reader->ReadInt64();
				evt.AssistInterCoreDomainRejects = reader->ReadInt64();
				if (version >= 7)
				{
					evt.AssistInterCorePodLocalInjections = reader->ReadInt64();
					evt.AssistInterCoreCrossPodInjections = reader->ReadInt64();
					evt.AssistInterCorePodLocalRejects = reader->ReadInt64();
					evt.AssistInterCoreCrossPodRejects = reader->ReadInt64();
					evt.AssistInterCorePodLocalDomainRejects = reader->ReadInt64();
					evt.AssistInterCoreCrossPodDomainRejects = reader->ReadInt64();
					if (version >= 8)
					{
						evt.AssistInterCoreSameVtVectorInjects = reader->ReadInt64();
						evt.AssistInterCoreDonorVtVectorInjects = reader->ReadInt64();
						if (version >= 13)
						{
							evt.AssistInterCoreSameVtVectorWritebackInjects = reader->ReadInt64();
							evt.AssistInterCoreDonorVtVectorWritebackInjects = reader->ReadInt64();
						}
						if (version >= 9)
						{
							if (version >= 11 && version < 18)
							{
								reader->ReadInt64();
							}
							if (version >= 17)
							{
								evt.AssistInterCoreLane6DefaultStoreDonorPrefetchInjects = reader->ReadInt64();
							}
							if (version >= 14)
							{
								if (version >= 19)
								{
									evt.AssistInterCoreLane6HotLoadDonorPrefetchInjects = reader->ReadInt64();
								}
								else
								{
									reader->ReadInt64();
								}
							}
							if (version >= 15)
							{
								evt.AssistInterCoreLane6HotStoreDonorPrefetchInjects = reader->ReadInt64();
							}
							if (version >= 12 && version < 18)
							{
								reader->ReadInt64();
							}
							if (version >= 10)
							{
								evt.AssistInterCoreLane6DonorPrefetchInjects = reader->ReadInt64();
							}
							if (version >= 16)
							{
								evt.AssistInterCoreLane6ColdStoreLdsaInjects = reader->ReadInt64();
							}
							evt.AssistInterCoreLane6LdsaInjects = reader->ReadInt64();

						}
					}
				}
				evt.AssistQuotaRejects = reader->ReadInt64();
				evt.AssistQuotaIssueRejects = reader->ReadInt64();
				evt.AssistQuotaLineRejects = reader->ReadInt64();
				evt.AssistQuotaLinesReserved = reader->ReadInt64();
				evt.AssistBackpressureRejects = reader->ReadInt64();
				evt.AssistBackpressureOuterCapRejects = reader->ReadInt64();
				evt.AssistBackpressureMshrRejects = reader->ReadInt64();
				evt.AssistBackpressureDmaSrfRejects = reader->ReadInt64();
				evt.AssistDonorPrefetchInjects = reader->ReadInt64();
				evt.AssistLdsaInjects = reader->ReadInt64();
				evt.AssistVdsaInjects = reader->ReadInt64();
				evt.AssistSameVtInjects = reader->ReadInt64();
				evt.AssistDonorVtInjects = reader->ReadInt64();
				evt.AssistInvalidationReason = static_cast<AssistInvalidationReason>(reader->ReadByte());
				evt.AssistOwnershipSignature = reader->ReadUInt64();
			}
		}

		return evt;
	}

	std::optional<StateCheckpoint> ReplayEngine::FindNearestCheckpoint(long long targetCycle)
	{
		if (_checkpoints.empty())
		{
			return std::nullopt;
		}

		// Find the checkpoint with the highest cycle number <= targetCycle
		std::optional<StateCheckpoint> nearest;
		for (auto checkpoint : _checkpoints)
		{
			if (checkpoint.CycleNumber <= targetCycle)
			{
				if (!nearest || checkpoint.CycleNumber > nearest.value().CycleNumber)
				{
					nearest = checkpoint;
				}
			}
		}

		return nearest;
	}

	std::vector<FullStateTraceEvent> ReplayEngine::GetEventsForCycle(long long cycle)
	{
		std::vector<FullStateTraceEvent> events;

		for (const auto &[threadId, threadTrace] : _perThreadTraces)
		{
			for (auto evt : threadTrace)
			{
				if (evt.CycleNumber == cycle)
				{
					events.push_back(evt);
				}
			}
		}

		return events;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool ReplayToCycle(long targetCycle, System.Action<FullStateTraceEvent>? eventCallback = null)
	bool ReplayEngine::ReplayToCycle(long long targetCycle, std::function<void(FullStateTraceEvent)> eventCallback)
	{
		// 1. Find nearest checkpoint before target
		std::optional<StateCheckpoint> checkpoint = FindNearestCheckpoint(targetCycle);
		long long startCycle = checkpoint ? checkpoint->CycleNumber : 0;

		// 2. Replay from checkpoint (or beginning) to target
		long long currentCycle = startCycle;

		while (currentCycle <= targetCycle)
		{
			// Get events for this cycle across all threads
			std::vector<FullStateTraceEvent> cycleEvents = GetEventsForCycle(currentCycle);

			// Replay events in deterministic order (by thread ID, then op index)
			std::stable_sort(cycleEvents.begin(), cycleEvents.end(), [](const FullStateTraceEvent &left, const FullStateTraceEvent &right)
			{
				return std::tie(left.ThreadId, left.OpIndex) < std::tie(right.ThreadId, right.OpIndex);
			});
			for (const FullStateTraceEvent &evt : cycleEvents)
			{
				if (eventCallback)
					eventCallback(evt);
				// In full implementation, would restore processor state here
			}

			currentCycle++;
		}

		return true;
	}

	std::tuple<int, int, long long> ReplayEngine::GetStatistics()
	{
		int totalEvents = 0;
		int threadsWithEvents = 0;
		long long maxCycle = 0;

		for (const auto &entry : _perThreadTraces)
		{
			const auto &threadTrace = entry.second;
			if (!threadTrace.empty())
			{
				threadsWithEvents++;
				totalEvents += static_cast<int>(threadTrace.size());

				long long threadMaxCycle = std::max_element(threadTrace.begin(), threadTrace.end(),
					[](const FullStateTraceEvent &left, const FullStateTraceEvent &right)
					{
						return left.CycleNumber < right.CycleNumber;
					})->CycleNumber;
				if (threadMaxCycle > maxCycle)
				{
					maxCycle = threadMaxCycle;
				}
			}
		}

		return {totalEvents, threadsWithEvents, maxCycle};
	}

	bool ReplayEngine::VerifyTraceIntegrity()
	{
		// Check that all thread traces are ordered by cycle number
		for (const auto &[threadId, trace] : _perThreadTraces)
		{
			for (int i = 1; i < trace.size(); i++)
			{
				if (trace[i].CycleNumber < trace[i - 1].CycleNumber)
				{
					return false; // Not monotonically increasing
				}
			}
		}

		return VerifyReplayPhaseConsistency();
	}

	bool ReplayEngine::VerifyReplayPhaseConsistency()
	{
		for (const auto &[threadId, trace] : _perThreadTraces)
		{
			unsigned long long currentEpochId = 0;
			unsigned long long cachedPc = 0;
			unsigned long long epochLength = 0;
			int validSlotCount = 0;
			unsigned char donorMask = 0;

			for (const auto &evt : trace)
			{
				if (evt.ReplayEpochId == 0)
				{
					if (evt.ReplayEpochLength != 0 || evt.StableDonorMask != 0)
					{
						return false;
					}

					continue;
				}

				if (currentEpochId != evt.ReplayEpochId)
				{
					currentEpochId = evt.ReplayEpochId;
					cachedPc = evt.ReplayPhaseCachedPc;
					epochLength = evt.ReplayEpochLength;
					validSlotCount = evt.ReplayPhaseValidSlotCount;
					donorMask = evt.StableDonorMask;
				}
				else if (cachedPc != evt.ReplayPhaseCachedPc || epochLength != evt.ReplayEpochLength || validSlotCount != evt.ReplayPhaseValidSlotCount || donorMask != evt.StableDonorMask)
				{
					return false;
				}

				if (evt.PhaseCertificateTemplateReusable && evt.PhaseCertificateInvalidationReason == ReplayPhaseInvalidationReason::CertificateMutation)
				{
					return false;
				}
			}
		}

		return true;
	}

	ReplayDeterminismReport ReplayEngine::CompareRepeatedRuns(std::shared_ptr<TraceSink> baselineTrace, std::shared_ptr<TraceSink> candidateTrace)
	{
		ArgumentNullException::ThrowIfNull(baselineTrace);
		ArgumentNullException::ThrowIfNull(candidateTrace);

		std::shared_ptr<ReplayEngine> baselineReplay = std::make_shared<ReplayEngine>();
		baselineReplay->LoadFromTraceSink(baselineTrace);

		std::shared_ptr<ReplayEngine> candidateReplay = std::make_shared<ReplayEngine>();
		candidateReplay->LoadFromTraceSink(candidateTrace);

		return baselineReplay->CompareReplayPhaseBehavior(candidateReplay);
	}

	ReplayEnvelopeReport ReplayEngine::CompareRepeatedRunsWithinEnvelope(std::shared_ptr<TraceSink> baselineTrace, std::shared_ptr<TraceSink> candidateTrace, ReplayEnvelopeConfiguration configuration)
	{
		ArgumentNullException::ThrowIfNull(baselineTrace);
		ArgumentNullException::ThrowIfNull(candidateTrace);

		std::shared_ptr<ReplayEngine> baselineReplay = std::make_shared<ReplayEngine>();
		baselineReplay->LoadFromTraceSink(baselineTrace);

		std::shared_ptr<ReplayEngine> candidateReplay = std::make_shared<ReplayEngine>();
		candidateReplay->LoadFromTraceSink(candidateTrace);

		return baselineReplay->CompareReplayPhaseBehaviorWithinEnvelope(candidateReplay, configuration);
	}

	ReplayTraceEvidenceSummary ReplayEngine::SummarizeReplayPhaseEvidence(std::shared_ptr<TraceSink> traceSink)
	{
		ArgumentNullException::ThrowIfNull(traceSink);

		std::shared_ptr<ReplayEngine> replay = std::make_shared<ReplayEngine>();
		replay->LoadFromTraceSink(traceSink);
		return replay->SummarizeReplayPhaseEvidence();
	}

	std::vector<ReplayEpochEvidenceSummary> ReplayEngine::SummarizeReplayEpochEvidence(std::shared_ptr<TraceSink> traceSink)
	{
		ArgumentNullException::ThrowIfNull(traceSink);

		std::shared_ptr<ReplayEngine> replay = std::make_shared<ReplayEngine>();
		replay->LoadFromTraceSink(traceSink);
		return replay->SummarizeReplayEpochEvidence();
	}

	ReplayDeterminismReport ReplayEngine::CompareReplayPhaseBehavior(std::shared_ptr<ReplayEngine> other)
	{
		ArgumentNullException::ThrowIfNull(other);

		int comparedEvents = 0;
		int comparedReplayEvents = 0;
		int comparedTimelineSamples = 0;
		int comparedInvalidationEvents = 0;
		int comparedEpochs = 0;

		for (int threadId = 0; threadId < 16; threadId++)
		{
			std::vector<FullStateTraceEvent> baselineTrace = _perThreadTraces[threadId];
			std::vector<FullStateTraceEvent> candidateTrace = other->_perThreadTraces[threadId];

			if (baselineTrace.size() != candidateTrace.size())
			{
				return ReplayDeterminismReport(false, comparedEvents, comparedReplayEvents, comparedTimelineSamples, comparedInvalidationEvents, comparedEpochs, threadId, -1, L"EventCount", std::to_wstring(baselineTrace.size()), std::to_wstring(candidateTrace.size()));
			}

			unsigned long long lastEpochId = 0;
			for (int index = 0; index < baselineTrace.size(); index++)
			{
				FullStateTraceEvent baselineEvent = baselineTrace[index];
				FullStateTraceEvent candidateEvent = candidateTrace[index];

				std::wstring mismatchField;
				std::wstring expectedValue;
				std::wstring actualValue;
				if (!TryCompareReplayEvent(baselineEvent, candidateEvent, mismatchField, expectedValue, actualValue))
				{
					return ReplayDeterminismReport(false, comparedEvents, comparedReplayEvents, comparedTimelineSamples, comparedInvalidationEvents, comparedEpochs, threadId, baselineEvent.CycleNumber, mismatchField, expectedValue, actualValue);
				}

				comparedEvents++;

				if (baselineEvent.ReplayEpochId != 0)
				{
					comparedReplayEvents++;
					if (baselineEvent.ReplayEpochId != lastEpochId)
					{
						comparedEpochs++;
						lastEpochId = baselineEvent.ReplayEpochId;
					}
				}

				if (IsDensePhaseTimelineSample(baselineEvent))
				{
					comparedTimelineSamples++;
				}

				if (HasInvalidationSignal(baselineEvent))
				{
					comparedInvalidationEvents++;
				}
			}
		}

		return ReplayDeterminismReport(true, comparedEvents, comparedReplayEvents, comparedTimelineSamples, comparedInvalidationEvents, comparedEpochs, -1, -1, L"", L"", L"");
	}
}
