#include "IseObservationService.h"
#include "../Arch/CsrAddresses.h"
#include <algorithm>

using namespace HybridCPU_ISE::Machine;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE
{
}

namespace HybridCPU_ISE::Machine
{

	CpuCoreDiagnosticSnapshot::CpuCoreDiagnosticSnapshot(unsigned int coreId, std::vector<unsigned long long> &committedPcs, std::vector<unsigned long long> &architecturalRegisters, std::vector<bool> &virtualThreadStalled, std::vector<PipelineState> &virtualThreadPipelineStates, std::vector<Processor::CPU_Core::FPExceptionContext> &threadFpContexts, Processor::CPU_Core::FlagsRegister coreFlagsRegister, int callStackDepth, unsigned long long callStackTop, int interruptStackDepth, unsigned long long interruptStackTop, std::vector<unsigned int> &grlbBanks, unsigned long long structuralStalls, bool hasExceptions, int l1VliwBundleCount, int l1DataEntryCount, int l2VliwBundleCount, int l2DataEntryCount, unsigned long long powerStateCsrValue, unsigned long long performanceLevelCsrValue, unsigned long long vectorLength, unsigned char vectorTailAgnostic, unsigned char vectorMaskAgnostic, Processor::CPU_Core::PipelineObservationSnapshot pipelineObservation, YAKSys_Hybrid_CPU::Core::ReplayPhaseMetrics replayPhaseMetrics, YAKSys_Hybrid_CPU::Core::SchedulerPhaseMetrics schedulerPhaseMetrics) : CoreId(coreId), CoreFlagsRegister(coreFlagsRegister), CallStackDepth(callStackDepth), CallStackTop(callStackTop), InterruptStackDepth(interruptStackDepth), InterruptStackTop(interruptStackTop), StructuralStalls(structuralStalls), HasExceptions(hasExceptions), L1VliwBundleCount(l1VliwBundleCount), L1DataEntryCount(l1DataEntryCount), L2VliwBundleCount(l2VliwBundleCount), L2DataEntryCount(l2DataEntryCount), PowerStateCsrValue(powerStateCsrValue), PerformanceLevelCsrValue(performanceLevelCsrValue), VectorLength(vectorLength), VectorTailAgnostic(vectorTailAgnostic), VectorMaskAgnostic(vectorMaskAgnostic), ReplayPhaseMetrics(replayPhaseMetrics), SchedulerPhaseMetrics(schedulerPhaseMetrics), _committedPcs(committedPcs), _architecturalRegisters(architecturalRegisters), _virtualThreadStalled(virtualThreadStalled), _virtualThreadPipelineStates(virtualThreadPipelineStates), _threadFpContexts(threadFpContexts), _grlbBanks(grlbBanks), _pipelineObservation(std::move(pipelineObservation))
	{
	}

	unsigned int CpuCoreDiagnosticSnapshot::getCoreId() const
	{
		return CoreId;
	}

	Processor::CPU_Core::FlagsRegister CpuCoreDiagnosticSnapshot::getCoreFlagsRegister() const
	{
		return CoreFlagsRegister;
	}

	int CpuCoreDiagnosticSnapshot::getCallStackDepth() const
	{
		return CallStackDepth;
	}

	unsigned long long CpuCoreDiagnosticSnapshot::getCallStackTop() const
	{
		return CallStackTop;
	}

	int CpuCoreDiagnosticSnapshot::getInterruptStackDepth() const
	{
		return InterruptStackDepth;
	}

	unsigned long long CpuCoreDiagnosticSnapshot::getInterruptStackTop() const
	{
		return InterruptStackTop;
	}

	unsigned long long CpuCoreDiagnosticSnapshot::getStructuralStalls() const
	{
		return StructuralStalls;
	}

	bool CpuCoreDiagnosticSnapshot::getHasExceptions() const
	{
		return HasExceptions;
	}

	int CpuCoreDiagnosticSnapshot::getL1VliwBundleCount() const
	{
		return L1VliwBundleCount;
	}

	int CpuCoreDiagnosticSnapshot::getL1DataEntryCount() const
	{
		return L1DataEntryCount;
	}

	int CpuCoreDiagnosticSnapshot::getL2VliwBundleCount() const
	{
		return L2VliwBundleCount;
	}

	int CpuCoreDiagnosticSnapshot::getL2DataEntryCount() const
	{
		return L2DataEntryCount;
	}

	unsigned long long CpuCoreDiagnosticSnapshot::getPowerStateCsrValue() const
	{
		return PowerStateCsrValue;
	}

	unsigned long long CpuCoreDiagnosticSnapshot::getPerformanceLevelCsrValue() const
	{
		return PerformanceLevelCsrValue;
	}

	unsigned long long CpuCoreDiagnosticSnapshot::getVectorLength() const
	{
		return VectorLength;
	}

	unsigned char CpuCoreDiagnosticSnapshot::getVectorTailAgnostic() const
	{
		return VectorTailAgnostic;
	}

	unsigned char CpuCoreDiagnosticSnapshot::getVectorMaskAgnostic() const
	{
		return VectorMaskAgnostic;
	}

	YAKSys_Hybrid_CPU::Core::ReplayPhaseMetrics CpuCoreDiagnosticSnapshot::getReplayPhaseMetrics() const
	{
		return ReplayPhaseMetrics;
	}

