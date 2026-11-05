#include "TraceSink.h"
#include "../Architecture/Common/BinaryIOCompat.h"
#include "../../../tangible_exception_helper.h"
#include <limits>
#include <filesystem>
#include <fstream>
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core;
using System::IO::FileMode;
using System::IO::FileStream;

namespace HybridCPU_ISE::Core
{
	namespace
	{
		int BinaryCount(size_t count)
		{
			if (count > static_cast<size_t>((std::numeric_limits<int>::max)())) throw InvalidDataException(L"Trace collection exceeds Int32 encoding bound.");
			return static_cast<int>(count);
		}

	std::wstring FormatAny(const std::any &value)
	{
		if (!value.has_value()) return L"";
		if (const auto item = std::any_cast<std::wstring>(&value)) return *item;
		if (const auto item = std::any_cast<std::string>(&value))
		{
			const auto *bytes = reinterpret_cast<const unsigned char *>(item->data());
			return System::IO::FromUtf8(std::span<const unsigned char>(bytes, item->size()));
		}
		if (const auto item = std::any_cast<bool>(&value)) return *item ? L"True" : L"False";
		if (const auto item = std::any_cast<unsigned char>(&value)) return std::to_wstring(static_cast<unsigned int>(*item));
		if (const auto item = std::any_cast<unsigned short>(&value)) return std::to_wstring(*item);
		if (const auto item = std::any_cast<unsigned int>(&value)) return std::to_wstring(*item);
		if (const auto item = std::any_cast<unsigned long long>(&value)) return std::to_wstring(*item);
		if (const auto item = std::any_cast<short>(&value)) return std::to_wstring(*item);
		if (const auto item = std::any_cast<int>(&value)) return std::to_wstring(*item);
		if (const auto item = std::any_cast<long long>(&value)) return std::to_wstring(*item);
		if (const auto item = std::any_cast<float>(&value)) return std::format(L"{}", *item);
		if (const auto item = std::any_cast<double>(&value)) return std::format(L"{}", *item);
		return L"";
	}

	std::wstring JsonQuote(const std::wstring &value)
	{
		std::wstring result = L"\"";
		for (wchar_t ch : value)
		{
			switch (ch)
			{
			case L'\"': result += L"\\\""; break;
			case L'\\': result += L"\\\\"; break;
			case L'\b': result += L"\\b"; break;
			case L'\f': result += L"\\f"; break;
			case L'\n': result += L"\\n"; break;
			case L'\r': result += L"\\r"; break;
			case L'\t': result += L"\\t"; break;
			default:
				if (ch < 0x20) result += std::format(L"\\u{:04X}", static_cast<unsigned int>(ch));
				else result += ch;
				break;
			}
		}
		result += L'\"';
		return result;
	}

	std::wstring FormatAnyJson(const std::any &value)
	{
		if (!value.has_value()) return L"null";
		if (std::any_cast<bool>(&value)) return *std::any_cast<bool>(&value) ? L"true" : L"false";
		if (std::any_cast<short>(&value) || std::any_cast<int>(&value) || std::any_cast<long long>(&value) ||
			std::any_cast<unsigned char>(&value) || std::any_cast<unsigned short>(&value) || std::any_cast<unsigned int>(&value) ||
			std::any_cast<unsigned long long>(&value) || std::any_cast<float>(&value) || std::any_cast<double>(&value))
		{
			return FormatAny(value);
		}
		return JsonQuote(FormatAny(value));
	}

	std::wstring FormatOperands(const std::vector<std::any> &operands)
	{
		std::wstring result;
		for (size_t index = 0; index < operands.size(); ++index)
		{
			if (index != 0) result += L';';
			result += FormatAny(operands[index]);
		}
		return result;
	}

	std::wstring FormatOperandsJson(const std::vector<std::any> &operands)
	{
		std::wstring result = L"[";
		for (size_t index = 0; index < operands.size(); ++index)
		{
			if (index != 0) result += L", ";
			result += FormatAnyJson(operands[index]);
		}
		result += L']';
		return result;
	}

