#include "HybridCPU_ISE.IseObservationService.h"
#include "../CloseToHSL/Core/Architecture/State/Pipeline/PipelineState.h"
#include "../CloseToHSL/Core/Pipeline/Scheduling/Fsp/FspPowerController.h"
#include "../Arch/PowerControlCsr.h"

using namespace HybridCPU_ISE::Machine;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;

namespace HybridCPU_ISE
{
	namespace
	{
		std::wstring PipelineStateName(YAKSys_Hybrid_CPU::Core::PipelineState state)
		{
			switch (state)
			{
			case YAKSys_Hybrid_CPU::Core::PipelineState::Task: return L"Task";
			case YAKSys_Hybrid_CPU::Core::PipelineState::VmEntry: return L"VmEntry";
			case YAKSys_Hybrid_CPU::Core::PipelineState::GuestExecution: return L"GuestExecution";
			case YAKSys_Hybrid_CPU::Core::PipelineState::VmExit: return L"VmExit";
			case YAKSys_Hybrid_CPU::Core::PipelineState::Halted: return L"Halted";
			case YAKSys_Hybrid_CPU::Core::PipelineState::Reset: return L"Reset";
			case YAKSys_Hybrid_CPU::Core::PipelineState::WaitForClusterSync: return L"WaitForClusterSync";
			case YAKSys_Hybrid_CPU::Core::PipelineState::PtwStall: return L"PtwStall";
			case YAKSys_Hybrid_CPU::Core::PipelineState::ClockGatedDonor: return L"ClockGatedDonor";
			case YAKSys_Hybrid_CPU::Core::PipelineState::TrapPending: return L"TrapPending";
			case YAKSys_Hybrid_CPU::Core::PipelineState::WaitForEvent: return L"WaitForEvent";
			default: return std::to_wstring(static_cast<int>(state));
			}
		}

		std::wstring BankArbitrationPolicyName(HybridCPU_ISE::CloseToHSL::Memory::Subsystem::BankArbitrationPolicy policy)
		{
			using Policy = HybridCPU_ISE::CloseToHSL::Memory::Subsystem::BankArbitrationPolicy;
			switch (policy)
			{
			case Policy::RoundRobin: return L"RoundRobin";
			case Policy::WeightedFair: return L"WeightedFair";
			case Policy::Priority: return L"Priority";
			case Policy::FixedStatic: return L"FixedStatic";
			default: return std::to_wstring(static_cast<int>(policy));
			}
		}
	}


