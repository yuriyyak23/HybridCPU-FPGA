#include "YAKSys_Hybrid_CPU.PerformanceReport.h"
#include "../../../tangible_exception_helper.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace HybridCPU_ISE::Core;

namespace YAKSys_Hybrid_CPU
{

	std::vector<std::shared_ptr<PerformanceReport::BurstTrace>> PerformanceReport::getBurstTraces() const
	{
		return BurstTraces;
	}

	void PerformanceReport::setBurstTraces(const std::vector<std::shared_ptr<BurstTrace>> &value)
	{
		BurstTraces = value;
	}

	long long PerformanceReport::BurstTrace::getTimestamp() const
	{
		return Timestamp;
	}

	void PerformanceReport::BurstTrace::setTimestamp(long long value)
	{
		Timestamp = value;
	}

	unsigned long long PerformanceReport::BurstTrace::getAddress() const
	{
		return Address;
	}

	void PerformanceReport::BurstTrace::setAddress(unsigned long long value)
	{
		Address = value;
	}

	int PerformanceReport::BurstTrace::getLength() const
	{
		return Length;
	}

	void PerformanceReport::BurstTrace::setLength(int value)
	{
		Length = value;
	}

	bool PerformanceReport::BurstTrace::isRead() const
	{
		return IsRead;
	}

	void PerformanceReport::BurstTrace::setRead(bool value)
	{
		IsRead = value;
	}

	int PerformanceReport::BurstTrace::getBankId() const
	{
		return BankId;
	}

	void PerformanceReport::BurstTrace::setBankId(int value)
	{
		BankId = value;
	}

	long long PerformanceReport::BurstTrace::getDuration() const
	{
		return Duration;
	}

	void PerformanceReport::BurstTrace::setDuration(long long value)
	{
		Duration = value;
	}