	std::wstring EscapeCsv(const std::wstring &value)
	{
		std::wstring result;
		result.reserve(value.size());
		for (wchar_t ch : value)
		{
			result += ch;
			if (ch == L'\"') result += L'\"';
		}
		return result;
	}
	}

	TraceEvent::TraceEvent(long long pc, int bundleId, int opIndex, unsigned int opcode)
	{
		PC = pc;
		BundleId = bundleId;
		OpIndex = opIndex;
		Opcode = opcode;
		Operands = {};
		PredicateMask = 0xFFFFFFFFFFFFFFFFULL;
		Flags = HybridCPU_ISE::Core::VectorExceptionFlags();
		Result = std::any();
		ExceptionCount = 0;
	}

	bool VectorExceptionFlags::getHasException() const
	{
		return Overflow || Underflow || DivByZero || InvalidOp || Inexact;
	}

	std::wstring VectorExceptionFlags::ToString()
	{
		if (!getHasException())
		{
			return L"None";
		}
		std::vector<std::wstring> flags;
		if (Overflow)
		{
			flags.push_back(L"OV");
		}
		if (Underflow)
		{
			flags.push_back(L"UF");
		}
		if (DivByZero)
		{
			flags.push_back(L"DZ");
		}
		if (InvalidOp)
		{
			flags.push_back(L"IO");
		}
		if (Inexact)
		{
			flags.push_back(L"IX");
		}
		std::wstring result;
		for (const auto &flag : flags)
		{
			if (!result.empty()) result += L'|';
			result += flag;
		}
		return result;
	}

	MemoryDelta::MemoryDelta(unsigned long long address, std::vector<unsigned char> &oldData, std::vector<unsigned char> &newData)
	{
		Address = address;
		OldData = oldData;
		NewData = newData;
	}

	FPExceptionContext::FPExceptionContext()
	{
		Flags = HybridCPU_ISE::Core::VectorExceptionFlags();
		RoundingMode = 0; // RNE by default
		FlushToZero = false;
		DefaultNaN = false;
	}

	TraceSink::TraceSink(TraceFormat format, const std::wstring &filePath) : events(std::vector<TraceEvent>()), format(format), filePath(filePath), _perThreadTraces(std::unordered_map<int, std::vector<FullStateTraceEvent>>()), _checkpoints(std::vector<StateCheckpoint>()), _fsmTransitions(std::vector<FsmTransitionEvent>()), _v4Events(std::vector<V4TraceEvent>())
	{
		this->enabled = false;
		this->level = TraceLevel::Summary;

		// Phase 5: Initialize per-thread trace storage

		// Phase 11: v4 event log

		// Initialize 16 thread traces
		for (int i = 0; i < 16; i++)
		{
			_perThreadTraces[i] = std::vector<FullStateTraceEvent>();
		}
	}

	void TraceSink::SetEnabled(bool enabled)
	{
		this->enabled = enabled;
	}

	void TraceSink::SetLevel(TraceLevel level)
	{
		this->level = level;
	}

	bool TraceSink::getShouldCaptureFullState() const
	{
		return enabled && level == TraceLevel::Full;
	}

	void TraceSink::Record(TraceEvent evt)
	{
		if (!enabled || level == TraceLevel::None)
		{
			return;
		}

		// For Summary level, clear operands and result to save memory
		if (level == TraceLevel::Summary)
		{
			evt.Operands = {};
			evt.Result = std::any();
		}

		events.push_back(evt);
	}

	void TraceSink::Clear()
	{
		events.clear();
	}

	int TraceSink::getEventCount() const
	{
		return events.size();
	}

	void TraceSink::SetCheckpointInterval(long long interval)
	{
		_checkpointInterval = interval;
	}

	void TraceSink::RecordFullState(FullStateTraceEvent evt)
	{
		if (!getShouldCaptureFullState())
		{
			return;
		}

		if (evt.ThreadId >= 0 && evt.ThreadId < 16)
		{
			_perThreadTraces[evt.ThreadId].push_back(evt);
			_currentCycle = evt.CycleNumber;

			// Check if we need to create a checkpoint
			if (_checkpointInterval > 0 && evt.CycleNumber % _checkpointInterval == 0)
			{
				// Checkpoint creation would be triggered here
				// For now, we mark the opportunity
			}
		}
	}