	std::vector<unsigned long long> IseObservationService::GetVirtualThreadLivePcs(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			Processor::CPU_Core::PipelineObservationSnapshot observation = ReadCoreObservationSnapshot(core);
			return BuildVirtualThreadLivePcs(core, observation.getActiveVirtualThreadId(), observation.getActiveLivePc());
		}
	}

	std::vector<bool> IseObservationService::GetVirtualThreadStalled(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			Processor::CPU_Core::PipelineObservationSnapshot observation = ReadCoreObservationSnapshot(core);
			Processor::CPU_Core::PipelineControl pipeState = observation.getPipelineControl();
			return BuildVirtualThreadStalledSnapshot(core, observation.getActiveVirtualThreadId(), pipeState);
		}
	}

	std::vector<std::wstring> IseObservationService::GetVirtualThreadStallReasons(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			Processor::CPU_Core::PipelineObservationSnapshot observation = ReadCoreObservationSnapshot(core);
			Processor::CPU_Core::PipelineControl pipeState = observation.getPipelineControl();
			std::vector<bool> stalled = BuildVirtualThreadStalledSnapshot(core, observation.getActiveVirtualThreadId(), pipeState);
			return BuildVirtualThreadStallReasons(core, stalled, observation);
		}
	}

	std::vector<Processor::CPU_Core::FPExceptionContext> IseObservationService::GetVirtualThreadExceptions(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			std::vector<Processor::CPU_Core::FPExceptionContext> exceptions(4);

			for (int vt = 0; vt < 4; vt++)
			{
				exceptions[vt] = core->ReadThreadFpContext(vt);
			}

			return exceptions;
		}
	}

	std::vector<unsigned long long> IseObservationService::GetVirtualThreadRegisters(int coreId, int vtId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			ValidateVtId(vtId);

			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			return BuildVirtualThreadRegisters(core, vtId);
		}
	}

	unsigned long long IseObservationService::GetLiveInstructionPointer(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			return GetCoreSnapshot(coreId)->GetPipelineObservationSnapshot().getActiveLivePc();
		}
	}

	int IseObservationService::GetActiveVirtualThreadId(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			return GetCoreSnapshot(coreId)->GetPipelineObservationSnapshot().getActiveVirtualThreadId();
		}
	}

	StackFlagsSnapshot IseObservationService::GetStackFlagsSnapshot(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			Processor::CPU_Core::FlagsRegister flags = core->getCoreFlagsRegister();

			StackFlagsSnapshot temp_var = StackFlagsSnapshot();
			temp_var.CallStackDepth = core->getCallStackDepth();
			temp_var.CallStackTop = core->getCallStackTop();
			temp_var.InterruptStackDepth = core->getInterruptStackDepth();
			temp_var.InterruptStackTop = core->getInterruptStackTop();
			temp_var.AluZero = flags.getZeroFlag();
			temp_var.AluSign = flags.getSignFlag();
			temp_var.AluOverflow = flags.getOverFlowFlag();
			temp_var.AluParity = flags.getParityFlag();
			temp_var.BaseEnableInterrupt = flags.getEnableInterruptFlag();
			temp_var.BaseIoPrivilege = flags.getIOPrivilegeFlag();
			temp_var.BaseOsKernel = flags.getOSKernelFlag();
			temp_var.CommonDirection = flags.getDirectionFlag();
			temp_var.CommonJump = flags.getJumpFlag();
			temp_var.CommonJumpOutsideVliw = flags.getJumpOutsideVLIWFlag();
			temp_var.EilpOffsetVliwOpcode = flags.Offset_VLIWOpCode;
			return temp_var;
		}
	}

	CoreStateSnapshot IseObservationService::GetCoreState(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			return BuildCoreStateSnapshot(_machineStateSource, coreId);
		}
	}

	int IseObservationService::GetTotalCores()
	{
		return GetCoreCount();
	}

	std::vector<unsigned char> IseObservationService::ReadMemory(unsigned long long address, int length)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			return ReadMachineMemory(address, length);
		}
	}

	void IseObservationService::ValidateCoreId(int coreId)
	{
		ValidateCoreId(_machineStateSource, coreId);
	}

	void IseObservationService::ValidateCoreId(std::shared_ptr<IIseMachineStateSource> machineStateSource, int coreId)
	{
		int coreCount = GetCoreCount(machineStateSource);
		if (coreId < 0 || coreId >= coreCount)
		{
			throw ArgumentOutOfRangeException(L"coreId", std::format(L"Core ID must be between 0 and {0}", coreCount - 1));
		}
	}

	void IseObservationService::ValidateVtId(int vtId)
	{
		if (vtId < 0 || vtId >= 4)
		{
			throw ArgumentOutOfRangeException(L"vtId", L"Virtual Thread ID must be between 0 and 3");
		}
	}

	Processor::CPU_Core::PipelineObservationSnapshot IseObservationService::ReadCoreObservationSnapshot(std::shared_ptr<CpuCoreDiagnosticSnapshot> core)
	{
		return core->GetPipelineObservationSnapshot();
	}

	std::wstring IseObservationService::DetermineStallReason(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int vtId, Processor::CPU_Core::PipelineObservationSnapshot const observation)
	{
		if (vtId == observation.getActiveVirtualThreadId() && observation.getPipelineControl().Stalled)
		{
			return Processor::CPU_Core::PipelineStallText::Render(observation.getPipelineControl().StallReason, Processor::CPU_Core::PipelineStallTextStyle::Snapshot);
		}

		if (IsVirtualThreadMemoryStalled(core, vtId))
		{
			return L"Memory Wait";
		}

		switch (observation.getCurrentVirtualThreadPipelineState())
		{
			case PipelineState::WaitForClusterSync:
				return L"Pod Barrier Wait";
			case PipelineState::PtwStall:
				return L"PTW Walk";
			case PipelineState::ClockGatedDonor:
				return L"Clock Gated";
			case PipelineState::WaitForEvent:
				return L"Wait For Event";
			case PipelineState::Halted:
				return L"Halted";
		default:
			break;
		}

		// Default for unknown stall
		return L"Unknown Stall";
	}

	std::vector<unsigned long long> IseObservationService::BuildVirtualThreadLivePcs(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int activeVtId, unsigned long long activeLivePc)
	{
		std::vector<unsigned long long> pcs(Processor::CPU_Core::SmtWays);

		for (int vt = 0; vt < pcs.size(); vt++)
		{
			pcs[vt] = vt == activeVtId ? activeLivePc : core->ReadCommittedPc(vt);
		}

		return pcs;
	}

	std::vector<unsigned long long> IseObservationService::BuildVirtualThreadCommittedPcs(std::shared_ptr<CpuCoreDiagnosticSnapshot> core)
	{
		std::vector<unsigned long long> pcs(Processor::CPU_Core::SmtWays);

		for (int vt = 0; vt < pcs.size(); vt++)
		{
			pcs[vt] = core->ReadCommittedPc(vt);
		}

		return pcs;
	}

	std::vector<bool> IseObservationService::BuildVirtualThreadStalledSnapshot(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int activeVtId, Processor::CPU_Core::PipelineControl const pipeState)
	{
		std::vector<bool> stalled(Processor::CPU_Core::SmtWays);
		for (int vt = 0; vt < stalled.size(); vt++)
		{
			stalled[vt] = core->ReadVirtualThreadStalled(vt);
		}

		if (pipeState.Enabled && pipeState.Stalled)
		{
			if (static_cast<unsigned int>(activeVtId) < static_cast<unsigned int>(stalled.size()))
			{
				stalled[activeVtId] = true;
			}
		}

		return stalled;
	}

	std::vector<std::wstring> IseObservationService::BuildVirtualThreadStallReasons(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, std::vector<bool> &stalled, Processor::CPU_Core::PipelineObservationSnapshot const observation)
	{
		std::vector<std::wstring> reasons(Processor::CPU_Core::SmtWays);

		for (int vt = 0; vt < reasons.size(); vt++)
		{
			if (vt >= stalled.size())
			{
				reasons[vt] = L"Unknown";
				continue;
			}

			reasons[vt] = stalled[vt] ? DetermineStallReason(core, vt, observation) : L"None";
		}

		return reasons;
	}

	std::vector<unsigned long long> IseObservationService::BuildVirtualThreadRegisters(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int vtId)
	{
		std::vector<unsigned long long> registers(32);

		for (int i = 0; i < registers.size(); i++)
		{
			registers[i] = core->ReadArch(vtId, i);
		}

		return registers;
	}

	bool IseObservationService::IsVirtualThreadMemoryStalled(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int vtId)
	{
		return static_cast<unsigned int>(vtId) < Processor::CPU_Core::SmtWays && core->ReadVirtualThreadStalled(vtId);
	}

	CoreTimebaseSnapshot IseObservationService::BuildCoreTimebaseSnapshot(Processor::CPU_Core::PipelineControl const pipeState)
	{
		bool isAvailable = pipeState.Enabled || pipeState.getCycleCount() != 0 || pipeState.getInstructionsRetired() != 0 || pipeState.getStallCycles() != 0 || pipeState.Stalled;

		return isAvailable ? CoreTimebaseSnapshot(pipeState.getCycleCount(), pipeState.Stalled, true, L"") : CoreTimebaseSnapshot(0, false, false, L"Pipeline timebase has not published any live timing facts yet.");
	}

	unsigned short IseObservationService::GetCorePodId(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			int podIndex = coreId / 16; // 16 cores per Pod
			if (static_cast<unsigned int>(podIndex) < static_cast<unsigned int>(GetPodCount()))
			{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: PodController? pod = GetPodOrNull(podIndex);
				std::shared_ptr<PodController> pod = GetPodOrNull(podIndex);
				if (pod != nullptr)
				{
					return pod->getPodId();
				}
			}
			return 0;
		}
	}

	bool IseObservationService::GetPodBarrierState(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			return core->HasAnyVirtualThreadPipelineState(PipelineState::WaitForClusterSync);
		}
	}

	PodSnapshot IseObservationService::GetPodSnapshot(int podIndex)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			return BuildPodSnapshot(_machineStateSource, podIndex);
		}
	}

	GrlbSnapshot IseObservationService::GetGrlbSnapshot(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);

			// Get GRLB banks (4 × 32-bit banks = 128 bits total)
			std::vector<unsigned int> banks = core->GetGrlbBanks();

			GrlbSnapshot temp_var = GrlbSnapshot();
			temp_var.CoreId = coreId;
			temp_var.Bank0 = banks.size() > 0 ? banks[0] : 0;
			temp_var.Bank1 = banks.size() > 1 ? banks[1] : 0;
			temp_var.Bank2 = banks.size() > 2 ? banks[2] : 0;
			temp_var.Bank3 = banks.size() > 3 ? banks[3] : 0;
			temp_var.StructuralStalls = core->getStructuralStalls();
			return temp_var;
		}
	}

	bool IseObservationService::IsGrlbResourceLocked(int coreId, int resourceBit)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			if (resourceBit < 0 || resourceBit >= 128)
			{
				throw ArgumentOutOfRangeException(L"resourceBit", L"Resource bit must be between 0 and 127");
			}

			std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(coreId);
			std::vector<unsigned int> banks = core->GetGrlbBanks();

			int bankIndex = resourceBit / 32;
			int bitInBank = resourceBit % 32;

			if (bankIndex < banks.size())
			{
				return (banks[bankIndex] & (1u << bitInBank)) != 0;
			}

			return false;
		}
	}

	FspPowerSnapshot IseObservationService::GetFspPowerSnapshot(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			int podIndex = coreId / 16;
			bool isClockGated = GetCoreSnapshot(coreId)->HasAnyVirtualThreadPipelineState(PipelineState::ClockGatedDonor);

			if (static_cast<unsigned int>(podIndex) < static_cast<unsigned int>(GetPodCount()))
			{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: PodController? pod = GetPodOrNull(podIndex);
				std::shared_ptr<PodController> pod = GetPodOrNull(podIndex);
				if (pod == nullptr)
				{
					FspPowerSnapshot temp_var = FspPowerSnapshot();
					temp_var.CoreId = coreId;
					temp_var.PodIndex = podIndex;
					temp_var.CurrentGatedCount = 0;
					temp_var.TotalGatedCycles = 0;
					temp_var.GateTransitions = 0;
					temp_var.IsClockGated = isClockGated;
					return temp_var;
				}

				FspPowerController powerController = pod->PowerController;

				FspPowerSnapshot temp_var2 = FspPowerSnapshot();
				temp_var2.CoreId = coreId;
				temp_var2.PodIndex = podIndex;
				temp_var2.CurrentGatedCount = powerController.getCurrentGatedCount();
				temp_var2.TotalGatedCycles = powerController.getTotalGatedCycles();
				temp_var2.GateTransitions = powerController.getGateTransitions();
				temp_var2.IsClockGated = isClockGated;
				return temp_var2;
			}

			FspPowerSnapshot temp_var3 = FspPowerSnapshot();
			temp_var3.CoreId = coreId;
			temp_var3.PodIndex = podIndex;
			temp_var3.CurrentGatedCount = 0;
			temp_var3.TotalGatedCycles = 0;
			temp_var3.GateTransitions = 0;
			temp_var3.IsClockGated = isClockGated;
			return temp_var3;
		}
	}

	MemorySubsystemSnapshot IseObservationService::GetMemorySubsystemSnapshot()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemorySubsystem? mem = GetMemorySubsystem();
			std::shared_ptr<MemorySubsystem> mem = GetMemorySubsystem();
			if (mem == nullptr)
			{
				MemorySubsystemSnapshot temp_var = MemorySubsystemSnapshot();
				temp_var.NumBanks = 0;
				temp_var.TotalBursts = 0;
				temp_var.BankConflicts = 0;
				temp_var.StallCycles = 0;
				temp_var.TotalBytesTransferred = 0;
				temp_var.CurrentQueuedRequests = 0;
				temp_var.MaxQueueDepth = 0;
				temp_var.AverageBurstLength = 0.0;
				temp_var.AverageWaitCycles = 0.0;
				temp_var.BurstEfficiency = 0.0;
				temp_var.IsChannelOverloaded = false;
				temp_var.ArbitrationPolicy = L"Unknown";
				return temp_var;
			}

			MemorySubsystemSnapshot temp_var2 = MemorySubsystemSnapshot();
			temp_var2.NumBanks = mem->getNumBanks();
			temp_var2.TotalBursts = mem->getTotalBursts();
			temp_var2.BankConflicts = mem->getBankConflicts();
			temp_var2.StallCycles = mem->getStallCycles();
			temp_var2.TotalBytesTransferred = mem->getTotalBytesTransferred();
			temp_var2.CurrentQueuedRequests = mem->getCurrentQueuedRequests();
			temp_var2.MaxQueueDepth = mem->getMaxQueueDepth();
			temp_var2.AverageBurstLength = mem->getAverageBurstLength();
			temp_var2.AverageWaitCycles = mem->getAverageWaitCycles();
			temp_var2.BurstEfficiency = mem->getBurstEfficiency();
			temp_var2.IsChannelOverloaded = mem->isChannelOverloaded();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
			temp_var2.ArbitrationPolicy = BankArbitrationPolicyName(mem->getArbitrationPolicy());
			return temp_var2;
		}
	}

	MemoryConfigSnapshot IseObservationService::GetMemoryConfig()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemorySubsystem? mem = GetMemorySubsystem();
			std::shared_ptr<MemorySubsystem> mem = GetMemorySubsystem();
			if (mem == nullptr)
			{
				MemoryConfigSnapshot temp_var = MemoryConfigSnapshot();
				temp_var.NumBanks = 0;
				temp_var.BankWidthBytes = 0;
				temp_var.NumMemoryPorts = 0;
				temp_var.BankBandwidthGBps = 0.0;
				temp_var.DmaThresholdBytes = 0;
				temp_var.AxiBoundary = 0;
				temp_var.MaxBurstLength = 0;
				temp_var.PortSwitchingPenalty = 0;
				temp_var.RowBufferHitLatency = 0;
				temp_var.RowBufferMissLatency = 0;
				temp_var.RowBufferSize = 0;
				return temp_var;
			}

			MemoryConfigSnapshot temp_var2 = MemoryConfigSnapshot();
			temp_var2.NumBanks = mem->getNumBanks();
			temp_var2.BankWidthBytes = mem->getBankWidthBytes();
			temp_var2.NumMemoryPorts = mem->getNumMemoryPorts();
			temp_var2.BankBandwidthGBps = mem->getBankBandwidthGBps();
			temp_var2.DmaThresholdBytes = mem->getDmaThresholdBytes();
			temp_var2.AxiBoundary = mem->getAxiBoundary();
			temp_var2.MaxBurstLength = mem->getMaxBurstLength();
			temp_var2.PortSwitchingPenalty = mem->getPortSwitchingPenalty();
			temp_var2.RowBufferHitLatency = mem->getRowBufferHitLatency();
			temp_var2.RowBufferMissLatency = mem->getRowBufferMissLatency();
			temp_var2.RowBufferSize = mem->getRowBufferSize();
			return temp_var2;
		}
	}

	long long IseObservationService::GetTotalMemorySize()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			return GetTotalMachineMemorySize();
		}
	}

	CompilerStateSnapshot IseObservationService::GetCompilerStateSnapshot()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			CompilerStateSnapshot temp_var = CompilerStateSnapshot();
			temp_var.IsAvailable = false;
			temp_var.AvailabilityReason = CompilerTelemetryUnavailableReason;
			temp_var.IsCompiling = false;
			temp_var.ThreadCount = 0;
			temp_var.TotalInstructionsCompiled = 0;
			temp_var.BarriersInserted = 0;
			temp_var.CrossThreadDependencies = 0;
			temp_var.BundleUtilization = 0.0;
			return temp_var;
		}
	}

	ThreadCompilerStateSnapshot IseObservationService::GetThreadCompilerState(int vtId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateVtId(vtId);

			ThreadCompilerStateSnapshot temp_var = ThreadCompilerStateSnapshot();
			temp_var.IsAvailable = false;
			temp_var.AvailabilityReason = CompilerTelemetryUnavailableReason;
			temp_var.VirtualThreadId = vtId;
			temp_var.InstructionCount = 0;
			temp_var.DomainTag = 0;
			temp_var.RegistersAllocated = 0;
			temp_var.HasPendingBarrier = false;
			return temp_var;
		}
	}

	DependencyGraphSnapshot IseObservationService::GetDependencyGraph()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