	bool PerformanceReport::getHasBurstTraces() const
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return !getBurstTraces().empty();
	}

	void PerformanceReport::AppendBurstTraceSummary(std::shared_ptr<StringBuilder> sb)
	{
		if (!getHasBurstTraces())
		{
			return;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: sb.AppendLine(string.Format("Burst Traces: {0} traces collected", BurstTraces!.Count));
		sb->appendLine(std::format("Burst Traces: {0} traces collected", getBurstTraces().size()));
		sb->appendLine();
	}

	std::wstring PerformanceReport::ExportBurstTracesToCSV()
	{
		if (!getHasBurstTraces())
		{
			return L"No burst traces available";
		}

		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Timestamp,Address,Length,IsRead,BankId,Duration");

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: foreach (BurstTrace trace in BurstTraces!)
		for (auto trace : getBurstTraces())
		{
			sb->appendLine(std::format("{0},{1},{2},{3},{4},{5}", trace->getTimestamp(), trace->getAddress(), trace->getLength(), (trace->isRead() ? "True" : "False"), trace->getBankId(), trace->getDuration()));
		}

		return sb->toString();
	}

	std::shared_ptr<PerformanceReport> PerformanceReport::CreateAdditiveDiagnosticIntervalSince(std::shared_ptr<PerformanceReport> baseline)
	{
		ArgumentNullException::ThrowIfNull(baseline);

		std::shared_ptr<PerformanceReport> interval = CreateMemoryCycleTelemetryIntervalSince(baseline);

		// Legacy MemorySubsystem compatibility counters are cumulative
		// between Processor.ResetPerformanceCounters calls.
		interval->setTotalBursts(SubtractMonotonicOrThrow(getTotalBursts(), baseline->getTotalBursts(), L"TotalBursts"));
		interval->setTotalBytesTransferred(SubtractMonotonicOrThrow(getTotalBytesTransferred(), baseline->getTotalBytesTransferred(), L"TotalBytesTransferred"));

		// The pod scheduler survives CPU_Core.PrepareExecutionStart and
		// owns cumulative phase/typed-slot telemetry.
		interval->setNopAvoided(SubtractMonotonicOrThrow(getNopAvoided(), baseline->getNopAvoided(), L"NopAvoided"));
		interval->setNopDueToNoClassCapacity(SubtractMonotonicOrThrow(getNopDueToNoClassCapacity(), baseline->getNopDueToNoClassCapacity(), L"NopDueToNoClassCapacity"));
		interval->setNopDueToPinnedConstraint(SubtractMonotonicOrThrow(getNopDueToPinnedConstraint(), baseline->getNopDueToPinnedConstraint(), L"NopDueToPinnedConstraint"));
		interval->setNopDueToResourceConflict(SubtractMonotonicOrThrow(getNopDueToResourceConflict(), baseline->getNopDueToResourceConflict(), L"NopDueToResourceConflict"));
		interval->setNopDueToDynamicState(SubtractMonotonicOrThrow(getNopDueToDynamicState(), baseline->getNopDueToDynamicState(), L"NopDueToDynamicState"));
		interval->setClassFlexibleInjects(SubtractMonotonicOrThrow(getClassFlexibleInjects(), baseline->getClassFlexibleInjects(), L"ClassFlexibleInjects"));
		interval->setHardPinnedInjects(SubtractMonotonicOrThrow(getHardPinnedInjects(), baseline->getHardPinnedInjects(), L"HardPinnedInjects"));
		interval->setEligibilityMaskedCycles(SubtractMonotonicOrThrow(getEligibilityMaskedCycles(), baseline->getEligibilityMaskedCycles(), L"EligibilityMaskedCycles"));
		interval->setEligibilityMaskedReadyCandidates(SubtractMonotonicOrThrow(getEligibilityMaskedReadyCandidates(), baseline->getEligibilityMaskedReadyCandidates(), L"EligibilityMaskedReadyCandidates"));
		interval->setPhaseCertificateReadyHits(SubtractMonotonicOrThrow(getPhaseCertificateReadyHits(), baseline->getPhaseCertificateReadyHits(), L"PhaseCertificateReadyHits"));
		interval->setPhaseCertificateReadyMisses(SubtractMonotonicOrThrow(getPhaseCertificateReadyMisses(), baseline->getPhaseCertificateReadyMisses(), L"PhaseCertificateReadyMisses"));
		interval->setEstimatedPhaseCertificateChecksSaved(SubtractMonotonicOrThrow(getEstimatedPhaseCertificateChecksSaved(), baseline->getEstimatedPhaseCertificateChecksSaved(), L"EstimatedPhaseCertificateChecksSaved"));
		interval->setPhaseCertificateInvalidations(SubtractMonotonicOrThrow(getPhaseCertificateInvalidations(), baseline->getPhaseCertificateInvalidations(), L"PhaseCertificateInvalidations"));
		interval->setPhaseCertificateMutationInvalidations(SubtractMonotonicOrThrow(getPhaseCertificateMutationInvalidations(), baseline->getPhaseCertificateMutationInvalidations(), L"PhaseCertificateMutationInvalidations"));
		interval->setPhaseCertificatePhaseMismatchInvalidations(SubtractMonotonicOrThrow(getPhaseCertificatePhaseMismatchInvalidations(), baseline->getPhaseCertificatePhaseMismatchInvalidations(), L"PhaseCertificatePhaseMismatchInvalidations"));
		interval->setSmtOwnerContextGuardRejects(SubtractMonotonicOrThrow(getSmtOwnerContextGuardRejects(), baseline->getSmtOwnerContextGuardRejects(), L"SmtOwnerContextGuardRejects"));
		interval->setSmtDomainGuardRejects(SubtractMonotonicOrThrow(getSmtDomainGuardRejects(), baseline->getSmtDomainGuardRejects(), L"SmtDomainGuardRejects"));
		interval->setSmtBoundaryGuardRejects(SubtractMonotonicOrThrow(getSmtBoundaryGuardRejects(), baseline->getSmtBoundaryGuardRejects(), L"SmtBoundaryGuardRejects"));
		interval->setSmtSharedResourceCertificateRejects(SubtractMonotonicOrThrow(getSmtSharedResourceCertificateRejects(), baseline->getSmtSharedResourceCertificateRejects(), L"SmtSharedResourceCertificateRejects"));
		interval->setSmtRegisterGroupCertificateRejects(SubtractMonotonicOrThrow(getSmtRegisterGroupCertificateRejects(), baseline->getSmtRegisterGroupCertificateRejects(), L"SmtRegisterGroupCertificateRejects"));
		interval->setSmtLegalityRejectByAluClass(SubtractMonotonicOrThrow(getSmtLegalityRejectByAluClass(), baseline->getSmtLegalityRejectByAluClass(), L"SmtLegalityRejectByAluClass"));
		interval->setSmtLegalityRejectByLsuClass(SubtractMonotonicOrThrow(getSmtLegalityRejectByLsuClass(), baseline->getSmtLegalityRejectByLsuClass(), L"SmtLegalityRejectByLsuClass"));
		interval->setSmtLegalityRejectByDmaStreamClass(SubtractMonotonicOrThrow(getSmtLegalityRejectByDmaStreamClass(), baseline->getSmtLegalityRejectByDmaStreamClass(), L"SmtLegalityRejectByDmaStreamClass"));
		interval->setSmtLegalityRejectByBranchControl(SubtractMonotonicOrThrow(getSmtLegalityRejectByBranchControl(), baseline->getSmtLegalityRejectByBranchControl(), L"SmtLegalityRejectByBranchControl"));
		interval->setSmtLegalityRejectBySystemSingleton(SubtractMonotonicOrThrow(getSmtLegalityRejectBySystemSingleton(), baseline->getSmtLegalityRejectBySystemSingleton(), L"SmtLegalityRejectBySystemSingleton"));

		// StreamRegisterFile statistics and warm-ingress telemetry are
		// cumulative for the lifetime of the shared memory subsystem.
		interval->setL1BypassHits(SubtractMonotonicOrThrow(getL1BypassHits(), baseline->getL1BypassHits(), L"L1BypassHits"));
		interval->setForegroundWarmAttempts(SubtractMonotonicOrThrow(getForegroundWarmAttempts(), baseline->getForegroundWarmAttempts(), L"ForegroundWarmAttempts"));
		interval->setForegroundWarmSuccesses(SubtractMonotonicOrThrow(getForegroundWarmSuccesses(), baseline->getForegroundWarmSuccesses(), L"ForegroundWarmSuccesses"));
		interval->setForegroundWarmReuseHits(SubtractMonotonicOrThrow(getForegroundWarmReuseHits(), baseline->getForegroundWarmReuseHits(), L"ForegroundWarmReuseHits"));
		interval->setForegroundBypassHits(SubtractMonotonicOrThrow(getForegroundBypassHits(), baseline->getForegroundBypassHits(), L"ForegroundBypassHits"));
		interval->setAssistWarmAttempts(SubtractMonotonicOrThrow(getAssistWarmAttempts(), baseline->getAssistWarmAttempts(), L"AssistWarmAttempts"));
		interval->setAssistWarmSuccesses(SubtractMonotonicOrThrow(getAssistWarmSuccesses(), baseline->getAssistWarmSuccesses(), L"AssistWarmSuccesses"));
		interval->setAssistWarmReuseHits(SubtractMonotonicOrThrow(getAssistWarmReuseHits(), baseline->getAssistWarmReuseHits(), L"AssistWarmReuseHits"));
		interval->setAssistBypassHits(SubtractMonotonicOrThrow(getAssistBypassHits(), baseline->getAssistBypassHits(), L"AssistBypassHits"));
		interval->setStreamWarmTranslationRejects(SubtractMonotonicOrThrow(getStreamWarmTranslationRejects(), baseline->getStreamWarmTranslationRejects(), L"StreamWarmTranslationRejects"));
		interval->setStreamWarmBackendRejects(SubtractMonotonicOrThrow(getStreamWarmBackendRejects(), baseline->getStreamWarmBackendRejects(), L"StreamWarmBackendRejects"));
		interval->setAssistWarmResidentBudgetRejects(SubtractMonotonicOrThrow(getAssistWarmResidentBudgetRejects(), baseline->getAssistWarmResidentBudgetRejects(), L"AssistWarmResidentBudgetRejects"));
		interval->setAssistWarmLoadingBudgetRejects(SubtractMonotonicOrThrow(getAssistWarmLoadingBudgetRejects(), baseline->getAssistWarmLoadingBudgetRejects(), L"AssistWarmLoadingBudgetRejects"));
		interval->setAssistWarmNoVictimRejects(SubtractMonotonicOrThrow(getAssistWarmNoVictimRejects(), baseline->getAssistWarmNoVictimRejects(), L"AssistWarmNoVictimRejects"));

		return interval;
	}

	long long PerformanceReport::SubtractMonotonicOrThrow(long long current, long long baseline, const std::wstring &metricName)
	{
		if (current < baseline)
		{
			throw InvalidOperationException(metricName + L" cumulative diagnostic metric regressed from " + std::to_wstring(baseline) + L" to " + std::to_wstring(current) + L"; an exact interval cannot be reported because this compatibility field has no availability carrier.");
		}

		return current - baseline;
	}

	long long PerformanceReport::getTotalBursts() const
	{
		return TotalBursts;
	}

	void PerformanceReport::setTotalBursts(long long value)
	{
		TotalBursts = value;
	}

	long long PerformanceReport::getTotalBytesTransferred() const
	{
		return TotalBytesTransferred;
	}

	void PerformanceReport::setTotalBytesTransferred(long long value)
	{
		TotalBytesTransferred = value;
	}

	long long PerformanceReport::getBankConflicts() const
	{
		return BankConflicts;
	}

	void PerformanceReport::setBankConflicts(long long value)
	{
		BankConflicts = value;
	}

	long long PerformanceReport::getStallCycles() const
	{
		return StallCycles;
	}

	void PerformanceReport::setStallCycles(long long value)
	{
		StallCycles = value;
	}

	long long PerformanceReport::getDmaTransfers() const
	{
		return DmaTransfers;
	}

	void PerformanceReport::setDmaTransfers(long long value)
	{
		DmaTransfers = value;
	}

	double PerformanceReport::getAverageBurstLength() const
	{
		return AverageBurstLength;
	}

	void PerformanceReport::setAverageBurstLength(double value)
	{
		AverageBurstLength = value;
	}

	long long PerformanceReport::getTotalWaitCycles() const
	{
		return TotalWaitCycles;
	}

	void PerformanceReport::setTotalWaitCycles(long long value)
	{
		TotalWaitCycles = value;
	}

	double PerformanceReport::getAverageWaitCycles() const
	{
		return AverageWaitCycles;
	}

	void PerformanceReport::setAverageWaitCycles(double value)
	{
		AverageWaitCycles = value;
	}

	int PerformanceReport::getMaxQueueDepth() const
	{
		return MaxQueueDepth;
	}

	void PerformanceReport::setMaxQueueDepth(int value)
	{
		MaxQueueDepth = value;
	}

	int PerformanceReport::getCurrentQueuedRequests() const
	{
		return CurrentQueuedRequests;
	}

	void PerformanceReport::setCurrentQueuedRequests(int value)
	{
		CurrentQueuedRequests = value;
	}

	long long PerformanceReport::getTotalMemoryStalls() const
	{
		return TotalMemoryStalls;
	}

	void PerformanceReport::setTotalMemoryStalls(long long value)
	{
		TotalMemoryStalls = value;
	}

	long long PerformanceReport::getMemoryQueueFullEvents() const
	{
		return MemoryQueueFullEvents;
	}

	void PerformanceReport::setMemoryQueueFullEvents(long long value)
	{
		MemoryQueueFullEvents = value;
	}

	long long PerformanceReport::getBankSaturationCycles() const
	{
		return BankSaturationCycles;
	}

	void PerformanceReport::setBankSaturationCycles(long long value)
	{
		BankSaturationCycles = value;
	}

	long long PerformanceReport::getIdleMemoryCycles() const
	{
		return IdleMemoryCycles;
	}

	void PerformanceReport::setIdleMemoryCycles(long long value)
	{
		IdleMemoryCycles = value;
	}

	double PerformanceReport::getMemoryUtilization() const
	{
		return MemoryUtilization;
	}

	void PerformanceReport::setMemoryUtilization(double value)
	{
		MemoryUtilization = value;
	}

	double PerformanceReport::getAverageMemoryQueueDepth() const
	{
		return AverageMemoryQueueDepth;
	}

	void PerformanceReport::setAverageMemoryQueueDepth(double value)
	{
		AverageMemoryQueueDepth = value;
	}

	long long PerformanceReport::getQueueOverflowEvents() const
	{
		return QueueOverflowEvents;
	}

	void PerformanceReport::setQueueOverflowEvents(long long value)
	{
		QueueOverflowEvents = value;
	}

	double PerformanceReport::getAverageMemoryLatency() const
	{
		return AverageMemoryLatency;
	}

	void PerformanceReport::setAverageMemoryLatency(double value)
	{
		AverageMemoryLatency = value;
	}

	std::wstring PerformanceReport::getMemoryCycleTelemetrySchemaVersion() const
	{
		return MemoryCycleTelemetrySchemaVersion;
	}

	void PerformanceReport::setMemoryCycleTelemetrySchemaVersion(const std::wstring &value)
	{
		MemoryCycleTelemetrySchemaVersion = value;
	}

	bool PerformanceReport::getMemoryCycleTelemetryAvailable() const
	{
		return MemoryCycleTelemetryAvailable;
	}

	void PerformanceReport::setMemoryCycleTelemetryAvailable(bool value)
	{
		MemoryCycleTelemetryAvailable = value;
	}

	long long PerformanceReport::getMemoryControllerCycles() const
	{
		return MemoryControllerCycles;
	}

	void PerformanceReport::setMemoryControllerCycles(long long value)
	{
		MemoryControllerCycles = value;
	}

	long long PerformanceReport::getMemoryReadServiceCycles() const
	{
		return MemoryReadServiceCycles;
	}

	void PerformanceReport::setMemoryReadServiceCycles(long long value)
	{
		MemoryReadServiceCycles = value;
	}

	long long PerformanceReport::getMemoryStoreReadinessServiceCycles() const
	{
		return MemoryStoreReadinessServiceCycles;
	}

	void PerformanceReport::setMemoryStoreReadinessServiceCycles(long long value)
	{
		MemoryStoreReadinessServiceCycles = value;
	}

	long long PerformanceReport::getMemoryCompletionPublicationCycles() const
	{
		return MemoryCompletionPublicationCycles;
	}

	void PerformanceReport::setMemoryCompletionPublicationCycles(long long value)
	{
		MemoryCompletionPublicationCycles = value;
	}

	long long PerformanceReport::getMemoryAcceptedRequests() const
	{
		return MemoryAcceptedRequests;
	}

	void PerformanceReport::setMemoryAcceptedRequests(long long value)
	{
		MemoryAcceptedRequests = value;
	}

	long long PerformanceReport::getMemoryCompletedRequests() const
	{
		return MemoryCompletedRequests;
	}

	void PerformanceReport::setMemoryCompletedRequests(long long value)
	{
		MemoryCompletedRequests = value;
	}

	long long PerformanceReport::getDataReadAcceptedRequests() const
	{
		return DataReadAcceptedRequests;
	}

	void PerformanceReport::setDataReadAcceptedRequests(long long value)
	{
		DataReadAcceptedRequests = value;
	}

	long long PerformanceReport::getDataReadCompletedRequests() const
	{
		return DataReadCompletedRequests;
	}

	void PerformanceReport::setDataReadCompletedRequests(long long value)
	{
		DataReadCompletedRequests = value;
	}

	long long PerformanceReport::getDataWriteAcceptedRequests() const
	{
		return DataWriteAcceptedRequests;
	}

	void PerformanceReport::setDataWriteAcceptedRequests(long long value)
	{
		DataWriteAcceptedRequests = value;
	}

	long long PerformanceReport::getDataWriteCompletedRequests() const
	{
		return DataWriteCompletedRequests;
	}

	void PerformanceReport::setDataWriteCompletedRequests(long long value)
	{
		DataWriteCompletedRequests = value;
	}

	long long PerformanceReport::getDataReadBytes() const
	{
		return DataReadBytes;
	}

	void PerformanceReport::setDataReadBytes(long long value)
	{
		DataReadBytes = value;
	}

	long long PerformanceReport::getCommittedDataWriteBytes() const
	{
		return CommittedDataWriteBytes;
	}

	void PerformanceReport::setCommittedDataWriteBytes(long long value)
	{
		CommittedDataWriteBytes = value;
	}

	bool PerformanceReport::getInstructionFetchReadBytesTelemetryAvailable() const
	{
		return InstructionFetchReadBytesTelemetryAvailable;
	}

	void PerformanceReport::setInstructionFetchReadBytesTelemetryAvailable(bool value)
	{
		InstructionFetchReadBytesTelemetryAvailable = value;
	}

	long long PerformanceReport::getInstructionFetchReadBytes() const
	{
		return InstructionFetchReadBytes;
	}

	void PerformanceReport::setInstructionFetchReadBytes(long long value)
	{
		InstructionFetchReadBytes = value;
	}

	bool PerformanceReport::getInstructionFetchRequestTelemetryAvailable() const
	{
		return InstructionFetchRequestTelemetryAvailable;
	}

	void PerformanceReport::setInstructionFetchRequestTelemetryAvailable(bool value)
	{
		InstructionFetchRequestTelemetryAvailable = value;
	}

	long long PerformanceReport::getMemoryQueueFullRejects() const
	{
		return MemoryQueueFullRejects;
	}

	void PerformanceReport::setMemoryQueueFullRejects(long long value)
	{
		MemoryQueueFullRejects = value;
	}

	bool PerformanceReport::getMemoryBankConflictRejectTelemetryAvailable() const
	{
		return MemoryBankConflictRejectTelemetryAvailable;
	}

	void PerformanceReport::setMemoryBankConflictRejectTelemetryAvailable(bool value)
	{
		MemoryBankConflictRejectTelemetryAvailable = value;
	}

	long long PerformanceReport::getMemoryBankConflictRejects() const
	{
		return MemoryBankConflictRejects;
	}

	void PerformanceReport::setMemoryBankConflictRejects(long long value)
	{
		MemoryBankConflictRejects = value;
	}

	long long PerformanceReport::getMemoryTelemetryBaselineOutstandingRequests() const
	{
		return MemoryTelemetryBaselineOutstandingRequests;
	}

	void PerformanceReport::setMemoryTelemetryBaselineOutstandingRequests(long long value)
	{
		MemoryTelemetryBaselineOutstandingRequests = value;
	}

	long long PerformanceReport::getMemoryCanceledRequests() const
	{
		return MemoryCanceledRequests;
	}

	void PerformanceReport::setMemoryCanceledRequests(long long value)
	{
		MemoryCanceledRequests = value;
	}

	long long PerformanceReport::getMemoryConsumedCompletions() const
	{
		return MemoryConsumedCompletions;
	}

	void PerformanceReport::setMemoryConsumedCompletions(long long value)
	{
		MemoryConsumedCompletions = value;
	}

	long long PerformanceReport::getMemoryOutstandingRequests() const
	{
		return MemoryOutstandingRequests;
	}

	void PerformanceReport::setMemoryOutstandingRequests(long long value)
	{
		MemoryOutstandingRequests = value;
	}

	std::shared_ptr<PerformanceReport> PerformanceReport::CreateMemoryCycleTelemetryIntervalSince(std::shared_ptr<PerformanceReport> baseline)
	{
		ArgumentNullException::ThrowIfNull(baseline);

		std::shared_ptr<PerformanceReport> interval = std::make_shared<PerformanceReport>(*this);
		long long controllerCycles = 0;
		long long readServiceCycles = 0;
		long long storeReadinessCycles = 0;
		long long publicationCycles = 0;
		long long acceptedRequests = 0;
		long long completedRequests = 0;
		long long dataReadAccepted = 0;
		long long dataReadCompleted = 0;
		long long dataWriteAccepted = 0;
		long long dataWriteCompleted = 0;
		long long dataReadBytes = 0;
		long long committedWriteBytes = 0;
		long long queueFullRejects = 0;
		long long canceledRequests = 0;
		long long consumedCompletions = 0;
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		bool compatible = getMemoryCycleTelemetryAvailable() && baseline->getMemoryCycleTelemetryAvailable() && (getMemoryCycleTelemetrySchemaVersion() == baseline->getMemoryCycleTelemetrySchemaVersion()) && getMemoryTelemetryBaselineOutstandingRequests() == baseline->getMemoryTelemetryBaselineOutstandingRequests() && TrySubtract(getMemoryControllerCycles(), baseline->getMemoryControllerCycles(), controllerCycles) && TrySubtract(getMemoryReadServiceCycles(), baseline->getMemoryReadServiceCycles(), readServiceCycles) && TrySubtract(getMemoryStoreReadinessServiceCycles(), baseline->getMemoryStoreReadinessServiceCycles(), storeReadinessCycles) && TrySubtract(getMemoryCompletionPublicationCycles(), baseline->getMemoryCompletionPublicationCycles(), publicationCycles) && TrySubtract(getMemoryAcceptedRequests(), baseline->getMemoryAcceptedRequests(), acceptedRequests) && TrySubtract(getMemoryCompletedRequests(), baseline->getMemoryCompletedRequests(), completedRequests) && TrySubtract(getDataReadAcceptedRequests(), baseline->getDataReadAcceptedRequests(), dataReadAccepted) && TrySubtract(getDataReadCompletedRequests(), baseline->getDataReadCompletedRequests(), dataReadCompleted) && TrySubtract(getDataWriteAcceptedRequests(), baseline->getDataWriteAcceptedRequests(), dataWriteAccepted) && TrySubtract(getDataWriteCompletedRequests(), baseline->getDataWriteCompletedRequests(), dataWriteCompleted) && TrySubtract(getDataReadBytes(), baseline->getDataReadBytes(), dataReadBytes) && TrySubtract(getCommittedDataWriteBytes(), baseline->getCommittedDataWriteBytes(), committedWriteBytes) && TrySubtract(getMemoryQueueFullRejects(), baseline->getMemoryQueueFullRejects(), queueFullRejects) && TrySubtract(getMemoryCanceledRequests(), baseline->getMemoryCanceledRequests(), canceledRequests) && TrySubtract(getMemoryConsumedCompletions(), baseline->getMemoryConsumedCompletions(), consumedCompletions);

		interval->setMemoryCycleTelemetryAvailable(compatible);
		interval->setMemoryControllerCycles(compatible ? controllerCycles : 0);
		interval->setMemoryReadServiceCycles(compatible ? readServiceCycles : 0);
		interval->setMemoryStoreReadinessServiceCycles(compatible ? storeReadinessCycles : 0);
		interval->setMemoryCompletionPublicationCycles(compatible ? publicationCycles : 0);
		interval->setMemoryAcceptedRequests(compatible ? acceptedRequests : 0);
		interval->setMemoryCompletedRequests(compatible ? completedRequests : 0);
		interval->setDataReadAcceptedRequests(compatible ? dataReadAccepted : 0);
		interval->setDataReadCompletedRequests(compatible ? dataReadCompleted : 0);
		interval->setDataWriteAcceptedRequests(compatible ? dataWriteAccepted : 0);
		interval->setDataWriteCompletedRequests(compatible ? dataWriteCompleted : 0);
		interval->setDataReadBytes(compatible ? dataReadBytes : 0);
		interval->setCommittedDataWriteBytes(compatible ? committedWriteBytes : 0);
		interval->setMemoryQueueFullRejects(compatible ? queueFullRejects : 0);
		interval->setMemoryTelemetryBaselineOutstandingRequests(compatible ? getMemoryTelemetryBaselineOutstandingRequests() : 0);
		interval->setMemoryCanceledRequests(compatible ? canceledRequests : 0);
		interval->setMemoryConsumedCompletions(compatible ? consumedCompletions : 0);
		interval->setMemoryOutstandingRequests(compatible ? getMemoryOutstandingRequests() : 0);

		long long instructionFetchReadBytes = 0;
		bool fetchBytesCompatible = compatible && getInstructionFetchReadBytesTelemetryAvailable() && baseline->getInstructionFetchReadBytesTelemetryAvailable() && TrySubtract(getInstructionFetchReadBytes(), baseline->getInstructionFetchReadBytes(), instructionFetchReadBytes);
		interval->setInstructionFetchReadBytesTelemetryAvailable(fetchBytesCompatible);
		interval->setInstructionFetchReadBytes(fetchBytesCompatible ? instructionFetchReadBytes : 0);

		long long bankConflictRejects = 0;
		bool bankRejectsCompatible = compatible && getMemoryBankConflictRejectTelemetryAvailable() && baseline->getMemoryBankConflictRejectTelemetryAvailable() && TrySubtract(getMemoryBankConflictRejects(), baseline->getMemoryBankConflictRejects(), bankConflictRejects);
		interval->setMemoryBankConflictRejectTelemetryAvailable(bankRejectsCompatible);
		interval->setMemoryBankConflictRejects(bankRejectsCompatible ? bankConflictRejects : 0);

		return interval;
	}

	bool PerformanceReport::TrySubtract(long long current, long long baseline, long long &difference)
	{
		if (current < baseline)
		{
			difference = 0;
			return false;
		}

		difference = current - baseline;
		return true;
	}

	long long PerformanceReport::getLoadStalls() const
	{
		return LoadStalls;
	}

	void PerformanceReport::setLoadStalls(long long value)
	{
		LoadStalls = value;
	}

	long long PerformanceReport::getStoreStalls() const
	{
		return StoreStalls;
	}

	void PerformanceReport::setStoreStalls(long long value)
	{
		StoreStalls = value;
	}

	long long PerformanceReport::getBurstStalls() const
	{
		return BurstStalls;
	}

	void PerformanceReport::setBurstStalls(long long value)
	{
		BurstStalls = value;
	}

	long long PerformanceReport::getUnalignedAccessCount() const
	{
		return UnalignedAccessCount;
	}

	void PerformanceReport::setUnalignedAccessCount(long long value)
	{
		UnalignedAccessCount = value;
	}

	long long PerformanceReport::getTotalAlignmentPenalty() const
	{
		return TotalAlignmentPenalty;
	}

	void PerformanceReport::setTotalAlignmentPenalty(long long value)
	{
		TotalAlignmentPenalty = value;
	}

	double PerformanceReport::getBurstEfficiency() const
	{
		return BurstEfficiency;
	}

	void PerformanceReport::setBurstEfficiency(double value)
	{
		BurstEfficiency = value;
	}

	double PerformanceReport::getAverageAlignmentPenalty() const
	{
		if (getUnalignedAccessCount() == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getTotalAlignmentPenalty()) / getUnalignedAccessCount();
	}

	long long PerformanceReport::getEligibilityMaskedCycles() const
	{
		return EligibilityMaskedCycles;
	}

	void PerformanceReport::setEligibilityMaskedCycles(long long value)
	{
		EligibilityMaskedCycles = value;
	}

	long long PerformanceReport::getEligibilityMaskedReadyCandidates() const
	{
		return EligibilityMaskedReadyCandidates;
	}

	void PerformanceReport::setEligibilityMaskedReadyCandidates(long long value)
	{
		EligibilityMaskedReadyCandidates = value;
	}

	unsigned char PerformanceReport::getLastEligibilityRequestedMask() const
	{
		return LastEligibilityRequestedMask;
	}

	void PerformanceReport::setLastEligibilityRequestedMask(unsigned char value)
	{
		LastEligibilityRequestedMask = value;
	}

	unsigned char PerformanceReport::getLastEligibilityNormalizedMask() const
	{
		return LastEligibilityNormalizedMask;
	}

	void PerformanceReport::setLastEligibilityNormalizedMask(unsigned char value)
	{
		LastEligibilityNormalizedMask = value;
	}

	unsigned char PerformanceReport::getLastEligibilityReadyPortMask() const
	{
		return LastEligibilityReadyPortMask;
	}

	void PerformanceReport::setLastEligibilityReadyPortMask(unsigned char value)
	{
		LastEligibilityReadyPortMask = value;
	}

	unsigned char PerformanceReport::getLastEligibilityVisibleReadyMask() const
	{
		return LastEligibilityVisibleReadyMask;
	}

	void PerformanceReport::setLastEligibilityVisibleReadyMask(unsigned char value)
	{
		LastEligibilityVisibleReadyMask = value;
	}

	unsigned char PerformanceReport::getLastEligibilityMaskedReadyMask() const
	{
		return LastEligibilityMaskedReadyMask;
	}

	void PerformanceReport::setLastEligibilityMaskedReadyMask(unsigned char value)
	{
		LastEligibilityMaskedReadyMask = value;
	}

	bool PerformanceReport::getHasEligibilityTelemetry() const
	{
		return getEligibilityMaskedCycles() > 0 || getEligibilityMaskedReadyCandidates() > 0 || getLastEligibilityRequestedMask() != 0 || getLastEligibilityNormalizedMask() != 0 || getLastEligibilityReadyPortMask() != 0 || getLastEligibilityVisibleReadyMask() != 0 || getLastEligibilityMaskedReadyMask() != 0;
	}

	double PerformanceReport::getEligibilityMaskedReadyCandidatesPerMaskedCycle() const
	{
		return getEligibilityMaskedCycles() == 0 ? 0.0 : static_cast<double>(getEligibilityMaskedReadyCandidates()) / getEligibilityMaskedCycles();
	}

	void PerformanceReport::AppendEligibilityValidationSummary(std::shared_ptr<StringBuilder> sb)
	{
		if (!getHasEligibilityTelemetry())
		{
			return;
		}

// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Eligibility Gate: masked cycles {0}, masked ready candidates {1}, avg masked-ready/cycle {2:.2f}", getEligibilityMaskedCycles(), getEligibilityMaskedReadyCandidates(), getEligibilityMaskedReadyCandidatesPerMaskedCycle()));
	}

	void PerformanceReport::AppendEligibilityExecutionSummary(std::shared_ptr<StringBuilder> sb)
	{
		if (!getHasEligibilityTelemetry())
		{
			return;
		}

		AppendEligibilityValidationSummary(sb);
		sb->appendLine(std::format("  Eligibility Snapshot: requested 0x{0:02X}, normalized 0x{1:02X}, ready 0x{2:02X}, visible 0x{3:02X}, masked 0x{4:02X}", getLastEligibilityRequestedMask(), getLastEligibilityNormalizedMask(), getLastEligibilityReadyPortMask(), getLastEligibilityVisibleReadyMask(), getLastEligibilityMaskedReadyMask()));
	}

	long long PerformanceReport::getTotalInstructions() const
	{
		return TotalInstructions;
	}

	void PerformanceReport::setTotalInstructions(long long value)
	{
		TotalInstructions = value;
	}

	long long PerformanceReport::getTotalCycles() const
	{
		return TotalCycles;
	}

	void PerformanceReport::setTotalCycles(long long value)
	{
		TotalCycles = value;
	}

	double PerformanceReport::getIPC() const
	{
		if (getTotalCycles() == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getTotalInstructions()) / getTotalCycles();
	}

	long long PerformanceReport::getPipelineStalls() const
	{
		return PipelineStalls;
	}

	void PerformanceReport::setPipelineStalls(long long value)
	{
		PipelineStalls = value;
	}

	long long PerformanceReport::getBranchMispredictions() const
	{
		return BranchMispredictions;
	}

	void PerformanceReport::setBranchMispredictions(long long value)
	{
		BranchMispredictions = value;
	}

	long long PerformanceReport::getVectorOperations() const
	{
		return VectorOperations;
	}

	void PerformanceReport::setVectorOperations(long long value)
	{
		VectorOperations = value;
	}

	long long PerformanceReport::getVectorElementsProcessed() const
	{
		return VectorElementsProcessed;
	}

	void PerformanceReport::setVectorElementsProcessed(long long value)
	{
		VectorElementsProcessed = value;
	}

	long long PerformanceReport::getVectorExceptions() const
	{
		return VectorExceptions;
	}

	void PerformanceReport::setVectorExceptions(long long value)
	{
		VectorExceptions = value;
	}

	long long PerformanceReport::getTotalSchedulerCycles() const
	{
		return TotalSchedulerCycles;
	}

	void PerformanceReport::setTotalSchedulerCycles(long long value)
	{
		TotalSchedulerCycles = value;
	}

	long long PerformanceReport::getSuccessfulInjections() const
	{
		return SuccessfulInjections;
	}

	void PerformanceReport::setSuccessfulInjections(long long value)
	{
		SuccessfulInjections = value;
	}

	long long PerformanceReport::getRejectedInjections() const
	{
		return RejectedInjections;
	}

	void PerformanceReport::setRejectedInjections(long long value)
	{
		RejectedInjections = value;
	}

	long long PerformanceReport::getRejectedDueToHazards() const
	{
		return RejectedDueToHazards;
	}

	void PerformanceReport::setRejectedDueToHazards(long long value)
	{
		RejectedDueToHazards = value;
	}

	long long PerformanceReport::getRejectedDueToMemConflict() const
	{
		return RejectedDueToMemConflict;
	}

	void PerformanceReport::setRejectedDueToMemConflict(long long value)
	{
		RejectedDueToMemConflict = value;
	}

	long long PerformanceReport::getRollbackEvents() const
	{
		return RollbackEvents;
	}

	void PerformanceReport::setRollbackEvents(long long value)
	{
		RollbackEvents = value;
	}

	double PerformanceReport::getAverageReadyQueueDepth() const
	{
		return AverageReadyQueueDepth;
	}

	void PerformanceReport::setAverageReadyQueueDepth(double value)
	{
		AverageReadyQueueDepth = value;
	}

	int PerformanceReport::getMaxReadyQueueDepth() const
	{
		return MaxReadyQueueDepth;
	}

	void PerformanceReport::setMaxReadyQueueDepth(int value)
	{
		MaxReadyQueueDepth = value;
	}

	long long PerformanceReport::getThreadStarvationEvents() const
	{
		return ThreadStarvationEvents;
	}

	void PerformanceReport::setThreadStarvationEvents(long long value)
	{
		ThreadStarvationEvents = value;
	}

	double PerformanceReport::getSchedulerUtilization() const
	{
		return SchedulerUtilization;
	}

	void PerformanceReport::setSchedulerUtilization(double value)
	{
		SchedulerUtilization = value;
	}

	double PerformanceReport::getFSPContribution() const
	{
		return FSPContribution;
	}

	void PerformanceReport::setFSPContribution(double value)
	{
		FSPContribution = value;
	}

	const std::vector<double>& PerformanceReport::getPerThreadUtilization() const
	{
		return PerThreadUtilization;
	}

	void PerformanceReport::setPerThreadUtilization(const std::vector<double>& value)
	{
		PerThreadUtilization = value;
	}

	long long PerformanceReport::getTotalSchedulerLatency() const
	{
		return TotalSchedulerLatency;
	}

	void PerformanceReport::setTotalSchedulerLatency(long long value)
	{
		TotalSchedulerLatency = value;
	}

	double PerformanceReport::getAverageSchedulerLatency() const
	{
		return AverageSchedulerLatency;
	}

	void PerformanceReport::setAverageSchedulerLatency(double value)
	{
		AverageSchedulerLatency = value;
	}

	long long PerformanceReport::getBankContentionStalls() const
	{
		return BankContentionStalls;
	}

	void PerformanceReport::setBankContentionStalls(long long value)
	{
		BankContentionStalls = value;
	}

	long long PerformanceReport::getFspPipelineLatencyCycles() const
	{
		return FspPipelineLatencyCycles;
	}

	void PerformanceReport::setFspPipelineLatencyCycles(long long value)
	{
		FspPipelineLatencyCycles = value;
	}

	long long PerformanceReport::getReplayEpochCount() const
	{
		return ReplayEpochCount;
	}

	void PerformanceReport::setReplayEpochCount(long long value)
	{
		ReplayEpochCount = value;
	}

	double PerformanceReport::getAverageReplayEpochLength() const
	{
		return AverageReplayEpochLength;
	}

	void PerformanceReport::setAverageReplayEpochLength(double value)
	{
		AverageReplayEpochLength = value;
	}

	double PerformanceReport::getStableDonorSlotRatio() const
	{
		return StableDonorSlotRatio;
	}

	void PerformanceReport::setStableDonorSlotRatio(double value)
	{
		StableDonorSlotRatio = value;
	}

	long long PerformanceReport::getReplayAwareCycles() const
	{
		return ReplayAwareCycles;
	}

	void PerformanceReport::setReplayAwareCycles(long long value)
	{
		ReplayAwareCycles = value;
	}

	long long PerformanceReport::getPhaseCertificateReadyHits() const
	{
		return PhaseCertificateReadyHits;
	}

	void PerformanceReport::setPhaseCertificateReadyHits(long long value)
	{
		PhaseCertificateReadyHits = value;
	}

	long long PerformanceReport::getPhaseCertificateReadyMisses() const
	{
		return PhaseCertificateReadyMisses;
	}

	void PerformanceReport::setPhaseCertificateReadyMisses(long long value)
	{
		PhaseCertificateReadyMisses = value;
	}

	long long PerformanceReport::getEstimatedPhaseCertificateChecksSaved() const
	{
		return EstimatedPhaseCertificateChecksSaved;
	}

	void PerformanceReport::setEstimatedPhaseCertificateChecksSaved(long long value)
	{
		EstimatedPhaseCertificateChecksSaved = value;
	}

	long long PerformanceReport::getPhaseCertificateInvalidations() const
	{
		return PhaseCertificateInvalidations;
	}

	void PerformanceReport::setPhaseCertificateInvalidations(long long value)
	{
		PhaseCertificateInvalidations = value;
	}

	long long PerformanceReport::getPhaseCertificateMutationInvalidations() const
	{
		return PhaseCertificateMutationInvalidations;
	}

	void PerformanceReport::setPhaseCertificateMutationInvalidations(long long value)
	{
		PhaseCertificateMutationInvalidations = value;
	}

	long long PerformanceReport::getPhaseCertificatePhaseMismatchInvalidations() const
	{
		return PhaseCertificatePhaseMismatchInvalidations;
	}

	void PerformanceReport::setPhaseCertificatePhaseMismatchInvalidations(long long value)
	{
		PhaseCertificatePhaseMismatchInvalidations = value;
	}

	long long PerformanceReport::getDeterministicReplayTransitions() const
	{
		return DeterministicReplayTransitions;
	}

	void PerformanceReport::setDeterministicReplayTransitions(long long value)
	{
		DeterministicReplayTransitions = value;
	}

	const std::vector<long long>& PerformanceReport::getPerVtInjections() const
	{
		return PerVtInjections;
	}

	void PerformanceReport::setPerVtInjections(const std::vector<long long>& value)
	{
		PerVtInjections = value;
	}

	long long PerformanceReport::getFairnessStarvationEvents() const
	{
		return FairnessStarvationEvents;
	}

	void PerformanceReport::setFairnessStarvationEvents(long long value)
	{
		FairnessStarvationEvents = value;
	}

	long long PerformanceReport::getBankPressureAvoidanceCount() const
	{
		return BankPressureAvoidanceCount;
	}

	void PerformanceReport::setBankPressureAvoidanceCount(long long value)
	{
		BankPressureAvoidanceCount = value;
	}

	long long PerformanceReport::getSpeculationBudgetExhaustionEvents() const
	{
		return SpeculationBudgetExhaustionEvents;
	}

	void PerformanceReport::setSpeculationBudgetExhaustionEvents(long long value)
	{
		SpeculationBudgetExhaustionEvents = value;
	}

	long long PerformanceReport::getPeakConcurrentSpeculativeOps() const
	{
		return PeakConcurrentSpeculativeOps;
	}

	void PerformanceReport::setPeakConcurrentSpeculativeOps(long long value)
	{
		PeakConcurrentSpeculativeOps = value;
	}

	long long PerformanceReport::getDeterminismReferenceOpportunitySlots() const
	{
		return DeterminismReferenceOpportunitySlots;
	}

	void PerformanceReport::setDeterminismReferenceOpportunitySlots(long long value)
	{
		DeterminismReferenceOpportunitySlots = value;
	}

	long long PerformanceReport::getDeterminismReplayEligibleSlots() const
	{
		return DeterminismReplayEligibleSlots;
	}

	void PerformanceReport::setDeterminismReplayEligibleSlots(long long value)
	{
		DeterminismReplayEligibleSlots = value;
	}

	long long PerformanceReport::getDeterminismMaskedSlots() const
	{
		return DeterminismMaskedSlots;
	}

	void PerformanceReport::setDeterminismMaskedSlots(long long value)
	{
		DeterminismMaskedSlots = value;
	}

	long long PerformanceReport::getDeterminismEstimatedLostSlots() const
	{
		return DeterminismEstimatedLostSlots;
	}

	void PerformanceReport::setDeterminismEstimatedLostSlots(long long value)
	{
		DeterminismEstimatedLostSlots = value;
	}

	long long PerformanceReport::getDeterminismConstrainedCycles() const
	{
		return DeterminismConstrainedCycles;
	}

	void PerformanceReport::setDeterminismConstrainedCycles(long long value)
	{
		DeterminismConstrainedCycles = value;
	}

	long long PerformanceReport::getDomainIsolationProbeAttempts() const
	{
		return DomainIsolationProbeAttempts;
	}

	void PerformanceReport::setDomainIsolationProbeAttempts(long long value)
	{
		DomainIsolationProbeAttempts = value;
	}

	long long PerformanceReport::getDomainIsolationBlockedAttempts() const
	{
		return DomainIsolationBlockedAttempts;
	}

	void PerformanceReport::setDomainIsolationBlockedAttempts(long long value)
	{
		DomainIsolationBlockedAttempts = value;
	}

	long long PerformanceReport::getDomainIsolationCrossDomainBlocks() const
	{
		return DomainIsolationCrossDomainBlocks;
	}

	void PerformanceReport::setDomainIsolationCrossDomainBlocks(long long value)
	{
		DomainIsolationCrossDomainBlocks = value;
	}

	long long PerformanceReport::getDomainIsolationKernelToUserBlocks() const
	{
		return DomainIsolationKernelToUserBlocks;
	}

	void PerformanceReport::setDomainIsolationKernelToUserBlocks(long long value)
	{
		DomainIsolationKernelToUserBlocks = value;
	}

	long long PerformanceReport::getOracleGapTotalOracleSlots() const
	{
		return OracleGapTotalOracleSlots;
	}

	void PerformanceReport::setOracleGapTotalOracleSlots(long long value)
	{
		OracleGapTotalOracleSlots = value;
	}

	long long PerformanceReport::getOracleGapTotalRealSlots() const
	{
		return OracleGapTotalRealSlots;
	}

	void PerformanceReport::setOracleGapTotalRealSlots(long long value)
	{
		OracleGapTotalRealSlots = value;
	}

	long long PerformanceReport::getOracleGapDonorRestriction() const
	{
		return OracleGapDonorRestriction;
	}

	void PerformanceReport::setOracleGapDonorRestriction(long long value)
	{
		OracleGapDonorRestriction = value;
	}

	long long PerformanceReport::getOracleGapFairnessOrdering() const
	{
		return OracleGapFairnessOrdering;
	}

	void PerformanceReport::setOracleGapFairnessOrdering(long long value)
	{
		OracleGapFairnessOrdering = value;
	}

	long long PerformanceReport::getOracleGapLegalityConservatism() const
	{
		return OracleGapLegalityConservatism;
	}

	void PerformanceReport::setOracleGapLegalityConservatism(long long value)
	{
		OracleGapLegalityConservatism = value;
	}

	long long PerformanceReport::getOracleGapDomainIsolation() const
	{
		return OracleGapDomainIsolation;
	}

	void PerformanceReport::setOracleGapDomainIsolation(long long value)
	{
		OracleGapDomainIsolation = value;
	}

	long long PerformanceReport::getOracleGapSpeculationBudget() const
	{
		return OracleGapSpeculationBudget;
	}

	void PerformanceReport::setOracleGapSpeculationBudget(long long value)
	{
		OracleGapSpeculationBudget = value;
	}

	long long PerformanceReport::getOracleGapCyclesWithGap() const
	{
		return OracleGapCyclesWithGap;
	}

	void PerformanceReport::setOracleGapCyclesWithGap(long long value)
	{
		OracleGapCyclesWithGap = value;
	}

	long long PerformanceReport::getOracleGapTotalCyclesAnalyzed() const
	{
		return OracleGapTotalCyclesAnalyzed;
	}

	void PerformanceReport::setOracleGapTotalCyclesAnalyzed(long long value)
	{
		OracleGapTotalCyclesAnalyzed = value;
	}

	int PerformanceReport::getOracleCounterexampleCount() const
	{
		return OracleCounterexampleCount;
	}

	void PerformanceReport::setOracleCounterexampleCount(int value)
	{
		OracleCounterexampleCount = value;
	}

	long long PerformanceReport::getOracleCounterexampleTotalMissedSlots() const
	{
		return OracleCounterexampleTotalMissedSlots;
	}

	void PerformanceReport::setOracleCounterexampleTotalMissedSlots(long long value)
	{
		OracleCounterexampleTotalMissedSlots = value;
	}

	std::wstring PerformanceReport::getOracleCounterexampleDominantCategory() const
	{
		return OracleCounterexampleDominantCategory;
	}

	void PerformanceReport::setOracleCounterexampleDominantCategory(const std::wstring &value)
	{
		OracleCounterexampleDominantCategory = value;
	}

	long long PerformanceReport::getL1BypassHits() const
	{
		return L1BypassHits;
	}

	void PerformanceReport::setL1BypassHits(long long value)
	{
		L1BypassHits = value;
	}

	long long PerformanceReport::getForegroundWarmAttempts() const
	{
		return ForegroundWarmAttempts;
	}

	void PerformanceReport::setForegroundWarmAttempts(long long value)
	{
		ForegroundWarmAttempts = value;
	}

	long long PerformanceReport::getForegroundWarmSuccesses() const
	{
		return ForegroundWarmSuccesses;
	}

	void PerformanceReport::setForegroundWarmSuccesses(long long value)
	{
		ForegroundWarmSuccesses = value;
	}

	long long PerformanceReport::getForegroundWarmReuseHits() const
	{
		return ForegroundWarmReuseHits;
	}

	void PerformanceReport::setForegroundWarmReuseHits(long long value)
	{
		ForegroundWarmReuseHits = value;
	}

	long long PerformanceReport::getForegroundBypassHits() const
	{
		return ForegroundBypassHits;
	}

	void PerformanceReport::setForegroundBypassHits(long long value)
	{
		ForegroundBypassHits = value;
	}

	long long PerformanceReport::getAssistWarmAttempts() const
	{
		return AssistWarmAttempts;
	}

	void PerformanceReport::setAssistWarmAttempts(long long value)
	{
		AssistWarmAttempts = value;
	}

	long long PerformanceReport::getAssistWarmSuccesses() const
	{
		return AssistWarmSuccesses;
	}

	void PerformanceReport::setAssistWarmSuccesses(long long value)
	{
		AssistWarmSuccesses = value;
	}

	long long PerformanceReport::getAssistWarmReuseHits() const
	{
		return AssistWarmReuseHits;
	}

	void PerformanceReport::setAssistWarmReuseHits(long long value)
	{
		AssistWarmReuseHits = value;
	}

	long long PerformanceReport::getAssistBypassHits() const
	{
		return AssistBypassHits;
	}

	void PerformanceReport::setAssistBypassHits(long long value)
	{
		AssistBypassHits = value;
	}

	long long PerformanceReport::getStreamWarmTranslationRejects() const
	{
		return StreamWarmTranslationRejects;
	}

	void PerformanceReport::setStreamWarmTranslationRejects(long long value)
	{
		StreamWarmTranslationRejects = value;
	}

	long long PerformanceReport::getStreamWarmBackendRejects() const
	{
		return StreamWarmBackendRejects;
	}

	void PerformanceReport::setStreamWarmBackendRejects(long long value)
	{
		StreamWarmBackendRejects = value;
	}

	long long PerformanceReport::getAssistWarmResidentBudgetRejects() const
	{
		return AssistWarmResidentBudgetRejects;
	}

	void PerformanceReport::setAssistWarmResidentBudgetRejects(long long value)
	{
		AssistWarmResidentBudgetRejects = value;
	}

	long long PerformanceReport::getAssistWarmLoadingBudgetRejects() const
	{
		return AssistWarmLoadingBudgetRejects;
	}

	void PerformanceReport::setAssistWarmLoadingBudgetRejects(long long value)
	{
		AssistWarmLoadingBudgetRejects = value;
	}

	long long PerformanceReport::getAssistWarmNoVictimRejects() const
	{
		return AssistWarmNoVictimRejects;
	}

	void PerformanceReport::setAssistWarmNoVictimRejects(long long value)
	{
		AssistWarmNoVictimRejects = value;
	}

	int PerformanceReport::getActiveDmaTransfers() const
	{
		return ActiveDmaTransfers;
	}

	void PerformanceReport::setActiveDmaTransfers(int value)
	{
		ActiveDmaTransfers = value;
	}

	long long PerformanceReport::getCompletedDmaTransfers() const
	{
		return CompletedDmaTransfers;
	}

	void PerformanceReport::setCompletedDmaTransfers(long long value)
	{
		CompletedDmaTransfers = value;
	}

	long long PerformanceReport::getTotalDmaLatency() const
	{
		return TotalDmaLatency;
	}

	void PerformanceReport::setTotalDmaLatency(long long value)
	{
		TotalDmaLatency = value;
	}

	long long PerformanceReport::getDmaErrors() const
	{
		return DmaErrors;
	}

	void PerformanceReport::setDmaErrors(long long value)
	{
		DmaErrors = value;
	}

	long long PerformanceReport::getDmaBytesTransferred() const
	{
		return DmaBytesTransferred;
	}

	void PerformanceReport::setDmaBytesTransferred(long long value)
	{
		DmaBytesTransferred = value;
	}

	std::wstring PerformanceReport::GeneratePhase1ValidationSummary()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Phase 1 Validation Contour:");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Replay Evidence: {0} epochs, avg length {1:.2f}, {2} replay-aware cycles", getReplayEpochCount(), getAverageReplayEpochLength(), getReplayAwareCycles()));
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Dense Timeline: stable donor ratio {0:.2f}%, deterministic transitions {1}", getStableDonorSlotRatio() * 100, getDeterministicReplayTransitions()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Usefulness: {0} hits / {1} misses ({2:.2f}%), est. {3} checks saved", getPhaseCertificateReadyHits(), getPhaseCertificateReadyMisses(), getPhaseCertificateReuseHitRate() * 100, getEstimatedPhaseCertificateChecksSaved()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Invalidations: {0} total ({1:.2f}% per replay-aware cycle), mutation {2}, phase mismatch {3}", getPhaseCertificateInvalidations(), getPhaseCertificateInvalidationRate() * 100, getPhaseCertificateMutationInvalidations(), getPhaseCertificatePhaseMismatchInvalidations()));
		AppendEligibilityValidationSummary(sb);
		return sb->toString();
	}

	Phase1EvidenceCorrelationReport PerformanceReport::CorrelatePhase1Evidence(ReplayTraceEvidenceSummary traceSummary, ReplayDeterminismReport determinismReport)
	{
		bool replayEpochsAligned = getReplayEpochCount() == traceSummary.getReplayEpochCount();
		bool averageEpochLengthAligned = ApproximatelyEqual(getAverageReplayEpochLength(), traceSummary.getAverageEpochLength());
		bool stableDonorRatioAligned = ApproximatelyEqual(getStableDonorSlotRatio(), traceSummary.getStableDonorSlotRatio());
		bool usefulnessAligned = getPhaseCertificateReadyHits() == traceSummary.getPhaseCertificateReadyHits() && getPhaseCertificateReadyMisses() == traceSummary.getPhaseCertificateReadyMisses() && getEstimatedPhaseCertificateChecksSaved() == traceSummary.getEstimatedPhaseCertificateChecksSaved();
		bool invalidationsAligned = getPhaseCertificateInvalidations() == traceSummary.getPhaseCertificateInvalidations();
		bool eligibilityAligned = getEligibilityMaskedCycles() == traceSummary.getEligibilityMaskedCycles() && getEligibilityMaskedReadyCandidates() == traceSummary.getEligibilityMaskedReadyCandidates();
		bool determinismAligned = determinismReport.isDeterministic() && getDeterministicReplayTransitions() == determinismReport.getComparedEpochs();

		int mismatchCount = 0;
		mismatchCount += replayEpochsAligned ? 0 : 1;
		mismatchCount += averageEpochLengthAligned ? 0 : 1;
		mismatchCount += stableDonorRatioAligned ? 0 : 1;
		mismatchCount += usefulnessAligned ? 0 : 1;
		mismatchCount += invalidationsAligned ? 0 : 1;
		mismatchCount += eligibilityAligned ? 0 : 1;
		mismatchCount += determinismAligned ? 0 : 1;

		return Phase1EvidenceCorrelationReport(replayEpochsAligned, averageEpochLengthAligned, stableDonorRatioAligned, usefulnessAligned, invalidationsAligned, eligibilityAligned, determinismAligned, mismatchCount);
	}

	std::wstring PerformanceReport::GeneratePhase1RegressionBaselineSummary(ReplayTraceEvidenceSummary traceSummary, ReplayDeterminismReport determinismReport)
	{
		Phase1EvidenceCorrelationReport correlation = CorrelatePhase1Evidence(traceSummary, determinismReport);
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->append(GeneratePhase1ValidationSummary());
		sb->appendLine(L"Phase 1 Regression Baseline:");
		sb->appendLine(std::format(L"  Trace Evidence: {0}", traceSummary.Describe()));
		sb->appendLine(std::format(L"  Repeated Runs: {0}", determinismReport.Describe()));
		sb->appendLine(std::format(L"  Correlation: {0}", correlation.Describe()));
		return sb->toString();
	}

	std::wstring PerformanceReport::GeneratePhase2PolicySummary()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Phase 2 Policy Contour:");
		if (!getPerVtInjections().empty() && getPerVtInjections().size() >= 4)
		{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '4:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Fairness: VT0={0} VT1={1} VT2={2} VT3={3}, starvation events {4}", getPerVtInjections()[0], getPerVtInjections()[1], getPerVtInjections()[2], getPerVtInjections()[3], getFairnessStarvationEvents()));
		}
		else
		{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Fairness: starvation events {0}", getFairnessStarvationEvents()));
		}
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Bank Pressure: avoidance reorders {0}", getBankPressureAvoidanceCount()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Speculation Budget: exhaustion events {0}, peak concurrent {1}", getSpeculationBudgetExhaustionEvents(), getPeakConcurrentSpeculativeOps()));
		return sb->toString();
	}

	std::wstring PerformanceReport::GeneratePhase3ExecutionSummary()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Phase 3 Execution Contour:");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:P2' format specifier is not converted to C++:
		sb->appendLine(std::format("  Replay-Aware FSP: {0} replay-aware cycles, {1} epochs, stable donor ratio {2:.2f}%", getReplayAwareCycles(), getReplayEpochCount(), getStableDonorSlotRatio() * 100));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Phase-Certified Packing: {0} hits / {1} misses ({2:.2f}%), est. {3} checks saved", getPhaseCertificateReadyHits(), getPhaseCertificateReadyMisses(), getPhaseCertificateReuseHitRate() * 100, getEstimatedPhaseCertificateChecksSaved()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Invalidation Surface: {0} total, mutation {1}, phase mismatch {2}", getPhaseCertificateInvalidations(), getPhaseCertificateMutationInvalidations(), getPhaseCertificatePhaseMismatchInvalidations()));
		AppendEligibilityExecutionSummary(sb);
		return sb->toString();
	}

	std::wstring PerformanceReport::GeneratePhase4EvidenceSummary(const std::optional<ReplayEnvelopeReport> &envelopeReport)
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Phase 4 Evidence Baseline:");

		if (getDeterminismReferenceOpportunitySlots() > 0 || getDeterminismMaskedSlots() > 0 || getDeterminismEstimatedLostSlots() > 0)
		{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '4:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '5:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Determinism Tax: reference slots {0}, replay-eligible {1}, masked {2}, est. lost {3} ({4:.2f}%), constrained cycles {5}", getDeterminismReferenceOpportunitySlots(), getDeterminismReplayEligibleSlots(), getDeterminismMaskedSlots(), getDeterminismEstimatedLostSlots(), getDeterminismTaxRate() * 100, getDeterminismConstrainedCycles()));
		}

		if (getDomainIsolationProbeAttempts() > 0 || getDomainIsolationBlockedAttempts() > 0)
		{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '4:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Domain Isolation Stress: probes {0}, blocked {1} ({2:.2f}%), cross-domain {3}, kernel-to-user {4}", getDomainIsolationProbeAttempts(), getDomainIsolationBlockedAttempts(), getDomainIsolationBlockRate() * 100, getDomainIsolationCrossDomainBlocks(), getDomainIsolationKernelToUserBlocks()));
		}

		if (envelopeReport)
		{
			sb->appendLine(std::format(L"  Determinism Envelope: {0}", envelopeReport.value().Describe()));
		}

		return sb->toString();
	}

	std::wstring PerformanceReport::GeneratePhase4RegressionSnapshot(ReplayTraceEvidenceSummary traceSummary, ReplayDeterminismReport determinismReport, ReplayEnvelopeReport envelopeReport)
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->append(GeneratePhase4EvidenceSummary(envelopeReport));
		sb->appendLine(L"Phase 4 Regression Snapshot:");