	YAKSys_Hybrid_CPU::Core::SchedulerPhaseMetrics CpuCoreDiagnosticSnapshot::getSchedulerPhaseMetrics() const
	{
		return SchedulerPhaseMetrics;
	}

	unsigned long long CpuCoreDiagnosticSnapshot::ReadActiveLivePc()
	{
		return _pipelineObservation.getActiveLivePc();
	}

	int CpuCoreDiagnosticSnapshot::ReadActiveVirtualThreadId()
	{
		return _pipelineObservation.getActiveVirtualThreadId();
	}

	Processor::CPU_Core::PipelineControl CpuCoreDiagnosticSnapshot::GetPipelineControl()
	{
		return _pipelineObservation.getPipelineControl();
	}

	YAKSys_Hybrid_CPU::Core::ReplayPhaseMetrics CpuCoreDiagnosticSnapshot::GetReplayPhaseMetrics()
	{
		return getReplayPhaseMetrics();
	}

	YAKSys_Hybrid_CPU::Core::SchedulerPhaseMetrics CpuCoreDiagnosticSnapshot::GetSchedulerPhaseMetrics()
	{
		return getSchedulerPhaseMetrics();
	}

	unsigned long long CpuCoreDiagnosticSnapshot::ReadCommittedPc(int virtualThreadId)
	{
		return _committedPcs[virtualThreadId];
	}

	unsigned long long CpuCoreDiagnosticSnapshot::ReadArch(int virtualThreadId, int registerId)
	{
		return _architecturalRegisters[(virtualThreadId * 32) + registerId];
	}

	bool CpuCoreDiagnosticSnapshot::ReadVirtualThreadStalled(int virtualThreadId)
	{
		return _virtualThreadStalled[virtualThreadId];
	}

	Processor::CPU_Core::FPExceptionContext CpuCoreDiagnosticSnapshot::ReadThreadFpContext(int virtualThreadId)
	{
		return _threadFpContexts[virtualThreadId];
	}

	bool CpuCoreDiagnosticSnapshot::HasAnyVirtualThreadPipelineState(PipelineState state)
	{
		return std::find(_virtualThreadPipelineStates.begin(), _virtualThreadPipelineStates.end(), state) != _virtualThreadPipelineStates.end();
	}

	std::vector<unsigned int> CpuCoreDiagnosticSnapshot::GetGrlbBanks()
	{
		return _grlbBanks;
	}

	Processor::CPU_Core::PipelineObservationSnapshot CpuCoreDiagnosticSnapshot::GetPipelineObservationSnapshot()
	{
		return _pipelineObservation;
	}

	std::shared_ptr<CpuCoreDiagnosticSnapshot> CpuCoreDiagnosticSnapshot::Capture(std::shared_ptr<Processor::CPU_Core> core)
	{
		std::vector<unsigned long long> committedPcs(Processor::CPU_Core::SmtWays);
		std::vector<unsigned long long> architecturalRegisters(Processor::CPU_Core::SmtWays * 32);
		std::vector<bool> virtualThreadStalled(Processor::CPU_Core::SmtWays);
		std::vector<PipelineState> virtualThreadPipelineStates(Processor::CPU_Core::SmtWays);
		std::vector<Processor::CPU_Core::FPExceptionContext> threadFpContexts(Processor::CPU_Core::SmtWays);

		for (int vt = 0; vt < Processor::CPU_Core::SmtWays; vt++)
		{
			committedPcs[vt] = core->ReadCommittedPc(vt);
			for (int registerId = 0; registerId < 32; registerId++)
			{
				architecturalRegisters[(vt * 32) + registerId] = core->ReadArch(vt, registerId);
			}
			virtualThreadPipelineStates[vt] = core->ReadVirtualThreadPipelineState(vt);
			virtualThreadStalled[vt] = core->ReadVirtualThreadStalled(vt);
			threadFpContexts[vt] = core->ReadThreadFpContext(vt);
		}

	const auto callStack = core->GetCallStackSnapshot();
	const auto interruptStack = core->GetInterruptStackSnapshot();
	const int callStackDepth = callStack.first;
	const int interruptStackDepth = interruptStack.first;
	const auto cacheCounts = core->GetCacheEntryCountsSnapshot();
		std::vector<unsigned int> grlbBanks = core->GetGrlbBanks();
		return std::make_shared<CpuCoreDiagnosticSnapshot>(core->getCoreID(), committedPcs, architecturalRegisters, virtualThreadStalled, virtualThreadPipelineStates, threadFpContexts, core->getCoreFlagsRegisterSnapshot(), callStackDepth, callStack.second, interruptStackDepth, interruptStack.second, grlbBanks, core->getStructuralStalls(), core->getExceptionStatus()->HasExceptions(), cacheCounts[0], cacheCounts[1], cacheCounts[2], cacheCounts[3], core->getCsr()->DirectRead(CsrAddresses::MpowerState), core->getCsr()->DirectRead(CsrAddresses::MperfLevel), core->getVectorConfig()->getVL(), core->getVectorConfig()->getTailAgnostic(), core->getVectorConfig()->getMaskAgnostic(), core->GetPipelineObservationSnapshot(), core->GetReplayPhaseMetrics(), core->GetSchedulerPhaseMetrics());
	}
}
