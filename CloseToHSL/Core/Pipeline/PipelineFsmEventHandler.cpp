#include "PipelineFsmEventHandler.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Arch/CsrAddresses.h"
#include "../Architecture/State/Pipeline/PipelineFsmGuard.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public PipelineFsmEventHandler(CsrFile csr, int vtCount = 4, IHybridCpuIseEcallBridgeV1? ecallBridge = null)
	PipelineFsmEventHandler::PipelineFsmEventHandler(std::shared_ptr<CsrFile> csr, int vtCount, std::shared_ptr<IHybridCpuIseEcallBridgeV1> ecallBridge) : _csr((csr != nullptr) ? csr : throw std::invalid_argument("csr")), _ecallBridge(ecallBridge)
	{

		if (vtCount < 1)
		{
			throw std::out_of_range("At least one virtual thread is required.");
		}
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::Handle(std::shared_ptr<PipelineEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState, PrivilegeLevel privilege)
	{
		if (evt == nullptr)
		{
			throw std::invalid_argument("evt");
		}
		if (cpuState == nullptr)
		{
			throw std::invalid_argument("cpuState");
		}

// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var liveReservation = (dynamic_cast<Processor::CPU_Core::LiveCpuStateAdapter*>(cpuState)) == null ? null : (dynamic_cast<Processor::CPU_Core::LiveCpuStateAdapter*>(cpuState)).BeginHandler(_csr, evt.VtId);
		auto liveAdapter = std::dynamic_pointer_cast<Processor::CPU_Core::LiveCpuStateAdapter>(cpuState);
		auto liveReservation = liveAdapter ? liveAdapter->BeginHandler(_csr, evt->VtId) : nullptr;
		PipelineState result = current;
		if (auto e = std::dynamic_pointer_cast<EcallEvent>(evt)) result = HandleEcall(e, current, cpuState, privilege);
		else if (auto e = std::dynamic_pointer_cast<EbreakEvent>(evt)) result = HandleEbreak(e, current, cpuState);
		else if (auto e = std::dynamic_pointer_cast<TrapEntryEvent>(evt)) result = HandleTrapEntry(e, current, cpuState);
		else if (auto e = std::dynamic_pointer_cast<MretEvent>(evt)) result = HandleMret(e, current, cpuState);
		else if (auto e = std::dynamic_pointer_cast<SretEvent>(evt)) result = HandleSret(e, current, cpuState);
		else if (auto e = std::dynamic_pointer_cast<FenceEvent>(evt)) result = HandleFence(e, current);
		else if (auto e = std::dynamic_pointer_cast<WfiEvent>(evt)) result = HandleWfi(e, current);
		else if (auto e = std::dynamic_pointer_cast<YieldEvent>(evt)) result = HandleYield(e, current);
		else if (auto e = std::dynamic_pointer_cast<WfeEvent>(evt)) result = HandleWfe(e, current);
		else if (auto e = std::dynamic_pointer_cast<SevEvent>(evt)) result = HandleSev(e, current);
		else if (auto e = std::dynamic_pointer_cast<PodBarrierEvent>(evt)) result = HandlePodBarrier(e, current);
		else if (auto e = std::dynamic_pointer_cast<VtBarrierEvent>(evt)) result = HandleVtBarrier(e, current);
		if (liveReservation)
			liveReservation->Complete();
		return result;
	}

	void PipelineFsmEventHandler::WriteCsr(std::shared_ptr<ICanonicalCpuState> state, unsigned short address, unsigned long long value, PrivilegeLevel privilege)
	{
		std::shared_ptr<Processor::CPU_Core::LiveCpuStateAdapter> live = std::dynamic_pointer_cast<Processor::CPU_Core::LiveCpuStateAdapter>(state);
		if (live != nullptr)
		{
			live->WriteEventCsr(_csr, address, value, privilege);
		}
		else
		{
			_csr->Write(address, value, privilege);
		}
	}

	unsigned long long PipelineFsmEventHandler::ReadCsr(std::shared_ptr<ICanonicalCpuState> state, unsigned short address, PrivilegeLevel privilege)
	{
		std::shared_ptr<Processor::CPU_Core::LiveCpuStateAdapter> live = std::dynamic_pointer_cast<Processor::CPU_Core::LiveCpuStateAdapter>(state);
		return live != nullptr ? live->ReadEventCsr(_csr, address, privilege) : _csr->Read(address, privilege);
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleEcall(std::shared_ptr<EcallEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState, PrivilegeLevel privilege)
	{
		if (_ecallBridge && _ecallBridge->TryHandle(evt, cpuState, privilege))
		{
			return current;
		}
		WriteCsr(cpuState, CsrAddresses::Mepc, cpuState->ReadPc(evt->VtId), PrivilegeLevel::Machine);

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			ulong cause = privilege switch
	//			{
	//				PrivilegeLevel.Machine => CauseMachineEcall,
	//				PrivilegeLevel.Supervisor => CauseSupervisorEcall,
	//				_ => CauseUserEcall,
	//			};
		const unsigned long long cause = privilege == PrivilegeLevel::Machine ? CauseMachineEcall : privilege == PrivilegeLevel::Supervisor ? CauseSupervisorEcall : CauseUserEcall;
		WriteCsr(cpuState, CsrAddresses::Mcause, cause, PrivilegeLevel::Machine);

		unsigned long long mtvec = ReadCsr(cpuState, CsrAddresses::Mtvec, PrivilegeLevel::Machine);
		cpuState->WritePc(evt->VtId, mtvec);
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleEbreak(std::shared_ptr<EbreakEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState)
	{
		WriteCsr(cpuState, CsrAddresses::Mepc, cpuState->ReadPc(evt->VtId), PrivilegeLevel::Machine);
		WriteCsr(cpuState, CsrAddresses::Mcause, CauseBreakpoint, PrivilegeLevel::Machine);

		unsigned long long mtvec = ReadCsr(cpuState, CsrAddresses::Mtvec, PrivilegeLevel::Machine);
		cpuState->WritePc(evt->VtId, mtvec);
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleTrapEntry(std::shared_ptr<TrapEntryEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState)
	{
		WriteCsr(cpuState, CsrAddresses::Mepc, cpuState->ReadPc(evt->VtId), PrivilegeLevel::Machine);
		WriteCsr(cpuState, CsrAddresses::Mcause, evt->CauseCode == 0 ? CauseIllegalInstruction : evt->CauseCode, PrivilegeLevel::Machine);
		WriteCsr(cpuState, CsrAddresses::Mtval, evt->FaultAddress, PrivilegeLevel::Machine);

		unsigned long long mtvec = ReadCsr(cpuState, CsrAddresses::Mtvec, PrivilegeLevel::Machine);
		cpuState->WritePc(evt->VtId, mtvec);
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleMret(std::shared_ptr<MretEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState)
	{
		unsigned long long mepc = ReadCsr(cpuState, CsrAddresses::Mepc, PrivilegeLevel::Machine);
		cpuState->WritePc(evt->VtId, mepc);
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleSret(std::shared_ptr<SretEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState)
	{
		unsigned long long sepc = ReadCsr(cpuState, CsrAddresses::Sepc, PrivilegeLevel::Supervisor);
		cpuState->WritePc(evt->VtId, sepc);
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleFence(std::shared_ptr<FenceEvent> evt, PipelineState current)
	{
		(void)evt;
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleWfi(std::shared_ptr<WfiEvent> evt, PipelineState current)
	{
		return PipelineFsmGuard::Advance(current, evt);
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleYield(std::shared_ptr<YieldEvent> evt, PipelineState current)
	{
		(void)evt;
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleWfe(std::shared_ptr<WfeEvent> evt, PipelineState current)
	{
		return PipelineFsmGuard::Advance(current, evt);
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleSev(std::shared_ptr<SevEvent> evt, PipelineState current)
	{
		return current == PipelineState::WaitForEvent ? PipelineFsmGuard::Advance(current, evt) : current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandlePodBarrier(std::shared_ptr<PodBarrierEvent> evt, PipelineState current)
	{
		(void)evt;
		return current;
	}

	YAKSys_Hybrid_CPU::Core::PipelineState PipelineFsmEventHandler::HandleVtBarrier(std::shared_ptr<VtBarrierEvent> evt, PipelineState current)
	{
		(void)evt;
		return current;
	}
}