// C# TO C++ CONVERTER TASK: The '0:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Opportunity Surface: retention {0:.2f}%, tax rate {1:.2f}%, masked slots {2}, constrained cycles {3}", getDeterminismOpportunityRetentionRate() * 100, getDeterminismTaxRate() * 100, getDeterminismMaskedSlots(), getDeterminismConstrainedCycles()));
// C# TO C++ CONVERTER TASK: The '0:P2' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Isolation Surface: block rate {0:.2f}%, probes {1}, blocked {2}", getDomainIsolationBlockRate() * 100, getDomainIsolationProbeAttempts(), getDomainIsolationBlockedAttempts()));
		sb->appendLine(std::format(L"  Trace Evidence: {0}", traceSummary.Describe()));
		sb->appendLine(std::format(L"  Repeated Runs: {0}", determinismReport.Describe()));
		sb->appendLine(std::format(L"  Envelope Result: {0}", envelopeReport.Describe()));
		sb->appendLine(std::format(L"  Snapshot Verdict: baseline {0}, envelope {1}", (determinismReport.isDeterministic() ? L"stable" : L"diverged"), (envelopeReport.isWithinEnvelope() ? L"within bounds" : L"out of envelope")));
		return sb->toString();
	}

	std::wstring PerformanceReport::GeneratePhase8TypedSlotSummary()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Typed-Slot Statistics (Phase 08):");

		long long totalInjects = getClassFlexibleInjects() + getHardPinnedInjects();
		if (totalInjects > 0)
		{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:P1' format specifier is not converted to C++:
			sb->appendLine(std::format("  Class-Flexible Injects: {0} ({1:.1f}%)", getClassFlexibleInjects(), getClassFlexibleRatio() * 100));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:P1' format specifier is not converted to C++:
			sb->appendLine(std::format("  Hard-Pinned Injects: {0} ({1:.1f}%)", getHardPinnedInjects(), (1.0 - getClassFlexibleRatio()) * 100));
		}

		long long totalRejects = getStaticClassOvercommitRejects() + getDynamicClassExhaustionRejects() + getPinnedLaneConflicts() + getLateBindingConflicts() + getNopDueToResourceConflict() + getTypedSlotDomainRejects();
		if (totalRejects > 0)
		{
			sb->appendLine(L"  Reject Breakdown:");
			if (getStaticClassOvercommitRejects() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("    StaticClassOvercommit: {0}", getStaticClassOvercommitRejects()));
			}
			if (getDynamicClassExhaustionRejects() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("    DynamicClassExhaustion: {0}", getDynamicClassExhaustionRejects()));
			}
			if (getPinnedLaneConflicts() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("    PinnedLaneConflict: {0}", getPinnedLaneConflicts()));
			}
			if (getLateBindingConflicts() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("    LateBindingConflict: {0}", getLateBindingConflicts()));
			}
			if (getNopDueToResourceConflict() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("    ResourceConflict: {0}", getNopDueToResourceConflict()));
			}
			if (getTypedSlotDomainRejects() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("    DomainReject: {0}", getTypedSlotDomainRejects()));
			}
		}

		if (getTotalLaneBindings() > 0)
		{
// C# TO C++ CONVERTER TASK: The '0:P1' format specifier is not converted to C++:
			sb->appendLine(std::format("  Lane Reuse Rate: {0:.1f}%", getLaneReuseRate() * 100));
		}
		if (getClassTemplateReuseHits() > 0 || getClassTemplateInvalidations() > 0)
		{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Class Template: {0} hits, {1} invalidations", getClassTemplateReuseHits(), getClassTemplateInvalidations()));
		}
		if (getNopAvoided() > 0 || getNopDueToPinnedConstraint() > 0 || getNopDueToNoClassCapacity() > 0)
		{
// C# TO C++ CONVERTER TASK: The '0:P1' format specifier is not converted to C++:
			sb->appendLine(std::format("  NOP Reduction: {0:.1f}% avoided via class-flexible placement", getNopReductionRate() * 100));
		}

		return sb->toString();
	}

	std::wstring PerformanceReport::GenerateSummary()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"=== HybridCPU Performance Report ===");
		sb->appendLine();

		sb->appendLine(L"Memory Subsystem:");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Total Bursts: {0}", getTotalBursts()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format(L"  Total Bytes: {0} ({1})", getTotalBytesTransferred(), FormatBytes(getTotalBytesTransferred())));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Bank Conflicts: {0}", getBankConflicts()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Stall Cycles: {0}", getStallCycles()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  DMA Transfers: {0}", getDmaTransfers()));
		sb->appendLine(std::format("  Avg Burst Length: {0:.2f} bytes", getAverageBurstLength()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Total Wait Cycles: {0}", getTotalWaitCycles()));
		sb->appendLine(std::format("  Avg Wait Cycles: {0:.2f}", getAverageWaitCycles()));
		sb->appendLine(std::format("  Max Queue Depth: {0}", getMaxQueueDepth()));
		sb->appendLine(std::format("  Current Queued: {0}", getCurrentQueuedRequests()));
		sb->appendLine();

		// Phase 3: Memory Wall metrics
		sb->appendLine(L"Memory Wall (Phase 3):");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Total Memory Stalls: {0}", getTotalMemoryStalls()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Queue Full Events: {0}", getMemoryQueueFullEvents()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Bank Saturation Cycles: {0}", getBankSaturationCycles()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Idle Cycles: {0}", getIdleMemoryCycles()));
		sb->appendLine(std::format("  Memory Utilization: {0:.2f}%", getMemoryUtilization() * 100));
		sb->appendLine(std::format("  Avg Queue Depth: {0:.2f}", getAverageMemoryQueueDepth()));
		sb->appendLine(std::format("  Avg Memory Latency: {0:.2f} cycles", getAverageMemoryLatency()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Load Stalls: {0}", getLoadStalls()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Store Stalls: {0}", getStoreStalls()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Burst Stalls: {0}", getBurstStalls()));
		sb->appendLine();

		// Phase 3: Burst timing metrics
		if (getUnalignedAccessCount() > 0 || getBurstEfficiency() > 0)
		{
			sb->appendLine(L"Burst Timing & Efficiency (Phase 3):");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Unaligned Accesses: {0}", getUnalignedAccessCount()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Total Alignment Penalty: {0} cycles", getTotalAlignmentPenalty()));
			sb->appendLine(std::format("  Avg Alignment Penalty: {0:.2f} cycles", getAverageAlignmentPenalty()));
			sb->appendLine(std::format("  Burst Efficiency: {0:.2f}%", getBurstEfficiency() * 100));
			sb->appendLine();
		}

		sb->appendLine(L"DMA Controller:");
		sb->appendLine(std::format("  Active Transfers: {0}", getActiveDmaTransfers()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Completed Transfers: {0}", getCompletedDmaTransfers()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Total Latency: {0} cycles", getTotalDmaLatency()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Errors: {0}", getDmaErrors()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format(L"  Bytes Transferred: {0} ({1})", getDmaBytesTransferred(), FormatBytes(getDmaBytesTransferred())));
		if (getCompletedDmaTransfers() > 0)
		{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Avg Latency: {0} cycles/transfer", getTotalDmaLatency() / getCompletedDmaTransfers()));
		}
		sb->appendLine();

		sb->appendLine(L"Pipeline:");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Total Instructions: {0}", getTotalInstructions()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Total Cycles: {0}", getTotalCycles()));
		sb->appendLine(std::format("  IPC: {0:.3f}", getIPC()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Pipeline Stalls: {0}", getPipelineStalls()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Branch Mispredictions: {0}", getBranchMispredictions()));
		sb->appendLine();

		sb->appendLine(L"Vector Engine:");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Vector Operations: {0}", getVectorOperations()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Elements Processed: {0}", getVectorElementsProcessed()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("  Vector Exceptions: {0}", getVectorExceptions()));
		sb->appendLine();

		// Phase 3: Scheduler metrics
		if (getSuccessfulInjections() > 0 || getRejectedInjections() > 0)
		{
			sb->appendLine(L"MicroOp Scheduler (Phase 3):");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Successful Injections: {0}", getSuccessfulInjections()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Rejected Injections: {0}", getRejectedInjections()));
			long long totalAttempts = getSuccessfulInjections() + getRejectedInjections();
			if (totalAttempts > 0)
			{
				sb->appendLine(std::format("  Rejection Rate: {0:.2f}%", getRejectedInjections() * 100.0 / totalAttempts));
			}
			sb->appendLine(std::format("  FSP Contribution: {0:.2f}%", getFSPContribution() * 100));
			sb->appendLine(std::format("  Scheduler Utilization: {0:.2f}%", getSchedulerUtilization() * 100));
			sb->appendLine(std::format("  Avg Ready Queue Depth: {0:.2f}", getAverageReadyQueueDepth()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Bank Contention Stalls: {0}", getBankContentionStalls()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  FSP Pipeline Latency Cycles: {0}", getFspPipelineLatencyCycles()));
			sb->appendLine();
		}

		if (getHasPhase1ValidationTelemetry())
		{
			sb->append(GeneratePhase1ValidationSummary());
			sb->appendLine();
		}

		if (getHasPhase3ReuseTelemetry())
		{
			sb->append(GeneratePhase3ExecutionSummary());
			sb->appendLine();
		}

		if (getHasPhase4EvidenceTelemetry())
		{
			sb->append(GeneratePhase4EvidenceSummary());
			sb->appendLine();
		}

		if (getHasPhase5OracleGapTelemetry())
		{
			sb->append(GeneratePhase5OracleGapSummary());
			sb->appendLine();
		}

		if (getHasPhase2PolicyTelemetry())
		{
			sb->append(GeneratePhase2PolicySummary());
			sb->appendLine();
		}

		if (getHasPhase8TypedSlotTelemetry())
		{
			sb->append(GeneratePhase8TypedSlotSummary());
			sb->appendLine();
		}

		// Q1 Review §5: Stream Register File ingress/bypass metrics
		if (getHasStreamIngressWarmTelemetry())
		{
			sb->appendLine(L"Stream Register File (Q1 §5):");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  L1 Bypass Hits: {0}", getL1BypassHits()));
			if (getForegroundWarmAttempts() > 0 || getForegroundWarmSuccesses() > 0 || getForegroundWarmReuseHits() > 0 || getForegroundBypassHits() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("  Foreground Warm: {0}/{1} success, {2} reuse, {3} bypass", getForegroundWarmSuccesses(), getForegroundWarmAttempts(), getForegroundWarmReuseHits(), getForegroundBypassHits()));
			}

			if (getAssistWarmAttempts() > 0 || getAssistWarmSuccesses() > 0 || getAssistWarmReuseHits() > 0 || getAssistBypassHits() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("  Assist Warm: {0}/{1} success, {2} reuse, {3} bypass", getAssistWarmSuccesses(), getAssistWarmAttempts(), getAssistWarmReuseHits(), getAssistBypassHits()));
			}

			if (getStreamWarmTranslationRejects() > 0 || getStreamWarmBackendRejects() > 0 || getAssistWarmResidentBudgetRejects() > 0 || getAssistWarmLoadingBudgetRejects() > 0 || getAssistWarmNoVictimRejects() > 0)
			{
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '4:N0' format specifier is not converted to C++:
				sb->appendLine(std::format("  Warm Rejects: translation {0}, backend {1}, resident-budget {2}, loading-budget {3}, no-victim {4}", getStreamWarmTranslationRejects(), getStreamWarmBackendRejects(), getAssistWarmResidentBudgetRejects(), getAssistWarmLoadingBudgetRejects(), getAssistWarmNoVictimRejects()));
			}

			sb->appendLine();
		}

		AppendBurstTraceSummary(sb);

		return sb->toString();
	}

	std::wstring PerformanceReport::ExportToCSV()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Category,Metric,Value");

		// Memory
		sb->appendLine(std::format("Memory,TotalBursts,{0}", getTotalBursts()));
		sb->appendLine(std::format("Memory,TotalBytesTransferred,{0}", getTotalBytesTransferred()));
		sb->appendLine(std::format("Memory,BankConflicts,{0}", getBankConflicts()));
		sb->appendLine(std::format("Memory,StallCycles,{0}", getStallCycles()));
		sb->appendLine(std::format("Memory,DmaTransfers,{0}", getDmaTransfers()));
		sb->appendLine(std::format("Memory,AverageBurstLength,{0}", getAverageBurstLength()));
		sb->appendLine(std::format("Memory,TotalWaitCycles,{0}", getTotalWaitCycles()));
		sb->appendLine(std::format("Memory,AverageWaitCycles,{0}", getAverageWaitCycles()));
		sb->appendLine(std::format("Memory,MaxQueueDepth,{0}", getMaxQueueDepth()));
		sb->appendLine(std::format("Memory,CurrentQueuedRequests,{0}", getCurrentQueuedRequests()));

		// Phase 3: Memory wall
		sb->appendLine(std::format("MemoryWall,TotalMemoryStalls,{0}", getTotalMemoryStalls()));
		sb->appendLine(std::format("MemoryWall,MemoryQueueFullEvents,{0}", getMemoryQueueFullEvents()));
		sb->appendLine(std::format("MemoryWall,BankSaturationCycles,{0}", getBankSaturationCycles()));
		sb->appendLine(std::format("MemoryWall,IdleMemoryCycles,{0}", getIdleMemoryCycles()));
		sb->appendLine(std::format("MemoryWall,MemoryUtilization,{0}", getMemoryUtilization()));
		sb->appendLine(std::format("MemoryWall,AverageMemoryQueueDepth,{0}", getAverageMemoryQueueDepth()));
		sb->appendLine(std::format("MemoryWall,AverageMemoryLatency,{0}", getAverageMemoryLatency()));
		sb->appendLine(std::format("MemoryWall,LoadStalls,{0}", getLoadStalls()));
		sb->appendLine(std::format("MemoryWall,StoreStalls,{0}", getStoreStalls()));
		sb->appendLine(std::format("MemoryWall,BurstStalls,{0}", getBurstStalls()));

		// Phase 3: Burst timing
		sb->appendLine(std::format("BurstTiming,UnalignedAccessCount,{0}", getUnalignedAccessCount()));
		sb->appendLine(std::format("BurstTiming,TotalAlignmentPenalty,{0}", getTotalAlignmentPenalty()));
		sb->appendLine(std::format("BurstTiming,AverageAlignmentPenalty,{0}", getAverageAlignmentPenalty()));
		sb->appendLine(std::format("BurstTiming,BurstEfficiency,{0}", getBurstEfficiency()));

		// DMA
		sb->appendLine(std::format("DMA,ActiveTransfers,{0}", getActiveDmaTransfers()));
		sb->appendLine(std::format("DMA,CompletedTransfers,{0}", getCompletedDmaTransfers()));
		sb->appendLine(std::format("DMA,TotalLatency,{0}", getTotalDmaLatency()));
		sb->appendLine(std::format("DMA,Errors,{0}", getDmaErrors()));
		sb->appendLine(std::format("DMA,BytesTransferred,{0}", getDmaBytesTransferred()));

		// Pipeline
		sb->appendLine(std::format("Pipeline,TotalInstructions,{0}", getTotalInstructions()));
		sb->appendLine(std::format("Pipeline,TotalCycles,{0}", getTotalCycles()));
		sb->appendLine(std::format("Pipeline,IPC,{0}", getIPC()));
		sb->appendLine(std::format("Pipeline,PipelineStalls,{0}", getPipelineStalls()));
		sb->appendLine(std::format("Pipeline,BranchMispredictions,{0}", getBranchMispredictions()));

		// Vector
		sb->appendLine(std::format("Vector,VectorOperations,{0}", getVectorOperations()));
		sb->appendLine(std::format("Vector,ElementsProcessed,{0}", getVectorElementsProcessed()));
		sb->appendLine(std::format("Vector,VectorExceptions,{0}", getVectorExceptions()));

		// Phase 3: Scheduler
		sb->appendLine(std::format("Scheduler,TotalSchedulerCycles,{0}", getTotalSchedulerCycles()));
		sb->appendLine(std::format("Scheduler,SuccessfulInjections,{0}", getSuccessfulInjections()));
		sb->appendLine(std::format("Scheduler,RejectedInjections,{0}", getRejectedInjections()));
		sb->appendLine(std::format("Scheduler,RejectedDueToHazards,{0}", getRejectedDueToHazards()));
		sb->appendLine(std::format("Scheduler,RejectedDueToMemConflict,{0}", getRejectedDueToMemConflict()));
		sb->appendLine(std::format("Scheduler,RollbackEvents,{0}", getRollbackEvents()));
		sb->appendLine(std::format("Scheduler,AverageReadyQueueDepth,{0}", getAverageReadyQueueDepth()));
		sb->appendLine(std::format("Scheduler,MaxReadyQueueDepth,{0}", getMaxReadyQueueDepth()));
		sb->appendLine(std::format("Scheduler,ThreadStarvationEvents,{0}", getThreadStarvationEvents()));
		sb->appendLine(std::format("Scheduler,SchedulerUtilization,{0}", getSchedulerUtilization()));
		sb->appendLine(std::format("Scheduler,FSPContribution,{0}", getFSPContribution()));
		sb->appendLine(std::format("Scheduler,TotalSchedulerLatency,{0}", getTotalSchedulerLatency()));
		sb->appendLine(std::format("Scheduler,AverageSchedulerLatency,{0}", getAverageSchedulerLatency()));
		sb->appendLine(std::format("Scheduler,BankContentionStalls,{0}", getBankContentionStalls()));
		sb->appendLine(std::format("Scheduler,FspPipelineLatencyCycles,{0}", getFspPipelineLatencyCycles()));
		sb->appendLine(std::format("Scheduler,ReplayEpochCount,{0}", getReplayEpochCount()));
		sb->appendLine(std::format("Scheduler,AverageReplayEpochLength,{0}", getAverageReplayEpochLength()));
		sb->appendLine(std::format("Scheduler,StableDonorSlotRatio,{0}", getStableDonorSlotRatio()));
		sb->appendLine(std::format("Scheduler,ReplayAwareCycles,{0}", getReplayAwareCycles()));
		sb->appendLine(std::format("Scheduler,PhaseCertificateReadyHits,{0}", getPhaseCertificateReadyHits()));
		sb->appendLine(std::format("Scheduler,PhaseCertificateReadyMisses,{0}", getPhaseCertificateReadyMisses()));
		sb->appendLine(std::format("Scheduler,PhaseCertificateReuseHitRate,{0}", getPhaseCertificateReuseHitRate()));
		sb->appendLine(std::format("Scheduler,EstimatedPhaseCertificateChecksSaved,{0}", getEstimatedPhaseCertificateChecksSaved()));
		sb->appendLine(std::format("Scheduler,PhaseCertificateInvalidations,{0}", getPhaseCertificateInvalidations()));
		sb->appendLine(std::format("Scheduler,PhaseCertificateInvalidationRate,{0}", getPhaseCertificateInvalidationRate()));
		sb->appendLine(std::format("Scheduler,PhaseCertificateMutationInvalidations,{0}", getPhaseCertificateMutationInvalidations()));
		sb->appendLine(std::format("Scheduler,PhaseCertificatePhaseMismatchInvalidations,{0}", getPhaseCertificatePhaseMismatchInvalidations()));
		sb->appendLine(std::format("Scheduler,DeterministicReplayTransitions,{0}", getDeterministicReplayTransitions()));
		sb->appendLine(std::format("Scheduler,DeterminismReferenceOpportunitySlots,{0}", getDeterminismReferenceOpportunitySlots()));
		sb->appendLine(std::format("Scheduler,DeterminismReplayEligibleSlots,{0}", getDeterminismReplayEligibleSlots()));
		sb->appendLine(std::format("Scheduler,DeterminismMaskedSlots,{0}", getDeterminismMaskedSlots()));
		sb->appendLine(std::format("Scheduler,DeterminismEstimatedLostSlots,{0}", getDeterminismEstimatedLostSlots()));
		sb->appendLine(std::format("Scheduler,DeterminismConstrainedCycles,{0}", getDeterminismConstrainedCycles()));
		sb->appendLine(std::format("Scheduler,DeterminismTaxRate,{0}", getDeterminismTaxRate()));
		sb->appendLine(std::format("Scheduler,DomainIsolationProbeAttempts,{0}", getDomainIsolationProbeAttempts()));
		sb->appendLine(std::format("Scheduler,DomainIsolationBlockedAttempts,{0}", getDomainIsolationBlockedAttempts()));
		sb->appendLine(std::format("Scheduler,DomainIsolationCrossDomainBlocks,{0}", getDomainIsolationCrossDomainBlocks()));
		sb->appendLine(std::format("Scheduler,DomainIsolationKernelToUserBlocks,{0}", getDomainIsolationKernelToUserBlocks()));
		sb->appendLine(std::format("Scheduler,DomainIsolationBlockRate,{0}", getDomainIsolationBlockRate()));

		// Phase 5: Oracle Gap
		sb->appendLine(std::format("OracleGap,TotalOracleSlots,{0}", getOracleGapTotalOracleSlots()));
		sb->appendLine(std::format("OracleGap,TotalRealSlots,{0}", getOracleGapTotalRealSlots()));
		sb->appendLine(std::format("OracleGap,DonorRestriction,{0}", getOracleGapDonorRestriction()));
		sb->appendLine(std::format("OracleGap,FairnessOrdering,{0}", getOracleGapFairnessOrdering()));
		sb->appendLine(std::format("OracleGap,LegalityConservatism,{0}", getOracleGapLegalityConservatism()));
		sb->appendLine(std::format("OracleGap,DomainIsolation,{0}", getOracleGapDomainIsolation()));
		sb->appendLine(std::format("OracleGap,SpeculationBudget,{0}", getOracleGapSpeculationBudget()));
		sb->appendLine(std::format("OracleGap,CyclesWithGap,{0}", getOracleGapCyclesWithGap()));
		sb->appendLine(std::format("OracleGap,TotalCyclesAnalyzed,{0}", getOracleGapTotalCyclesAnalyzed()));
		sb->appendLine(std::format("OracleGap,OracleEfficiency,{0}", getOracleEfficiency()));
		sb->appendLine(std::format("OracleGap,GapRate,{0}", getOracleGapRate()));
		sb->appendLine(std::format("OracleGap,CounterexampleCount,{0}", getOracleCounterexampleCount()));
		sb->appendLine(std::format("OracleGap,CounterexampleTotalMissedSlots,{0}", getOracleCounterexampleTotalMissedSlots()));

		// Phase 2: Fairness / Pressure / Speculation Budget
		sb->appendLine(std::format("Scheduler,FairnessStarvationEvents,{0}", getFairnessStarvationEvents()));
		sb->appendLine(std::format("Scheduler,BankPressureAvoidanceCount,{0}", getBankPressureAvoidanceCount()));
		sb->appendLine(std::format("Scheduler,SpeculationBudgetExhaustionEvents,{0}", getSpeculationBudgetExhaustionEvents()));
		sb->appendLine(std::format("Scheduler,PeakConcurrentSpeculativeOps,{0}", getPeakConcurrentSpeculativeOps()));
		if (!getPerVtInjections().empty())
		{
			for (int vt = 0; vt < getPerVtInjections().size() && vt < 4; vt++)
			{
				sb->appendLine(std::format("Scheduler,PerVtInjections_VT{0},{1}", vt, getPerVtInjections()[vt]));
			}
		}

		// Q1 Review §5: Stream Register File
		sb->appendLine(std::format("StreamRegFile,L1BypassHits,{0}", getL1BypassHits()));
		sb->appendLine(std::format("StreamRegFile,ForegroundWarmAttempts,{0}", getForegroundWarmAttempts()));
		sb->appendLine(std::format("StreamRegFile,ForegroundWarmSuccesses,{0}", getForegroundWarmSuccesses()));
		sb->appendLine(std::format("StreamRegFile,ForegroundWarmReuseHits,{0}", getForegroundWarmReuseHits()));
		sb->appendLine(std::format("StreamRegFile,ForegroundBypassHits,{0}", getForegroundBypassHits()));
		sb->appendLine(std::format("StreamRegFile,AssistWarmAttempts,{0}", getAssistWarmAttempts()));
		sb->appendLine(std::format("StreamRegFile,AssistWarmSuccesses,{0}", getAssistWarmSuccesses()));
		sb->appendLine(std::format("StreamRegFile,AssistWarmReuseHits,{0}", getAssistWarmReuseHits()));
		sb->appendLine(std::format("StreamRegFile,AssistBypassHits,{0}", getAssistBypassHits()));
		sb->appendLine(std::format("StreamRegFile,StreamWarmTranslationRejects,{0}", getStreamWarmTranslationRejects()));
		sb->appendLine(std::format("StreamRegFile,StreamWarmBackendRejects,{0}", getStreamWarmBackendRejects()));
		sb->appendLine(std::format("StreamRegFile,AssistWarmResidentBudgetRejects,{0}", getAssistWarmResidentBudgetRejects()));
		sb->appendLine(std::format("StreamRegFile,AssistWarmLoadingBudgetRejects,{0}", getAssistWarmLoadingBudgetRejects()));
		sb->appendLine(std::format("StreamRegFile,AssistWarmNoVictimRejects,{0}", getAssistWarmNoVictimRejects()));

		return sb->toString();
	}

	std::wstring PerformanceReport::ExportSchedulerMetrics()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Scheduler Metrics:");
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("Total Cycles: {0}", getTotalSchedulerCycles()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("Successful Injections: {0}", getSuccessfulInjections()));

		long long totalAttempts = getSuccessfulInjections() + getRejectedInjections();
		if (totalAttempts > 0)
		{
			sb->appendLine(std::format("Rejection Rate: {0:.2f}%", getRejectedInjections() * 100.0 / totalAttempts));
		}

// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("Hazard Rejections: {0}", getRejectedDueToHazards()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("Memory Conflict Rejections: {0}", getRejectedDueToMemConflict()));
		sb->appendLine(std::format("FSP Contribution: {0:.2f}%", getFSPContribution() * 100));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("Rollback Events: {0}", getRollbackEvents()));
		sb->appendLine(std::format("Scheduler Utilization: {0:.2f}%", getSchedulerUtilization() * 100));
		sb->appendLine(std::format("Avg Ready Queue Depth: {0:.2f}", getAverageReadyQueueDepth()));
		sb->appendLine(std::format("Max Ready Queue Depth: {0}", getMaxReadyQueueDepth()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
		sb->appendLine(std::format("Thread Starvation Events: {0}", getThreadStarvationEvents()));

		if (getHasPhase4EvidenceTelemetry())
		{
// C# TO C++ CONVERTER TASK: The '0:P2' format specifier is not converted to C++:
			sb->appendLine(std::format("Determinism Tax Rate: {0:.2f}%", getDeterminismTaxRate() * 100));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("Determinism Masked Slots: {0}", getDeterminismMaskedSlots()));
// C# TO C++ CONVERTER TASK: The '0:P2' format specifier is not converted to C++:
			sb->appendLine(std::format("Domain Isolation Block Rate: {0:.2f}%", getDomainIsolationBlockRate() * 100));
		}

		if (getTotalSchedulerCycles() > 0)
		{
			sb->appendLine(std::format("Avg Scheduler Latency: {0:.2f} cycles/bundle", getAverageSchedulerLatency()));
		}

		return sb->toString();
	}

	std::wstring PerformanceReport::GeneratePhase5OracleGapSummary()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"Phase 5 Oracle Gap Analysis:");

		if (getOracleGapTotalCyclesAnalyzed() > 0)
		{
			long long totalGap = getOracleGapTotalOracleSlots() - getOracleGapTotalRealSlots();
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:P2' format specifier is not converted to C++:
			sb->appendLine(std::format("  Oracle Contour: {0} oracle slots, {1} real slots, gap {2} ({3:.2f}%)", getOracleGapTotalOracleSlots(), getOracleGapTotalRealSlots(), totalGap, getOracleGapRate() * 100));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Gap Cycles: {0} / {1} cycles", getOracleGapCyclesWithGap(), getOracleGapTotalCyclesAnalyzed()));
// C# TO C++ CONVERTER TASK: The '0:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '2:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '3:N0' format specifier is not converted to C++:
// C# TO C++ CONVERTER TASK: The '4:N0' format specifier is not converted to C++:
			sb->appendLine(std::format("  Gap Decomposition: donor restriction {0}, fairness ordering {1}, legality conservatism {2}, domain isolation {3}, speculation budget {4}", getOracleGapDonorRestriction(), getOracleGapFairnessOrdering(), getOracleGapLegalityConservatism(), getOracleGapDomainIsolation(), getOracleGapSpeculationBudget()));
		}

		if (getOracleCounterexampleCount() > 0)
		{
// C# TO C++ CONVERTER TASK: The '1:N0' format specifier is not converted to C++:
			sb->appendLine(std::format(L"  Counterexamples: {0} captured, {1} total missed slots, dominant category: {2}", getOracleCounterexampleCount(), getOracleCounterexampleTotalMissedSlots(), (getOracleCounterexampleDominantCategory() != L"") ? getOracleCounterexampleDominantCategory() : L"none"));
		}

		return sb->toString();
	}

	std::wstring PerformanceReport::FormatBytes(long long bytes)
	{
		std::vector<std::wstring> sizes = {L"B", L"KB", L"MB", L"GB", L"TB"};
		double len = bytes;
		int order = 0;
		while (len >= 1024 && order < sizes.size() - 1)
		{
			order++;
			len = len / 1024;
		}
		std::wostringstream formatted;
		formatted << std::fixed << std::setprecision(2) << len << L" " << sizes[order];
		return formatted.str();
	}

	bool PerformanceReport::ApproximatelyEqual(double left, double right, double tolerance)
	{
		return std::abs(left - right) <= tolerance;
	}

	double PerformanceReport::getPhaseCertificateReuseHitRate() const
	{
		long long totalReadinessChecks = getPhaseCertificateReadyHits() + getPhaseCertificateReadyMisses();
		if (totalReadinessChecks == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getPhaseCertificateReadyHits()) / totalReadinessChecks;
	}

	double PerformanceReport::getPhaseCertificateInvalidationRate() const
	{
		if (getReplayAwareCycles() == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getPhaseCertificateInvalidations()) / getReplayAwareCycles();
	}

	bool PerformanceReport::getHasPhase1ValidationTelemetry() const
	{
		return getReplayEpochCount() > 0 || getReplayAwareCycles() > 0 || getPhaseCertificateReadyHits() > 0 || getPhaseCertificateReadyMisses() > 0 || getEstimatedPhaseCertificateChecksSaved() > 0 || getPhaseCertificateInvalidations() > 0 || getDeterministicReplayTransitions() > 0;
	}

	bool PerformanceReport::getHasPhase2PolicyTelemetry() const
{
	const auto perVtInjections = getPerVtInjections();
	return getFairnessStarvationEvents() > 0 || getBankPressureAvoidanceCount() > 0 || getSpeculationBudgetExhaustionEvents() > 0 || getPeakConcurrentSpeculativeOps() > 0 || std::any_of(perVtInjections.begin(), perVtInjections.end(), [](long long value) { return value > 0; });
	}

	bool PerformanceReport::getHasPhase3ReuseTelemetry() const
	{
		return getReplayAwareCycles() > 0 || getPhaseCertificateReadyHits() > 0 || getPhaseCertificateReadyMisses() > 0 || getEstimatedPhaseCertificateChecksSaved() > 0 || getPhaseCertificateInvalidations() > 0 || getHasEligibilityTelemetry();
	}

	bool PerformanceReport::getHasPhase4EvidenceTelemetry() const
	{
		return getDeterminismReferenceOpportunitySlots() > 0 || getDeterminismReplayEligibleSlots() > 0 || getDeterminismMaskedSlots() > 0 || getDeterminismEstimatedLostSlots() > 0 || getDeterminismConstrainedCycles() > 0 || getDomainIsolationProbeAttempts() > 0 || getDomainIsolationBlockedAttempts() > 0;
	}

	double PerformanceReport::getDeterminismOpportunityRetentionRate() const
	{
		return getDeterminismReferenceOpportunitySlots() == 0 ? 0.0 : static_cast<double>(getDeterminismReplayEligibleSlots()) / getDeterminismReferenceOpportunitySlots();
	}

	double PerformanceReport::getDeterminismTaxRate() const
	{
		return getDeterminismReferenceOpportunitySlots() == 0 ? 0.0 : static_cast<double>(getDeterminismEstimatedLostSlots()) / getDeterminismReferenceOpportunitySlots();
	}

	double PerformanceReport::getDomainIsolationBlockRate() const
	{
		return getDomainIsolationProbeAttempts() == 0 ? 0.0 : static_cast<double>(getDomainIsolationBlockedAttempts()) / getDomainIsolationProbeAttempts();
	}

	bool PerformanceReport::getHasPhase5OracleGapTelemetry() const
	{
		return getOracleGapTotalCyclesAnalyzed() > 0;
	}

	bool PerformanceReport::getHasStreamIngressWarmTelemetry() const
	{
		return getL1BypassHits() > 0 || getForegroundWarmAttempts() > 0 || getForegroundWarmSuccesses() > 0 || getForegroundWarmReuseHits() > 0 || getForegroundBypassHits() > 0 || getAssistWarmAttempts() > 0 || getAssistWarmSuccesses() > 0 || getAssistWarmReuseHits() > 0 || getAssistBypassHits() > 0 || getStreamWarmTranslationRejects() > 0 || getStreamWarmBackendRejects() > 0 || getAssistWarmResidentBudgetRejects() > 0 || getAssistWarmLoadingBudgetRejects() > 0 || getAssistWarmNoVictimRejects() > 0;
	}

	double PerformanceReport::getOracleEfficiency() const
	{
		return getOracleGapTotalOracleSlots() == 0 ? 1.0 : static_cast<double>(getOracleGapTotalRealSlots()) / getOracleGapTotalOracleSlots();
	}

	double PerformanceReport::getOracleGapRate() const
	{
		return getOracleGapTotalOracleSlots() == 0 ? 0.0 : static_cast<double>(getOracleGapTotalOracleSlots() - getOracleGapTotalRealSlots()) / getOracleGapTotalOracleSlots();
	}

	long long PerformanceReport::getStaticClassOvercommitRejects() const
	{
		return StaticClassOvercommitRejects;
	}

	void PerformanceReport::setStaticClassOvercommitRejects(long long value)
	{
		StaticClassOvercommitRejects = value;
	}

	long long PerformanceReport::getDynamicClassExhaustionRejects() const
	{
		return DynamicClassExhaustionRejects;
	}

	void PerformanceReport::setDynamicClassExhaustionRejects(long long value)
	{
		DynamicClassExhaustionRejects = value;
	}

	long long PerformanceReport::getPinnedLaneConflicts() const
	{
		return PinnedLaneConflicts;
	}

	void PerformanceReport::setPinnedLaneConflicts(long long value)
	{
		PinnedLaneConflicts = value;
	}

	long long PerformanceReport::getLateBindingConflicts() const
	{
		return LateBindingConflicts;
	}

	void PerformanceReport::setLateBindingConflicts(long long value)
	{
		LateBindingConflicts = value;
	}

	long long PerformanceReport::getTypedSlotDomainRejects() const
	{
		return TypedSlotDomainRejects;
	}

	void PerformanceReport::setTypedSlotDomainRejects(long long value)
	{
		TypedSlotDomainRejects = value;
	}

	long long PerformanceReport::getSmtOwnerContextGuardRejects() const
	{
		return SmtOwnerContextGuardRejects;
	}

	void PerformanceReport::setSmtOwnerContextGuardRejects(long long value)
	{
		SmtOwnerContextGuardRejects = value;
	}

	long long PerformanceReport::getSmtDomainGuardRejects() const
	{
		return SmtDomainGuardRejects;
	}

	void PerformanceReport::setSmtDomainGuardRejects(long long value)
	{
		SmtDomainGuardRejects = value;
	}

	long long PerformanceReport::getSmtBoundaryGuardRejects() const
	{
		return SmtBoundaryGuardRejects;
	}

	void PerformanceReport::setSmtBoundaryGuardRejects(long long value)
	{
		SmtBoundaryGuardRejects = value;
	}

	long long PerformanceReport::getSmtSharedResourceCertificateRejects() const
	{
		return SmtSharedResourceCertificateRejects;
	}

	void PerformanceReport::setSmtSharedResourceCertificateRejects(long long value)
	{
		SmtSharedResourceCertificateRejects = value;
	}

	long long PerformanceReport::getSmtRegisterGroupCertificateRejects() const
	{
		return SmtRegisterGroupCertificateRejects;
	}

	void PerformanceReport::setSmtRegisterGroupCertificateRejects(long long value)
	{
		SmtRegisterGroupCertificateRejects = value;
	}

	Core::RejectKind PerformanceReport::getLastSmtLegalityRejectKind() const
	{
		return LastSmtLegalityRejectKind;
	}

	void PerformanceReport::setLastSmtLegalityRejectKind(Core::RejectKind value)
	{
		LastSmtLegalityRejectKind = value;
	}

	Core::LegalityAuthoritySource PerformanceReport::getLastSmtLegalityAuthoritySource() const
	{
		return LastSmtLegalityAuthoritySource;
	}

	void PerformanceReport::setLastSmtLegalityAuthoritySource(Core::LegalityAuthoritySource value)
	{
		LastSmtLegalityAuthoritySource = value;
	}

	long long PerformanceReport::getSmtLegalityRejectByAluClass() const
	{
		return SmtLegalityRejectByAluClass;
	}

	void PerformanceReport::setSmtLegalityRejectByAluClass(long long value)
	{
		SmtLegalityRejectByAluClass = value;
	}

	long long PerformanceReport::getSmtLegalityRejectByLsuClass() const
	{
		return SmtLegalityRejectByLsuClass;
	}

	void PerformanceReport::setSmtLegalityRejectByLsuClass(long long value)
	{
		SmtLegalityRejectByLsuClass = value;
	}

	long long PerformanceReport::getSmtLegalityRejectByDmaStreamClass() const
	{
		return SmtLegalityRejectByDmaStreamClass;
	}

	void PerformanceReport::setSmtLegalityRejectByDmaStreamClass(long long value)
	{
		SmtLegalityRejectByDmaStreamClass = value;
	}

	long long PerformanceReport::getSmtLegalityRejectByMatrixTileStreamClass() const
	{
		return SmtLegalityRejectByMatrixTileStreamClass;
	}

	void PerformanceReport::setSmtLegalityRejectByMatrixTileStreamClass(long long value)
	{
		SmtLegalityRejectByMatrixTileStreamClass = value;
	}

	long long PerformanceReport::getSmtLegalityRejectByBranchControl() const
	{
		return SmtLegalityRejectByBranchControl;
	}

	void PerformanceReport::setSmtLegalityRejectByBranchControl(long long value)
	{
		SmtLegalityRejectByBranchControl = value;
	}

	long long PerformanceReport::getSmtLegalityRejectBySystemSingleton() const
	{
		return SmtLegalityRejectBySystemSingleton;
	}

	void PerformanceReport::setSmtLegalityRejectBySystemSingleton(long long value)
	{
		SmtLegalityRejectBySystemSingleton = value;
	}

	long long PerformanceReport::getTypedSlotFastPathAccepts() const
	{
		return TypedSlotFastPathAccepts;
	}

	void PerformanceReport::setTypedSlotFastPathAccepts(long long value)
	{
		TypedSlotFastPathAccepts = value;
	}

	long long PerformanceReport::getTypedSlotStandardPathAccepts() const
	{
		return TypedSlotStandardPathAccepts;
	}

	void PerformanceReport::setTypedSlotStandardPathAccepts(long long value)
	{
		TypedSlotStandardPathAccepts = value;
	}

	long long PerformanceReport::getClassTemplateReuseHits() const
	{
		return ClassTemplateReuseHits;
	}

	void PerformanceReport::setClassTemplateReuseHits(long long value)
	{
		ClassTemplateReuseHits = value;
	}

	long long PerformanceReport::getClassTemplateInvalidations() const
	{
		return ClassTemplateInvalidations;
	}

	void PerformanceReport::setClassTemplateInvalidations(long long value)
	{
		ClassTemplateInvalidations = value;
	}

	long long PerformanceReport::getClassTemplateDomainInvalidations() const
	{
		return ClassTemplateDomainInvalidations;
	}

	void PerformanceReport::setClassTemplateDomainInvalidations(long long value)
	{
		ClassTemplateDomainInvalidations = value;
	}

	long long PerformanceReport::getClassTemplateCapacityMismatchInvalidations() const
	{
		return ClassTemplateCapacityMismatchInvalidations;
	}

	void PerformanceReport::setClassTemplateCapacityMismatchInvalidations(long long value)
	{
		ClassTemplateCapacityMismatchInvalidations = value;
	}

	long long PerformanceReport::getLaneReuseHits() const
	{
		return LaneReuseHits;
	}

	void PerformanceReport::setLaneReuseHits(long long value)
	{
		LaneReuseHits = value;
	}

	long long PerformanceReport::getLaneReuseMisses() const
	{
		return LaneReuseMisses;
	}

	void PerformanceReport::setLaneReuseMisses(long long value)
	{
		LaneReuseMisses = value;
	}

	long long PerformanceReport::getTotalLaneBindings() const
	{
		return TotalLaneBindings;
	}

	void PerformanceReport::setTotalLaneBindings(long long value)
	{
		TotalLaneBindings = value;
	}

	long long PerformanceReport::getNopDueToPinnedConstraint() const
	{
		return NopDueToPinnedConstraint;
	}

	void PerformanceReport::setNopDueToPinnedConstraint(long long value)
	{
		NopDueToPinnedConstraint = value;
	}

	long long PerformanceReport::getNopDueToNoClassCapacity() const
	{
		return NopDueToNoClassCapacity;
	}

	void PerformanceReport::setNopDueToNoClassCapacity(long long value)
	{
		NopDueToNoClassCapacity = value;
	}

	long long PerformanceReport::getNopDueToResourceConflict() const
	{
		return NopDueToResourceConflict;
	}

	void PerformanceReport::setNopDueToResourceConflict(long long value)
	{
		NopDueToResourceConflict = value;
	}

	long long PerformanceReport::getNopDueToDynamicState() const
	{
		return NopDueToDynamicState;
	}

	void PerformanceReport::setNopDueToDynamicState(long long value)
	{
		NopDueToDynamicState = value;
	}

	long long PerformanceReport::getNopAvoided() const
	{
		return NopAvoided;
	}

	void PerformanceReport::setNopAvoided(long long value)
	{
		NopAvoided = value;
	}

	long long PerformanceReport::getAluClassInjects() const
	{
		return AluClassInjects;
	}

	void PerformanceReport::setAluClassInjects(long long value)
	{
		AluClassInjects = value;
	}

	long long PerformanceReport::getLsuClassInjects() const
	{
		return LsuClassInjects;
	}

	void PerformanceReport::setLsuClassInjects(long long value)
	{
		LsuClassInjects = value;
	}

	long long PerformanceReport::getDmaStreamClassInjects() const
	{
		return DmaStreamClassInjects;
	}

	void PerformanceReport::setDmaStreamClassInjects(long long value)
	{
		DmaStreamClassInjects = value;
	}

	long long PerformanceReport::getMatrixTileStreamClassInjects() const
	{
		return MatrixTileStreamClassInjects;
	}

	void PerformanceReport::setMatrixTileStreamClassInjects(long long value)
	{
		MatrixTileStreamClassInjects = value;
	}

	long long PerformanceReport::getBranchControlInjects() const
	{
		return BranchControlInjects;
	}

	void PerformanceReport::setBranchControlInjects(long long value)
	{
		BranchControlInjects = value;
	}

	long long PerformanceReport::getHardPinnedInjects() const
	{
		return HardPinnedInjects;
	}

	void PerformanceReport::setHardPinnedInjects(long long value)
	{
		HardPinnedInjects = value;
	}

	long long PerformanceReport::getClassFlexibleInjects() const
	{
		return ClassFlexibleInjects;
	}

	void PerformanceReport::setClassFlexibleInjects(long long value)
	{
		ClassFlexibleInjects = value;
	}

	double PerformanceReport::getClassCapacityRejectRate() const
	{
		long long total = getStaticClassOvercommitRejects() + getDynamicClassExhaustionRejects() + getPinnedLaneConflicts() + getLateBindingConflicts();
		return total > 0 ? static_cast<double>(getStaticClassOvercommitRejects() + getDynamicClassExhaustionRejects()) / total : 0.0;
	}

	double PerformanceReport::getLaneReuseRate() const
	{
		return getTotalLaneBindings() > 0 ? static_cast<double>(getLaneReuseHits()) / getTotalLaneBindings() : 0.0;
	}

	double PerformanceReport::getClassFlexibleRatio() const
	{
		return (getClassFlexibleInjects() + getHardPinnedInjects()) > 0 ? static_cast<double>(getClassFlexibleInjects()) / (getClassFlexibleInjects() + getHardPinnedInjects()) : 0.0;
	}

	double PerformanceReport::getNopReductionRate() const
	{
		return (getNopAvoided() + getNopDueToPinnedConstraint() + getNopDueToNoClassCapacity()) > 0 ? static_cast<double>(getNopAvoided()) / (getNopAvoided() + getNopDueToPinnedConstraint() + getNopDueToNoClassCapacity()) : 0.0;
	}

	bool PerformanceReport::getHasPhase8TypedSlotTelemetry() const
	{
		return getTotalLaneBindings() > 0 || getStaticClassOvercommitRejects() > 0 || getDynamicClassExhaustionRejects() > 0 || getPinnedLaneConflicts() > 0 || getLateBindingConflicts() > 0 || getSmtOwnerContextGuardRejects() > 0 || getSmtDomainGuardRejects() > 0 || getSmtBoundaryGuardRejects() > 0 || getSmtSharedResourceCertificateRejects() > 0 || getSmtRegisterGroupCertificateRejects() > 0 || getSmtLegalityRejectByAluClass() > 0 || getSmtLegalityRejectByLsuClass() > 0 || getSmtLegalityRejectByDmaStreamClass() > 0 || getSmtLegalityRejectByMatrixTileStreamClass() > 0 || getSmtLegalityRejectByBranchControl() > 0 || getSmtLegalityRejectBySystemSingleton() > 0 || getNopAvoided() > 0;
	}
}
