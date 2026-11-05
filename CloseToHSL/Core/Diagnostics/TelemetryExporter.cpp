#include "TelemetryExporter.h"
#include "TelemetryExporter.Json.h"
#include <limits>
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Diagnostics
{
	namespace {
		std::wstring RejectKindText(YAKSys_Hybrid_CPU::Core::RejectKind value) { using R=YAKSys_Hybrid_CPU::Core::RejectKind; switch(value){case R::None:return L"None";case R::Boundary:return L"Boundary";case R::OwnerMismatch:return L"OwnerMismatch";case R::DomainMismatch:return L"DomainMismatch";case R::EpochMismatch:return L"EpochMismatch";case R::ClassCapacity:return L"ClassCapacity";case R::LaneUnavailable:return L"LaneUnavailable";case R::CrossLaneConflict:return L"CrossLaneConflict";case R::RareHazard:return L"RareHazard";case R::Ordering:return L"Ordering";case R::CertificateStale:return L"CertificateStale";default:return L"Unknown";} }
		std::wstring AuthorityText(YAKSys_Hybrid_CPU::Core::LegalityAuthoritySource value) { using A=YAKSys_Hybrid_CPU::Core::LegalityAuthoritySource; switch(value){case A::StructuralCertificate:return L"StructuralCertificate";case A::ReplayPhaseCertificate:return L"ReplayPhaseCertificate";case A::GuardPlane:return L"GuardPlane";case A::DetailedCompatibilityCheck:return L"DetailedCompatibilityCheck";case A::AdmissionMetadataStructuralCheck:return L"AdmissionMetadataStructuralCheck";default:return L"Unknown";} }

		std::optional<long long> PositiveCheckedLong(unsigned long long value)
		{
			if (value == 0)
			{
				return std::nullopt;
			}
			if (value > static_cast<unsigned long long>((std::numeric_limits<long long>::max)()))
			{
				throw std::overflow_error("telemetry value exceeds Int64");
			}
			return std::optional<long long>{static_cast<long long>(value)};
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null)
	std::shared_ptr<TypedSlotTelemetryProfile> TelemetryExporter::BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics)
	{
		return BuildProfile(scheduler, programHash, std::nullopt, workerMetrics, std::nullopt, std::nullopt);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, DmaStreamComputeTelemetrySnapshot dmaStreamComputeTelemetry, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null)
	std::shared_ptr<TypedSlotTelemetryProfile> TelemetryExporter::BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::shared_ptr<DmaStreamComputeTelemetrySnapshot> dmaStreamComputeTelemetry, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics)
	{
		return BuildProfile(scheduler, programHash, std::nullopt, workerMetrics, dmaStreamComputeTelemetry, std::nullopt);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, AcceleratorTelemetrySnapshot acceleratorTelemetry, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null)
	std::shared_ptr<TypedSlotTelemetryProfile> TelemetryExporter::BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::shared_ptr<AcceleratorTelemetrySnapshot> acceleratorTelemetry, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics)
	{
		return BuildProfile(scheduler, programHash, std::nullopt, workerMetrics, std::nullopt, acceleratorTelemetry);
	}

	std::shared_ptr<AcceleratorTelemetrySnapshot> TelemetryExporter::ExportAcceleratorTelemetry(std::shared_ptr<AcceleratorTelemetry> telemetry)
	{
		ArgumentNullException::ThrowIfNull(telemetry);
		return telemetry->Snapshot();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, System.Nullable<Processor.CPU_Core.PipelineControl> pipelineControl, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null, System.Nullable<DmaStreamComputeTelemetrySnapshot> dmaStreamComputeTelemetry = null, System.Nullable<AcceleratorTelemetrySnapshot> acceleratorTelemetry = null)
	std::shared_ptr<TypedSlotTelemetryProfile> TelemetryExporter::BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::optional<Processor::CPU_Core::PipelineControl> pipelineControl, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics, std::optional<std::shared_ptr<DmaStreamComputeTelemetrySnapshot>> dmaStreamComputeTelemetry, std::optional<std::shared_ptr<AcceleratorTelemetrySnapshot>> acceleratorTelemetry)
	{
		ArgumentNullException::ThrowIfNull(scheduler);
		ArgumentNullException::ThrowIfNull(programHash);

		SchedulerPhaseMetrics metrics = scheduler->GetPhaseMetrics();
		std::unordered_map<SlotClass, long long> certificateRejectsPerClass = BuildCertificateRejectsPerClass(metrics);
		std::unordered_map<SlotClass, long long> temp_var = BuildSmtLegalityRejectsPerClass(metrics);
		std::unordered_map<SlotClass, long long> smtLegalityRejectsPerClass = temp_var.empty() ? certificateRejectsPerClass : temp_var;
		std::unordered_map<int, long long> perVtRejectionCounts = BuildPerVtRejectionCounts(metrics);
		std::unordered_map<int, long long> perVtRegGroupConflicts = BuildPerVtRegGroupConflicts(metrics);
		std::unordered_map<int, long long> bankPendingRejectsPerBank = BuildBankPendingRejectsPerBank(metrics);
		std::optional<long long> memoryClusteringEventCount = metrics.getMemoryClusteringEvents() > 0 ? std::optional<long long>{metrics.getMemoryClusteringEvents()} : std::nullopt;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> bankConflictStallCycles = pipelineControl is { BankConflictStallCycles: > 0UL } ? checked((long)pipelineControl.Value.BankConflictStallCycles) : null;
		std::optional<long long> bankConflictStallCycles;
		if (pipelineControl) bankConflictStallCycles = PositiveCheckedLong(pipelineControl->getBankConflictStallCycles());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> hazardRegisterDataCount = pipelineControl is { HazardRegisterDataCount: > 0UL } ? checked((long)pipelineControl.Value.HazardRegisterDataCount) : null;
		std::optional<long long> hazardRegisterDataCount;
		if (pipelineControl) hazardRegisterDataCount = PositiveCheckedLong(pipelineControl->getHazardRegisterDataCount());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> hazardMemoryBankCount = pipelineControl is { HazardMemoryBankCount: > 0UL } ? checked((long)pipelineControl.Value.HazardMemoryBankCount) : null;
		std::optional<long long> hazardMemoryBankCount;
		if (pipelineControl) hazardMemoryBankCount = PositiveCheckedLong(pipelineControl->getHazardMemoryBankCount());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> hazardControlFlowCount = pipelineControl is { HazardControlFlowCount: > 0UL } ? checked((long)pipelineControl.Value.HazardControlFlowCount) : null;
		std::optional<long long> hazardControlFlowCount;
		if (pipelineControl) hazardControlFlowCount = PositiveCheckedLong(pipelineControl->getHazardControlFlowCount());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> hazardSystemBarrierCount = pipelineControl is { HazardSystemBarrierCount: > 0UL } ? checked((long)pipelineControl.Value.HazardSystemBarrierCount) : null;
		std::optional<long long> hazardSystemBarrierCount;
		if (pipelineControl) hazardSystemBarrierCount = PositiveCheckedLong(pipelineControl->getHazardSystemBarrierCount());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> hazardPinnedLaneCount = pipelineControl is { HazardPinnedLaneCount: > 0UL } ? checked((long)pipelineControl.Value.HazardPinnedLaneCount) : null;
		std::optional<long long> hazardPinnedLaneCount;
		if (pipelineControl) hazardPinnedLaneCount = PositiveCheckedLong(pipelineControl->getHazardPinnedLaneCount());
		std::optional<long long> crossDomainRejectCount = metrics.getDomainIsolationCrossDomainBlocks() > 0 ? std::optional<long long>{metrics.getDomainIsolationCrossDomainBlocks()} : std::nullopt;
		bool hasEligibilityTelemetry = scheduler->getTotalSchedulerCycles() > 0 || metrics.getEligibilityMaskedCycles() > 0 || metrics.getEligibilityMaskedReadyCandidates() > 0 || metrics.getLastEligibilityRequestedMask() != 0 || metrics.getLastEligibilityNormalizedMask() != 0 || metrics.getLastEligibilityReadyPortMask() != 0 || metrics.getLastEligibilityVisibleReadyMask() != 0 || metrics.getLastEligibilityMaskedReadyMask() != 0;
		std::optional<long long> eligibilityMaskedCycles = hasEligibilityTelemetry ? std::optional<long long>{metrics.getEligibilityMaskedCycles()} : std::nullopt;
		std::optional<long long> eligibilityMaskedReadyCandidates = hasEligibilityTelemetry ? std::optional<long long>{metrics.getEligibilityMaskedReadyCandidates()} : std::nullopt;
		std::optional<unsigned char> lastEligibilityRequestedMask = hasEligibilityTelemetry ? std::optional<unsigned char>{metrics.getLastEligibilityRequestedMask()} : std::nullopt;
		std::optional<unsigned char> lastEligibilityNormalizedMask = hasEligibilityTelemetry ? std::optional<unsigned char>{metrics.getLastEligibilityNormalizedMask()} : std::nullopt;
		std::optional<unsigned char> lastEligibilityReadyPortMask = hasEligibilityTelemetry ? std::optional<unsigned char>{metrics.getLastEligibilityReadyPortMask()} : std::nullopt;
		std::optional<unsigned char> lastEligibilityVisibleReadyMask = hasEligibilityTelemetry ? std::optional<unsigned char>{metrics.getLastEligibilityVisibleReadyMask()} : std::nullopt;
		std::optional<unsigned char> lastEligibilityMaskedReadyMask = hasEligibilityTelemetry ? std::optional<unsigned char>{metrics.getLastEligibilityMaskedReadyMask()} : std::nullopt;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: IReadOnlyList<LoopPhaseClassProfile>? loopPhaseProfiles = metrics.LoopPhaseProfiles is { Count: > 0 } ? metrics.LoopPhaseProfiles : null;
		std::vector<std::shared_ptr<LoopPhaseClassProfile>> loopPhaseProfiles = metrics.getLoopPhaseProfiles();

		// Post-phase-05: heterogeneous retired width breakdown
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> scalarLanesRetired = pipelineControl is { ScalarLanesRetired: > 0UL } ? checked((long)pipelineControl.Value.ScalarLanesRetired) : null;
		std::optional<long long> scalarLanesRetired;
		if (pipelineControl) scalarLanesRetired = PositiveCheckedLong(pipelineControl->getScalarLanesRetired());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> nonScalarLanesRetired = pipelineControl is { NonScalarLanesRetired: > 0UL } ? checked((long)pipelineControl.Value.NonScalarLanesRetired) : null;
		std::optional<long long> nonScalarLanesRetired;
		if (pipelineControl) nonScalarLanesRetired = PositiveCheckedLong(pipelineControl->getNonScalarLanesRetired());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: System.Nullable<long> retireCycleCount = pipelineControl is { RetireCycleCount: > 0UL } ? checked((long)pipelineControl.Value.RetireCycleCount) : null;
		std::optional<long long> retireCycleCount;
		if (pipelineControl) retireCycleCount = PositiveCheckedLong(pipelineControl->getRetireCycleCount());
		std::optional<double> scalarIPC;
		if (pipelineControl) scalarIPC = NullIfZero(pipelineControl->GetScalarIPC());
		std::optional<double> averageRetiredWidth;
		if (pipelineControl) averageRetiredWidth = NullIfZero(pipelineControl->GetAverageRetiredWidth());

		// Per-class injection counts
		std::unordered_map<SlotClass, long long> injectionsPerClass =
		{
			{SlotClass::AluClass, metrics.getAluClassInjects()},
			{SlotClass::LsuClass, metrics.getLsuClassInjects()},
			{SlotClass::DmaStreamClass, metrics.getDmaStreamClassInjects()},
			{SlotClass::MatrixTileStreamClass, metrics.getMatrixTileStreamClassInjects()},
			{SlotClass::BranchControl, metrics.getBranchControlInjects()},
			{SlotClass::SystemSingleton, 0}
		};

		// Per-class reject counts (derived from disaggregated counters)
		std::unordered_map<SlotClass, long long> rejectsPerClass =
		{
			{SlotClass::AluClass, GetValueOrDefault(smtLegalityRejectsPerClass, SlotClass::AluClass)},
			{SlotClass::LsuClass, GetValueOrDefault(smtLegalityRejectsPerClass, SlotClass::LsuClass)},
			{SlotClass::DmaStreamClass, GetValueOrDefault(smtLegalityRejectsPerClass, SlotClass::DmaStreamClass)},
			{SlotClass::MatrixTileStreamClass, GetValueOrDefault(smtLegalityRejectsPerClass, SlotClass::MatrixTileStreamClass)},
			{SlotClass::BranchControl, GetValueOrDefault(smtLegalityRejectsPerClass, SlotClass::BranchControl)},
			{SlotClass::SystemSingleton, GetValueOrDefault(smtLegalityRejectsPerClass, SlotClass::SystemSingleton)}
		};

		// Rejects by reason
		std::unordered_map<TypedSlotRejectReason, long long> rejectsByReason =
		{
			{TypedSlotRejectReason::StaticClassOvercommit, metrics.getStaticClassOvercommitRejects()},
			{TypedSlotRejectReason::DynamicClassExhaustion, metrics.getDynamicClassExhaustionRejects()},
			{TypedSlotRejectReason::ResourceConflict, scheduler->getTypedSlotResourceConflictRejects()},
			{TypedSlotRejectReason::DomainReject, metrics.getTypedSlotDomainRejects()},
			{TypedSlotRejectReason::ScoreboardReject, scheduler->getTypedSlotScoreboardRejects()},
			{TypedSlotRejectReason::BankPendingReject, scheduler->getTypedSlotBankPendingRejects()},
			{TypedSlotRejectReason::HardwareBudgetReject, scheduler->getTypedSlotHardwareBudgetRejects()},
			{TypedSlotRejectReason::SpeculationBudgetReject, scheduler->getTypedSlotSpeculationBudgetRejects()},
			{TypedSlotRejectReason::AssistQuotaReject, scheduler->getTypedSlotAssistQuotaRejects()},
			{TypedSlotRejectReason::AssistBackpressureReject, scheduler->getTypedSlotAssistBackpressureRejects()},
			{TypedSlotRejectReason::PinnedLaneConflict, metrics.getPinnedLaneConflicts()},
			{TypedSlotRejectReason::InvalidPinnedLane, scheduler->getInvalidPinnedLaneRejects()},
			{TypedSlotRejectReason::LateBindingConflict, metrics.getLateBindingConflicts()}
		};

		// NOP density computation
		long long totalNops = metrics.getNopDueToPinnedConstraint() + metrics.getNopDueToNoClassCapacity() + metrics.getNopDueToResourceConflict() + metrics.getNopDueToDynamicState();

		long long totalBundles = scheduler->getTotalSchedulerCycles();
		constexpr int bundleWidth = 8;
		double nopDensity = totalBundles > 0 ? static_cast<double>(totalNops) / (totalBundles * bundleWidth) : 0.0;
		double utilization = 1.0 - nopDensity;

		// Replay hit rate
		long long replayHits = metrics.getClassTemplateReuseHits();
		long long replayMisses = metrics.getClassTemplateInvalidations();
		double replayHitRate = (replayHits + replayMisses) > 0 ? static_cast<double>(replayHits) / (replayHits + replayMisses) : 0.0;

		// Per-VT injection counts
		std::unordered_map<int, long long> perVtInjections(SmtWays);
		for (int vt = 0; vt < SmtWays; vt++)
		{
			perVtInjections[vt] = scheduler->GetPerVtInjectionCount(vt);
		}

		std::optional<std::shared_ptr<CertificatePressureMetrics>> certificatePressure;
		if (!certificateRejectsPerClass.empty() || !perVtRegGroupConflicts.empty())
		{
			std::optional<std::unordered_map<SlotClass, long long>> certificateRejects;
			if (!certificateRejectsPerClass.empty()) certificateRejects = certificateRejectsPerClass;
			std::optional<std::unordered_map<int, long long>> registerConflicts;
			if (!perVtRegGroupConflicts.empty()) registerConflicts = perVtRegGroupConflicts;
			certificatePressure = std::make_shared<CertificatePressureMetrics>(std::move(certificateRejects), std::move(registerConflicts));
		}

		std::shared_ptr<TypedSlotTelemetryProfile> temp_var2 = std::make_shared<TypedSlotTelemetryProfile>(programHash, injectionsPerClass, rejectsPerClass, rejectsByReason, nopDensity, utilization, totalBundles, totalNops, replayHits, replayMisses, replayHitRate, scheduler->getFairnessStarvationEvents(), perVtInjections, workerMetrics);
		temp_var2->CertificatePressure = certificatePressure;
		if (!perVtRejectionCounts.empty()) temp_var2->PerVtRejectionCounts = perVtRejectionCounts;
		if (!perVtRegGroupConflicts.empty()) temp_var2->PerVtRegGroupConflicts = perVtRegGroupConflicts;
		if (!smtLegalityRejectsPerClass.empty()) temp_var2->SmtLegalityRejectsPerClass = smtLegalityRejectsPerClass;
		if (!bankPendingRejectsPerBank.empty()) temp_var2->BankPendingRejectsPerBank = bankPendingRejectsPerBank;
		temp_var2->MemoryClusteringEventCount = memoryClusteringEventCount;
		temp_var2->BankConflictStallCycles = bankConflictStallCycles;
		temp_var2->HazardRegisterDataCount = hazardRegisterDataCount;
		temp_var2->HazardMemoryBankCount = hazardMemoryBankCount;
		temp_var2->HazardControlFlowCount = hazardControlFlowCount;
		temp_var2->HazardSystemBarrierCount = hazardSystemBarrierCount;
		temp_var2->HazardPinnedLaneCount = hazardPinnedLaneCount;
		temp_var2->CrossDomainRejectCount = crossDomainRejectCount;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
		temp_var2->LastSmtLegalityRejectKind = RejectKindText(metrics.getLastSmtLegalityRejectKind());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
		temp_var2->LastSmtLegalityAuthoritySource = AuthorityText(metrics.getLastSmtLegalityAuthoritySource());
		temp_var2->SmtOwnerContextGuardRejects = metrics.getSmtOwnerContextGuardRejects();
		temp_var2->SmtDomainGuardRejects = metrics.getSmtDomainGuardRejects();
		temp_var2->SmtBoundaryGuardRejects = metrics.getSmtBoundaryGuardRejects();
		temp_var2->SmtSharedResourceCertificateRejects = metrics.getSmtSharedResourceCertificateRejects();
		temp_var2->SmtRegisterGroupCertificateRejects = metrics.getSmtRegisterGroupCertificateRejects();
		temp_var2->EligibilityMaskedCycles = eligibilityMaskedCycles;
		temp_var2->EligibilityMaskedReadyCandidates = eligibilityMaskedReadyCandidates;
		temp_var2->LastEligibilityRequestedMask = lastEligibilityRequestedMask;
		temp_var2->LastEligibilityNormalizedMask = lastEligibilityNormalizedMask;
		temp_var2->LastEligibilityReadyPortMask = lastEligibilityReadyPortMask;
		temp_var2->LastEligibilityVisibleReadyMask = lastEligibilityVisibleReadyMask;
		temp_var2->LastEligibilityMaskedReadyMask = lastEligibilityMaskedReadyMask;
		if (!loopPhaseProfiles.empty()) temp_var2->LoopPhaseProfiles = loopPhaseProfiles;
		temp_var2->DmaStreamComputeTelemetry = dmaStreamComputeTelemetry;
		temp_var2->AcceleratorTelemetry = acceleratorTelemetry;
		temp_var2->ScalarLanesRetired = scalarLanesRetired;
		temp_var2->NonScalarLanesRetired = nonScalarLanesRetired;
		temp_var2->RetireCycleCount = retireCycleCount;
		temp_var2->ScalarIPC = scalarIPC;
		temp_var2->AverageRetiredWidth = averageRetiredWidth;
		return temp_var2;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<SlotClass, long>? BuildCertificateRejectsPerClass(SchedulerPhaseMetrics metrics)
	std::unordered_map<SlotClass, long long> TelemetryExporter::BuildCertificateRejectsPerClass(SchedulerPhaseMetrics metrics)
	{
		std::unordered_map<SlotClass, long long> rejectsPerClass(6);
		AddIfPositive(rejectsPerClass, SlotClass::AluClass, metrics.getCertificateRejectByAluClass());
		AddIfPositive(rejectsPerClass, SlotClass::LsuClass, metrics.getCertificateRejectByLsuClass());
		AddIfPositive(rejectsPerClass, SlotClass::DmaStreamClass, metrics.getCertificateRejectByDmaStreamClass());
		AddIfPositive(rejectsPerClass, SlotClass::MatrixTileStreamClass, metrics.getCertificateRejectByMatrixTileStreamClass());
		AddIfPositive(rejectsPerClass, SlotClass::BranchControl, metrics.getCertificateRejectByBranchControl());
		AddIfPositive(rejectsPerClass, SlotClass::SystemSingleton, metrics.getCertificateRejectBySystemSingleton());
		return rejectsPerClass.empty() ? std::unordered_map<SlotClass, long long>() : rejectsPerClass;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<SlotClass, long>? BuildSmtLegalityRejectsPerClass(SchedulerPhaseMetrics metrics)
	std::unordered_map<SlotClass, long long> TelemetryExporter::BuildSmtLegalityRejectsPerClass(SchedulerPhaseMetrics metrics)
	{
		std::unordered_map<SlotClass, long long> rejectsPerClass(6);
		AddIfPositive(rejectsPerClass, SlotClass::AluClass, metrics.getSmtLegalityRejectByAluClass());
		AddIfPositive(rejectsPerClass, SlotClass::LsuClass, metrics.getSmtLegalityRejectByLsuClass());
		AddIfPositive(rejectsPerClass, SlotClass::DmaStreamClass, metrics.getSmtLegalityRejectByDmaStreamClass());
		AddIfPositive(rejectsPerClass, SlotClass::MatrixTileStreamClass, metrics.getSmtLegalityRejectByMatrixTileStreamClass());
		AddIfPositive(rejectsPerClass, SlotClass::BranchControl, metrics.getSmtLegalityRejectByBranchControl());
		AddIfPositive(rejectsPerClass, SlotClass::SystemSingleton, metrics.getSmtLegalityRejectBySystemSingleton());
		return rejectsPerClass.empty() ? std::unordered_map<SlotClass, long long>() : rejectsPerClass;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, long>? BuildPerVtRejectionCounts(SchedulerPhaseMetrics metrics)
	std::unordered_map<int, long long> TelemetryExporter::BuildPerVtRejectionCounts(SchedulerPhaseMetrics metrics)
	{
		std::unordered_map<int, long long> rejectsPerVt(SmtWays);
		AddIfPositive(rejectsPerVt, 0, metrics.getRejectionsVT0());
		AddIfPositive(rejectsPerVt, 1, metrics.getRejectionsVT1());
		AddIfPositive(rejectsPerVt, 2, metrics.getRejectionsVT2());
		AddIfPositive(rejectsPerVt, 3, metrics.getRejectionsVT3());
		return rejectsPerVt.empty() ? std::unordered_map<int, long long>() : rejectsPerVt;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, long>? BuildPerVtRegGroupConflicts(SchedulerPhaseMetrics metrics)
	std::unordered_map<int, long long> TelemetryExporter::BuildPerVtRegGroupConflicts(SchedulerPhaseMetrics metrics)
	{
		std::unordered_map<int, long long> conflictsPerVt(SmtWays);
		AddIfPositive(conflictsPerVt, 0, metrics.getRegGroupConflictsVT0() != 0 ? metrics.getRegGroupConflictsVT0() : metrics.getCertificateRegGroupConflictVT0());
		AddIfPositive(conflictsPerVt, 1, metrics.getRegGroupConflictsVT1() != 0 ? metrics.getRegGroupConflictsVT1() : metrics.getCertificateRegGroupConflictVT1());
		AddIfPositive(conflictsPerVt, 2, metrics.getRegGroupConflictsVT2() != 0 ? metrics.getRegGroupConflictsVT2() : metrics.getCertificateRegGroupConflictVT2());
		AddIfPositive(conflictsPerVt, 3, metrics.getRegGroupConflictsVT3() != 0 ? metrics.getRegGroupConflictsVT3() : metrics.getCertificateRegGroupConflictVT3());
		return conflictsPerVt.empty() ? std::unordered_map<int, long long>() : conflictsPerVt;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, long>? BuildBankPendingRejectsPerBank(SchedulerPhaseMetrics metrics)
	std::unordered_map<int, long long> TelemetryExporter::BuildBankPendingRejectsPerBank(SchedulerPhaseMetrics metrics)
	{
		std::unordered_map<int, long long> rejectsPerBank(16);
		AddIfPositive(rejectsPerBank, 0, metrics.getBankPendingRejectBank0());
		AddIfPositive(rejectsPerBank, 1, metrics.getBankPendingRejectBank1());
		AddIfPositive(rejectsPerBank, 2, metrics.getBankPendingRejectBank2());
		AddIfPositive(rejectsPerBank, 3, metrics.getBankPendingRejectBank3());
		AddIfPositive(rejectsPerBank, 4, metrics.getBankPendingRejectBank4());
		AddIfPositive(rejectsPerBank, 5, metrics.getBankPendingRejectBank5());
		AddIfPositive(rejectsPerBank, 6, metrics.getBankPendingRejectBank6());
		AddIfPositive(rejectsPerBank, 7, metrics.getBankPendingRejectBank7());
		AddIfPositive(rejectsPerBank, 8, metrics.getBankPendingRejectBank8());
		AddIfPositive(rejectsPerBank, 9, metrics.getBankPendingRejectBank9());
		AddIfPositive(rejectsPerBank, 10, metrics.getBankPendingRejectBank10());
		AddIfPositive(rejectsPerBank, 11, metrics.getBankPendingRejectBank11());
		AddIfPositive(rejectsPerBank, 12, metrics.getBankPendingRejectBank12());
		AddIfPositive(rejectsPerBank, 13, metrics.getBankPendingRejectBank13());
		AddIfPositive(rejectsPerBank, 14, metrics.getBankPendingRejectBank14());
		AddIfPositive(rejectsPerBank, 15, metrics.getBankPendingRejectBank15());
		return rejectsPerBank.empty() ? std::unordered_map<int, long long>() : rejectsPerBank;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static long GetValueOrDefault(IReadOnlyDictionary<SlotClass, long>? values, SlotClass slotClass)
	long long TelemetryExporter::GetValueOrDefault(const std::unordered_map<SlotClass, long long> &values, SlotClass slotClass)
	{
		const auto it = values.find(slotClass);
		return it == values.end() ? 0 : it->second;
	}

	std::optional<double> TelemetryExporter::NullIfZero(double value)
	{
		if (value <= 0.0) return std::nullopt;
		return std::optional<double>{value};
	}

	std::wstring TelemetryExporter::SerializeToJson(std::shared_ptr<TypedSlotTelemetryProfile> profile)
	{
		ArgumentNullException::ThrowIfNull(profile);
		return SerializeTelemetryProfile(*profile);
	}

	std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> TelemetryExporter::DeserializeFromJson(const std::wstring &json)
	{
		if (StringHelper::isEmptyOrWhiteSpace(json))
		{
			return std::nullopt;
		}

		try
		{
			auto profile = DeserializeTelemetryProfile(json);
			if (!profile) return std::nullopt;
			return profile;
		}
		catch (const std::exception &)
		{
			return std::nullopt;
		}
	}
}