	void TraceSink::RecordFsmTransition(long long cycleNumber, int vtId, PipelineState fromState, PipelineTransitionTrigger trigger, PipelineState toState)
	{
		if (!enabled)
		{
			return;
		}

		FsmTransitionEvent temp_var = FsmTransitionEvent();
		temp_var.CycleNumber = cycleNumber;
		temp_var.VtId = vtId;
		temp_var.FromState = fromState;
		temp_var.Trigger = trigger;
		temp_var.ToState = toState;
		_fsmTransitions.push_back(temp_var);
	}

	const std::vector<FsmTransitionEvent> TraceSink::GetFsmTransitions()
	{
		return _fsmTransitions;
	}

	void TraceSink::RecordPhaseAwareState(FullStateTraceEvent evt, ReplayPhaseContext phaseContext, SchedulerPhaseMetrics schedulerMetrics, bool phaseCertificateTemplateReusable)
	{
		evt.ReplayEpochId = phaseContext.getEpochId();
		evt.ReplayPhaseCachedPc = phaseContext.getCachedPc();
		evt.ReplayEpochLength = phaseContext.getEpochLength();
		evt.ReplayPhaseValidSlotCount = phaseContext.getValidSlotCount();
		evt.StableDonorMask = phaseContext.getStableDonorMask();
		evt.ReplayInvalidationReason = phaseContext.getLastInvalidationReason();
		evt.PhaseCertificateTemplateReusable = phaseCertificateTemplateReusable;
		evt.PhaseCertificateReadyHits = schedulerMetrics.getPhaseCertificateReadyHits();
		evt.PhaseCertificateReadyMisses = schedulerMetrics.getPhaseCertificateReadyMisses();
		evt.EstimatedPhaseCertificateChecksSaved = schedulerMetrics.getEstimatedChecksSaved();
		evt.PhaseCertificateInvalidations = schedulerMetrics.getPhaseCertificateInvalidations();
		evt.PhaseCertificateInvalidationReason = schedulerMetrics.getLastCertificateInvalidationReason();
		evt.DeterminismReferenceOpportunitySlots = schedulerMetrics.getDeterminismReferenceOpportunitySlots();
		evt.DeterminismReplayEligibleSlots = schedulerMetrics.getDeterminismReplayEligibleSlots();
		evt.DeterminismMaskedSlots = schedulerMetrics.getDeterminismMaskedSlots();
		evt.DeterminismEstimatedLostSlots = schedulerMetrics.getDeterminismEstimatedLostSlots();
		evt.DeterminismConstrainedCycles = schedulerMetrics.getDeterminismConstrainedCycles();
		evt.DomainIsolationProbeAttempts = schedulerMetrics.getDomainIsolationProbeAttempts();
		evt.DomainIsolationBlockedAttempts = schedulerMetrics.getDomainIsolationBlockedAttempts();
		evt.DomainIsolationCrossDomainBlocks = schedulerMetrics.getDomainIsolationCrossDomainBlocks();
		evt.DomainIsolationKernelToUserBlocks = schedulerMetrics.getDomainIsolationKernelToUserBlocks();
		evt.EligibilityMaskedCycles = schedulerMetrics.getEligibilityMaskedCycles();
		evt.EligibilityMaskedReadyCandidates = schedulerMetrics.getEligibilityMaskedReadyCandidates();
		evt.LastEligibilityRequestedMask = schedulerMetrics.getLastEligibilityRequestedMask();
		evt.LastEligibilityNormalizedMask = schedulerMetrics.getLastEligibilityNormalizedMask();
		evt.LastEligibilityReadyPortMask = schedulerMetrics.getLastEligibilityReadyPortMask();
		evt.LastEligibilityVisibleReadyMask = schedulerMetrics.getLastEligibilityVisibleReadyMask();
		evt.LastEligibilityMaskedReadyMask = schedulerMetrics.getLastEligibilityMaskedReadyMask();
		evt.AssistNominations = schedulerMetrics.getAssistNominations();
		evt.AssistInjections = schedulerMetrics.getAssistInjections();
		evt.AssistRejects = schedulerMetrics.getAssistRejects();
		evt.AssistBoundaryRejects = schedulerMetrics.getAssistBoundaryRejects();
		evt.AssistInvalidations = schedulerMetrics.getAssistInvalidations();
		evt.AssistInterCoreNominations = schedulerMetrics.getAssistInterCoreNominations();
		evt.AssistInterCoreInjections = schedulerMetrics.getAssistInterCoreInjections();
		evt.AssistInterCoreRejects = schedulerMetrics.getAssistInterCoreRejects();
		evt.AssistInterCoreDomainRejects = schedulerMetrics.getAssistInterCoreDomainRejects();
		evt.AssistInterCorePodLocalInjections = schedulerMetrics.getAssistInterCorePodLocalInjections();
		evt.AssistInterCoreCrossPodInjections = schedulerMetrics.getAssistInterCoreCrossPodInjections();
		evt.AssistInterCorePodLocalRejects = schedulerMetrics.getAssistInterCorePodLocalRejects();
		evt.AssistInterCoreCrossPodRejects = schedulerMetrics.getAssistInterCoreCrossPodRejects();
		evt.AssistInterCorePodLocalDomainRejects = schedulerMetrics.getAssistInterCorePodLocalDomainRejects();
		evt.AssistInterCoreCrossPodDomainRejects = schedulerMetrics.getAssistInterCoreCrossPodDomainRejects();
		evt.AssistInterCoreSameVtVectorInjects = schedulerMetrics.getAssistInterCoreSameVtVectorInjects();
		evt.AssistInterCoreDonorVtVectorInjects = schedulerMetrics.getAssistInterCoreDonorVtVectorInjects();
		evt.AssistInterCoreSameVtVectorWritebackInjects = schedulerMetrics.getAssistInterCoreSameVtVectorWritebackInjects();
		evt.AssistInterCoreDonorVtVectorWritebackInjects = schedulerMetrics.getAssistInterCoreDonorVtVectorWritebackInjects();
		evt.AssistInterCoreLane6DefaultStoreDonorPrefetchInjects = schedulerMetrics.getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects();
		evt.AssistInterCoreLane6HotLoadDonorPrefetchInjects = schedulerMetrics.getAssistInterCoreLane6HotLoadDonorPrefetchInjects();
		evt.AssistInterCoreLane6HotStoreDonorPrefetchInjects = schedulerMetrics.getAssistInterCoreLane6HotStoreDonorPrefetchInjects();
		evt.AssistInterCoreLane6DonorPrefetchInjects = schedulerMetrics.getAssistInterCoreLane6DonorPrefetchInjects();
		evt.AssistInterCoreLane6ColdStoreLdsaInjects = schedulerMetrics.getAssistInterCoreLane6ColdStoreLdsaInjects();
		evt.AssistInterCoreLane6LdsaInjects = schedulerMetrics.getAssistInterCoreLane6LdsaInjects();
		evt.AssistQuotaRejects = schedulerMetrics.getAssistQuotaRejects();
		evt.AssistQuotaIssueRejects = schedulerMetrics.getAssistQuotaIssueRejects();
		evt.AssistQuotaLineRejects = schedulerMetrics.getAssistQuotaLineRejects();
		evt.AssistQuotaLinesReserved = schedulerMetrics.getAssistQuotaLinesReserved();
		evt.AssistBackpressureRejects = schedulerMetrics.getAssistBackpressureRejects();
		evt.AssistBackpressureOuterCapRejects = schedulerMetrics.getAssistBackpressureOuterCapRejects();
		evt.AssistBackpressureMshrRejects = schedulerMetrics.getAssistBackpressureMshrRejects();
		evt.AssistBackpressureDmaSrfRejects = schedulerMetrics.getAssistBackpressureDmaSrfRejects();
		evt.AssistDonorPrefetchInjects = schedulerMetrics.getAssistDonorPrefetchInjects();
		evt.AssistLdsaInjects = schedulerMetrics.getAssistLdsaInjects();
		evt.AssistVdsaInjects = schedulerMetrics.getAssistVdsaInjects();
		evt.AssistSameVtInjects = schedulerMetrics.getAssistSameVtInjects();
		evt.AssistDonorVtInjects = schedulerMetrics.getAssistDonorVtInjects();
		evt.AssistInvalidationReason = schedulerMetrics.getLastAssistInvalidationReason();
		evt.AssistOwnershipSignature = schedulerMetrics.getLastAssistOwnershipSignature();
		RecordFullState(evt);
	}