// C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
// ORIGINAL LINE: bool[,] dependencies = new bool[4, 4];
			std::vector<std::vector<bool>> dependencies = RectangularVectors::rectangularBoolVector(4, 4);
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					dependencies[i][j] = false;
				}
			}

			DependencyGraphSnapshot temp_var = DependencyGraphSnapshot();
			temp_var.IsAvailable = false;
			temp_var.AvailabilityReason = CompilerTelemetryUnavailableReason;
			temp_var.DependencyMatrix = dependencies;
			temp_var.MemoryAccessCount = 0;
			temp_var.HasCyclicDependency = false;
			temp_var.TotalDependencies = 0;
			return temp_var;
		}
	}

	BarrierSchedulerSnapshot IseObservationService::GetBarrierSchedulerState()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			BarrierSchedulerSnapshot temp_var = BarrierSchedulerSnapshot();
			temp_var.IsAvailable = false;
			temp_var.AvailabilityReason = CompilerTelemetryUnavailableReason;
			temp_var.BarriersInserted = 0;
			temp_var.ActiveBarrierCount = 0;
			temp_var.MaxBarrierTimeout = 0;
			temp_var.CoalescedBarriers = 0;
			return temp_var;
		}
	}

	CoreStateSnapshot IseObservationService::BuildCoreStateSnapshot(std::shared_ptr<IIseMachineStateSource> machineStateSource, int coreId)
	{
		ValidateCoreId(machineStateSource, coreId);
		std::shared_ptr<CpuCoreDiagnosticSnapshot> core = GetCoreSnapshot(machineStateSource, coreId);
		Processor::CPU_Core::PipelineObservationSnapshot observation = ReadCoreObservationSnapshot(core);
		Processor::CPU_Core::PipelineControl pipeState = observation.getPipelineControl();
		CoreTimebaseSnapshot timebase = BuildCoreTimebaseSnapshot(pipeState);
		std::vector<unsigned long long> livePcs = BuildVirtualThreadLivePcs(core, observation.getActiveVirtualThreadId(), observation.getActiveLivePc());
		std::vector<unsigned long long> committedPcs = BuildVirtualThreadCommittedPcs(core);
		std::vector<bool> stalled = BuildVirtualThreadStalledSnapshot(core, observation.getActiveVirtualThreadId(), pipeState);
		std::vector<std::wstring> stallReasons = BuildVirtualThreadStallReasons(core, stalled, observation);
		std::vector<unsigned long long> activeVtRegisters = BuildVirtualThreadRegisters(core, observation.getActiveVirtualThreadId());
		Processor::CPU_Core::CorePowerState currentPowerState = PowerControlCsr::DecodePowerState(core->getPowerStateCsrValue());
		unsigned int currentPerformanceLevel = PowerControlCsr::DecodePerformanceLevel(core->getPerformanceLevelCsrValue());

		CoreStateSnapshot temp_var(timebase, observation.getDecodePublicationCertificate(), observation.getExecuteCompletionCertificate(), observation.getRetireVisibilityCertificate());
		temp_var.CoreId = static_cast<int>(core->getCoreId());
		temp_var.LiveInstructionPointer = observation.getActiveLivePc();
		temp_var.CycleCount = timebase.getCycleCount();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
		temp_var.CurrentState = PipelineStateName(observation.getCurrentVirtualThreadPipelineState());
		temp_var.CurrentPowerState = currentPowerState;
		temp_var.CurrentPerformanceLevel = currentPerformanceLevel;
		temp_var.IsStalled = timebase.isStalled();
		temp_var.HasExceptions = core->getHasExceptions();
		temp_var.ActiveVirtualThreadId = observation.getActiveVirtualThreadId();
		temp_var.VirtualThreadLivePcs = livePcs;
		temp_var.VirtualThreadCommittedPcs = committedPcs;
		temp_var.VirtualThreadStalled = stalled;
		temp_var.VirtualThreadStallReasons = stallReasons;
		temp_var.ActiveVirtualThreadRegisters = activeVtRegisters;
		temp_var.L1VliwBundleCount = core->getL1VliwBundleCount();
		temp_var.L1DataEntryCount = core->getL1DataEntryCount();
		temp_var.L2VliwBundleCount = core->getL2VliwBundleCount();
		temp_var.L2DataEntryCount = core->getL2DataEntryCount();
		temp_var.PipelineEnabled = pipeState.Enabled;
		temp_var.PipelineIPC = pipeState.GetIPC();
		temp_var.PipelineEfficiency = pipeState.GetEfficiency();
		temp_var.PipelineCycleCount = timebase.getCycleCount();
		temp_var.PipelineStallCycles = pipeState.getStallCycles();
		temp_var.PipelineInstructionsRetired = pipeState.getInstructionsRetired();
		temp_var.PipelineBranchMispredicts = pipeState.getBranchMispredicts();
		temp_var.PipelineDataHazards = pipeState.getDataHazards();
		temp_var.PipelineMemoryStalls = pipeState.getMemoryStalls();
		temp_var.PipelineForwardingEvents = pipeState.getForwardingEvents();
		temp_var.PipelineControlHazards = pipeState.getControlHazards();
		temp_var.PipelineWAWHazards = pipeState.getWAWHazards();
		temp_var.PipelineLoadUseBubbles = pipeState.getLoadUseBubbles();
		temp_var.PipelineBundleSlotIndex = observation.getPipelineBundleSlotIndex();
		temp_var.DecodedBundleStateOwnerKind = observation.getDecodedBundleStateOwnerKind();
		temp_var.DecodedBundleStateEpoch = observation.getDecodedBundleStateEpoch();
		temp_var.DecodedBundleStateVersion = observation.getDecodedBundleStateVersion();
		temp_var.DecodedBundleStateKind = observation.getDecodedBundleStateKind();
		temp_var.DecodedBundleStateOrigin = observation.getDecodedBundleStateOrigin();
		temp_var.DecodedBundlePc = observation.getDecodedBundlePc();
		temp_var.DecodedBundleValidMask = observation.getDecodedBundleValidMask();
		temp_var.DecodedBundleNopMask = observation.getDecodedBundleNopMask();
		temp_var.DecodedBundleHasCanonicalDecode = observation.getDecodedBundleHasCanonicalDecode();
		temp_var.DecodedBundleHasCanonicalLegality = observation.getDecodedBundleHasCanonicalLegality();
		temp_var.DecodedBundleHasDecodeFault = observation.getDecodedBundleHasDecodeFault();
		temp_var.VectorLength = core->getVectorLength();
		temp_var.VectorTailAgnostic = core->getVectorTailAgnostic();
		temp_var.VectorMaskAgnostic = core->getVectorMaskAgnostic();
		return temp_var;
	}

	PodSnapshot IseObservationService::BuildPodSnapshot(std::shared_ptr<IIseMachineStateSource> machineStateSource, int podIndex)
	{
		int podCount = GetPodCount(machineStateSource);
		if (podIndex < 0 || podIndex >= podCount)
		{
			throw ArgumentOutOfRangeException(L"podIndex", std::format(L"Pod index must be between 0 and {0}", podCount - 1));
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: PodController? pod = GetPodOrNull(machineStateSource, podIndex);
		std::shared_ptr<PodController> pod = GetPodOrNull(machineStateSource, podIndex);
		if (pod == nullptr)
		{
			PodSnapshot temp_var = PodSnapshot();
			temp_var.PodIndex = podIndex;
			temp_var.PodX = 0;
			temp_var.PodY = 0;
			temp_var.PodId = 0;
			temp_var.DomainCertificate = 0;
			temp_var.PowerGatedCores = 0;
			return temp_var;
		}

		PodSnapshot temp_var2 = PodSnapshot();
		temp_var2.PodIndex = podIndex;
		temp_var2.PodX = pod->getPodX();
		temp_var2.PodY = pod->getPodY();
		temp_var2.PodId = pod->getPodId();
		temp_var2.DomainCertificate = pod->getDomainCertificate();
		temp_var2.PowerGatedCores = pod->PowerController.getCurrentGatedCount();
		return temp_var2;
	}

	std::shared_ptr<PerformanceReport> IseObservationService::GetPerformanceReport()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			return _machineStateSource->GetPerformanceReport();
		}
	}

	ReplayPhaseMetrics IseObservationService::GetReplayPhaseMetrics(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			return _machineStateSource->GetReplayPhaseMetrics(coreId);
		}
	}

	SchedulerPhaseMetrics IseObservationService::GetSchedulerPhaseMetrics(int coreId)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			return _machineStateSource->GetSchedulerPhaseMetrics(coreId);
		}
	}

	std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> IseObservationService::GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash)
	{
		ArgumentException::ThrowIfNullOrWhiteSpace(programHash);

		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			ValidateCoreId(coreId);
			return _machineStateSource->GetTypedSlotTelemetryProfile(coreId, programHash);
		}
	}

	std::wstring IseObservationService::GetReplayToken()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(*_syncLock);
			return _machineStateSource->GetReplayToken();
		}
	}

	IseObservationService::IseObservationService(std::shared_ptr<IIseMachineStateSource> machineStateSource, std::shared_ptr<std::recursive_mutex> syncLock) : _machineStateSource(machineStateSource), _syncLock(std::move(syncLock))
	{
		ArgumentNullException::ThrowIfNull(machineStateSource);
		ArgumentNullException::ThrowIfNull(syncLock);

	}

	MachineStateSourceProvenance IseObservationService::getSourceProvenance() const
	{
		return _machineStateSource->getSourceProvenance();
	}

	int IseObservationService::GetCoreCount()
	{
		return GetCoreCount(_machineStateSource);
	}

	int IseObservationService::GetCoreCount(std::shared_ptr<IIseMachineStateSource> machineStateSource)
	{
		return machineStateSource->GetCoreCount();
	}

	std::shared_ptr<CpuCoreDiagnosticSnapshot> IseObservationService::GetCoreSnapshot(int coreId)
	{
		return GetCoreSnapshot(_machineStateSource, coreId);
	}

	std::shared_ptr<CpuCoreDiagnosticSnapshot> IseObservationService::GetCoreSnapshot(std::shared_ptr<IIseMachineStateSource> machineStateSource, int coreId)
	{
		return machineStateSource->GetCoreSnapshot(coreId);
	}

	int IseObservationService::GetPodCount()
	{
		return GetPodCount(_machineStateSource);
	}

	int IseObservationService::GetPodCount(std::shared_ptr<IIseMachineStateSource> machineStateSource)
	{
		return machineStateSource->GetPodCount();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private PodController? GetPodOrNull(int podIndex)
	std::shared_ptr<PodController> IseObservationService::GetPodOrNull(int podIndex)
	{
		return GetPodOrNull(_machineStateSource, podIndex);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static PodController? GetPodOrNull(IIseMachineStateSource machineStateSource, int podIndex)
	std::shared_ptr<PodController> IseObservationService::GetPodOrNull(std::shared_ptr<IIseMachineStateSource> machineStateSource, int podIndex)
	{
		return machineStateSource->GetPod(podIndex);
	}

	std::vector<unsigned char> IseObservationService::ReadMachineMemory(unsigned long long address, int length)
	{
		return ReadMachineMemory(_machineStateSource, address, length);
	}

	std::vector<unsigned char> IseObservationService::ReadMachineMemory(std::shared_ptr<IIseMachineStateSource> machineStateSource, unsigned long long address, int length)
	{
		return machineStateSource->ReadMemory(address, length);
	}

	long long IseObservationService::GetTotalMachineMemorySize()
	{
		return GetTotalMachineMemorySize(_machineStateSource);
	}

	long long IseObservationService::GetTotalMachineMemorySize(std::shared_ptr<IIseMachineStateSource> machineStateSource)
	{
		return machineStateSource->GetTotalMemorySize();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemorySubsystem? GetMemorySubsystem()
	std::shared_ptr<MemorySubsystem> IseObservationService::GetMemorySubsystem()
	{
		return GetMemorySubsystem(_machineStateSource);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static MemorySubsystem? GetMemorySubsystem(IIseMachineStateSource machineStateSource)
	std::shared_ptr<MemorySubsystem> IseObservationService::GetMemorySubsystem(std::shared_ptr<IIseMachineStateSource> machineStateSource)
	{
		return machineStateSource->GetMemorySubsystem();
	}
}