	const std::vector<FullStateTraceEvent> TraceSink::GetThreadTrace(int threadId)
	{
		if (threadId >= 0 && threadId < 16 && _perThreadTraces.contains(threadId))
		{
			return _perThreadTraces.at(threadId);
		}
		return {};
	}

	int TraceSink::GetTotalTraceCount()
	{
		int total = 0;
		for (const auto &entry : _perThreadTraces)
		{
			total += static_cast<int>(entry.second.size());
		}
		return total;
	}

	void TraceSink::ExportBinaryTrace(const std::wstring &binaryFilePath)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using FileStream fs = new FileStream(binaryFilePath, FileMode.Create);
		{
			auto fs = std::make_shared<FileStream>(binaryFilePath, FileMode::Create);
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using BinaryWriter writer = new BinaryWriter(fs);
			{
				auto writer = std::make_shared<BinaryWriter>(fs);
    
				// Write header
				writer->Write(TraceMagic);
				writer->Write(BinaryTraceVersion);
				writer->Write(BinaryCount(_perThreadTraces.size()));
    
				// Write checkpoints
				writer->Write(BinaryCount(_checkpoints.size()));
				for (auto checkpoint : _checkpoints)
				{
					SerializeCheckpoint(writer, checkpoint);
				}
    
				// Write per-thread traces
				for (auto kvp : _perThreadTraces)
				{
					writer->Write(kvp.first); // Thread ID
					writer->Write(BinaryCount(kvp.second.size()));
					for (FullStateTraceEvent evt : kvp.second)
					{
						SerializeFullStateEvent(writer, evt);
					}
				}
			}
		}
	}

	void TraceSink::SerializeCheckpoint(std::shared_ptr<BinaryWriter> writer, StateCheckpoint checkpoint)
	{
		writer->Write(checkpoint.CycleNumber);
		writer->Write(BinaryCount(checkpoint.ProcessorState.size()));
		if (!checkpoint.ProcessorState.empty())
		{
			writer->Write(checkpoint.ProcessorState);
		}
		writer->Write(BinaryCount(checkpoint.MemorySnapshot.size()));
		if (!checkpoint.MemorySnapshot.empty())
		{
			writer->Write(checkpoint.MemorySnapshot);
		}
		writer->Write(checkpoint.RandomState);
	}

	void TraceSink::SerializeFullStateEvent(std::shared_ptr<BinaryWriter> writer, FullStateTraceEvent evt)
	{
		writer->Write(evt.PC);
		writer->Write(evt.BundleId);
		writer->Write(evt.OpIndex);
		writer->Write(evt.Opcode);
		writer->Write(evt.ThreadId);
		writer->Write(evt.CycleNumber);

		// Register file
		writer->Write(evt.RegisterFile ? BinaryCount(evt.RegisterFile->size()) : 0);
		if (evt.RegisterFile != nullptr)
		{
			for (unsigned long long reg : *evt.RegisterFile)
			{
				writer->Write(reg);
			}
		}

		// Vector registers
		writer->Write(evt.VectorRegisters ? BinaryCount(evt.VectorRegisters->size()) : 0);
		if (evt.VectorRegisters != nullptr)
		{
			for (unsigned long long vreg : *evt.VectorRegisters)
			{
				writer->Write(vreg);
			}
		}

		// Predicate registers
		writer->Write(evt.PredicateRegisters ? BinaryCount(evt.PredicateRegisters->size()) : 0);
		if (evt.PredicateRegisters != nullptr)
		{
			for (unsigned short preg : *evt.PredicateRegisters)
			{
				writer->Write(preg);
			}
		}

		// FP context
		writer->Write(evt.FPContext.RoundingMode);
		writer->Write(evt.FPContext.FlushToZero);
		writer->Write(evt.FPContext.DefaultNaN);

		// Memory writes
		writer->Write(evt.MemoryWrites ? BinaryCount(evt.MemoryWrites->size()) : 0);
		if (evt.MemoryWrites != nullptr)
		{
			for (MemoryDelta delta : *evt.MemoryWrites)
			{
				writer->Write(delta.Address);
				writer->Write(BinaryCount(delta.OldData.size()));
				if (!delta.OldData.empty())
				{
					writer->Write(delta.OldData);
				}
				writer->Write(BinaryCount(delta.NewData.size()));
				if (!delta.NewData.empty())
				{
					writer->Write(delta.NewData);
				}
			}
		}

		// FSP state
		writer->Write(evt.WasStolenSlot);
		writer->Write(evt.OriginalThreadId);

		// Pipeline state
		writer->Write((evt.PipelineStage != L"") ? evt.PipelineStage : L"");
		writer->Write(evt.Stalled);
		writer->Write((evt.StallReason != L"") ? evt.StallReason : L"");
		writer->Write(static_cast<unsigned char>(evt.DecodedBundleStateOwnerKind));
		writer->Write(evt.DecodedBundleStateEpoch);
		writer->Write(evt.DecodedBundleStateVersion);
		writer->Write(static_cast<unsigned char>(evt.DecodedBundleStateKind));
		writer->Write(static_cast<unsigned char>(evt.DecodedBundleStateOrigin));
		writer->Write(evt.DecodedBundlePc);
		writer->Write(evt.DecodedBundleValidMask);
		writer->Write(evt.DecodedBundleNopMask);
		writer->Write(evt.DecodedBundleHasCanonicalDecode);
		writer->Write(evt.DecodedBundleHasCanonicalLegality);
		writer->Write(evt.DecodedBundleHasDecodeFault);

		// Memory subsystem
		writer->Write(evt.BankQueueDepths ? BinaryCount(evt.BankQueueDepths->size()) : 0);
		if (evt.BankQueueDepths != nullptr)
		{
			for (int depth : *evt.BankQueueDepths)
			{
				writer->Write(depth);
			}
		}
		writer->Write(evt.ActiveMemoryRequests);
		writer->Write(evt.MemorySubsystemCycle);

		// Scheduler state
		writer->Write(evt.ThreadReadyQueueDepths ? BinaryCount(evt.ThreadReadyQueueDepths->size()) : 0);
		if (evt.ThreadReadyQueueDepths != nullptr)
		{
			for (int depth : *evt.ThreadReadyQueueDepths)
			{
				writer->Write(depth);
			}
		}
		writer->Write((evt.CurrentFSPPolicy != L"") ? evt.CurrentFSPPolicy : L"");

		// Phase-aware replay state
		writer->Write(evt.ReplayEpochId);
		writer->Write(evt.ReplayPhaseCachedPc);
		writer->Write(evt.ReplayEpochLength);
		writer->Write(evt.ReplayPhaseValidSlotCount);
		writer->Write(evt.StableDonorMask);
		writer->Write(static_cast<unsigned char>(evt.ReplayInvalidationReason));
		writer->Write(evt.PhaseCertificateTemplateReusable);
		writer->Write(evt.PhaseCertificateReadyHits);
		writer->Write(evt.PhaseCertificateReadyMisses);
		writer->Write(evt.EstimatedPhaseCertificateChecksSaved);
		writer->Write(evt.PhaseCertificateInvalidations);
		writer->Write(static_cast<unsigned char>(evt.PhaseCertificateInvalidationReason));
		writer->Write(evt.DeterminismReferenceOpportunitySlots);
		writer->Write(evt.DeterminismReplayEligibleSlots);
		writer->Write(evt.DeterminismMaskedSlots);
		writer->Write(evt.DeterminismEstimatedLostSlots);
		writer->Write(evt.DeterminismConstrainedCycles);
		writer->Write(evt.DomainIsolationProbeAttempts);
		writer->Write(evt.DomainIsolationBlockedAttempts);
		writer->Write(evt.DomainIsolationCrossDomainBlocks);
		writer->Write(evt.DomainIsolationKernelToUserBlocks);
		writer->Write(evt.EligibilityMaskedCycles);
		writer->Write(evt.EligibilityMaskedReadyCandidates);
		writer->Write(evt.LastEligibilityRequestedMask);
		writer->Write(evt.LastEligibilityNormalizedMask);
		writer->Write(evt.LastEligibilityReadyPortMask);
		writer->Write(evt.LastEligibilityVisibleReadyMask);
		writer->Write(evt.LastEligibilityMaskedReadyMask);
		writer->Write(evt.AssistNominations);
		writer->Write(evt.AssistInjections);
		writer->Write(evt.AssistRejects);
		writer->Write(evt.AssistBoundaryRejects);
		writer->Write(evt.AssistInvalidations);
		writer->Write(evt.AssistInterCoreNominations);
		writer->Write(evt.AssistInterCoreInjections);
		writer->Write(evt.AssistInterCoreRejects);
		writer->Write(evt.AssistInterCoreDomainRejects);
		writer->Write(evt.AssistInterCorePodLocalInjections);
		writer->Write(evt.AssistInterCoreCrossPodInjections);
		writer->Write(evt.AssistInterCorePodLocalRejects);
		writer->Write(evt.AssistInterCoreCrossPodRejects);
		writer->Write(evt.AssistInterCorePodLocalDomainRejects);
		writer->Write(evt.AssistInterCoreCrossPodDomainRejects);
		writer->Write(evt.AssistInterCoreSameVtVectorInjects);
		writer->Write(evt.AssistInterCoreDonorVtVectorInjects);
		writer->Write(evt.AssistInterCoreSameVtVectorWritebackInjects);
		writer->Write(evt.AssistInterCoreDonorVtVectorWritebackInjects);
		writer->Write(evt.AssistInterCoreLane6DefaultStoreDonorPrefetchInjects);
		writer->Write(evt.AssistInterCoreLane6HotLoadDonorPrefetchInjects);
		writer->Write(evt.AssistInterCoreLane6HotStoreDonorPrefetchInjects);
		writer->Write(evt.AssistInterCoreLane6DonorPrefetchInjects);
		writer->Write(evt.AssistInterCoreLane6ColdStoreLdsaInjects);
		writer->Write(evt.AssistInterCoreLane6LdsaInjects);
		writer->Write(evt.AssistQuotaRejects);
		writer->Write(evt.AssistQuotaIssueRejects);
		writer->Write(evt.AssistQuotaLineRejects);
		writer->Write(evt.AssistQuotaLinesReserved);
		writer->Write(evt.AssistBackpressureRejects);
		writer->Write(evt.AssistBackpressureOuterCapRejects);
		writer->Write(evt.AssistBackpressureMshrRejects);
		writer->Write(evt.AssistBackpressureDmaSrfRejects);
		writer->Write(evt.AssistDonorPrefetchInjects);
		writer->Write(evt.AssistLdsaInjects);
		writer->Write(evt.AssistVdsaInjects);
		writer->Write(evt.AssistSameVtInjects);
		writer->Write(evt.AssistDonorVtInjects);
		writer->Write(static_cast<unsigned char>(evt.AssistInvalidationReason));
		writer->Write(evt.AssistOwnershipSignature);
	}

	void TraceSink::ClearAllTraces()
	{
		events.clear();
		for (auto &entry : _perThreadTraces)
		{
			entry.second.clear();
		}
		_checkpoints.clear();
		_currentCycle = 0;
	}

	void TraceSink::Flush()
	{
		if (events.empty())
		{
			return;
		}

		switch (format)
		{
			case TraceFormat::CSV:
				FlushCSV();
				break;
			case TraceFormat::JSON:
				FlushJSON();
				break;
		}

		events.clear();
	}

	void TraceSink::FlushCSV()
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using StreamWriter writer = new StreamWriter(filePath, append: true);
		{
			const std::filesystem::path outputPath(filePath);
			std::error_code fileError;
			const bool fileExists = std::filesystem::exists(outputPath, fileError) && !fileError;
			const auto fileLength = fileExists ? std::filesystem::file_size(outputPath, fileError) : 0;
			const bool writeHeader = !fileExists || fileError || fileLength == 0;
			std::wofstream writer(outputPath, std::ios::out | std::ios::app);
			if (!writer.is_open()) throw std::runtime_error("Unable to open trace file for append.");

			// Write header if file is empty
			if (writeHeader)
			{
				if (level == TraceLevel::Summary)
				{
					writer << L"PC,BundleId,OpIndex,Opcode,ExceptionFlags,ExceptionCount" << L'\n';
				}
				else
				{
					writer << L"PC,BundleId,OpIndex,Opcode,Operands,PredicateMask,ExceptionFlags,Result,ExceptionCount" << L'\n';
				}
			}
        
			for (auto evt : events)
			{
				if (level == TraceLevel::Summary)
				{
					writer << std::format(L"{},{},{},{},{},{}", evt.PC, evt.BundleId, evt.OpIndex, evt.Opcode, evt.Flags.ToString(), evt.ExceptionCount) << L'\n';
				}
				else
				{
					const std::wstring operands = EscapeCsv(FormatOperands(evt.Operands));
					const std::wstring result = EscapeCsv(FormatAny(evt.Result));
					writer << std::format(
						L"{},{},{},{},\"{}\",0x{:016X},{},\"{}\",{}",
						evt.PC, evt.BundleId, evt.OpIndex, evt.Opcode, operands, evt.PredicateMask,
						evt.Flags.ToString(), result, evt.ExceptionCount) << L'\n';
				}
			}
		}
	}

	void TraceSink::FlushJSON()
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using StreamWriter writer = new StreamWriter(filePath, append: true);
		{
			const std::filesystem::path outputPath(filePath);
			std::wofstream writer(outputPath, std::ios::out | std::ios::app);
			if (!writer.is_open()) throw std::runtime_error("Unable to open trace file for append.");

			for (auto evt : events)
			{
				std::wstring json = std::format(
					L"{{\n  \"PC\": {},\n  \"BundleId\": {},\n  \"OpIndex\": {},\n  \"Opcode\": {},\n  \"ExceptionFlags\": {},\n  \"ExceptionCount\": {}",
					evt.PC, evt.BundleId, evt.OpIndex, JsonQuote(std::to_wstring(evt.Opcode)),
					JsonQuote(evt.Flags.ToString()), evt.ExceptionCount);

				if (level == TraceLevel::Full)
				{
					json += std::format(
						L",\n  \"Operands\": {},\n  \"PredicateMask\": {},\n  \"Result\": {}",
						FormatOperandsJson(evt.Operands),
						JsonQuote(std::format(L"0x{:016X}", evt.PredicateMask)),
						JsonQuote(FormatAny(evt.Result)));
				}
				json += L"\n}";
				writer << json << L'\n';
			}
		}
	}

	const std::vector<TraceEvent> TraceSink::GetEvents()
	{
		return events;
	}

	void TraceSink::RecordV4Event(V4TraceEvent evt)
	{
		if (!enabled)
		{
			return;
		}

		_v4Events.push_back(evt);
	}

	const std::vector<V4TraceEvent> TraceSink::GetV4Events()
	{
		return _v4Events;
	}

	const std::vector<V4TraceEvent> TraceSink::GetV4Events(TraceEventKind kind)
	{
		std::vector<V4TraceEvent> result;
		for (auto evt : _v4Events)
		{
			if (evt.getKind() == kind)
			{
				result.push_back(evt);
			}
		}
		return result;
	}

	const std::vector<V4TraceEvent> TraceSink::GetV4EventsForVt(unsigned char vtId)
	{
		std::vector<V4TraceEvent> result;
		for (auto evt : _v4Events)
		{
			if (evt.getVtId() == vtId)
			{
				result.push_back(evt);
			}
		}
		return result;
	}

	void TraceSink::ClearV4Events()
	{
		_v4Events.clear();
	}

	int TraceSink::getV4EventCount() const
	{
		return _v4Events.size();
	}
}
