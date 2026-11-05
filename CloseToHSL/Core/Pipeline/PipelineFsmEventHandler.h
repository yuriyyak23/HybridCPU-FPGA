#pragma once

#include "../Architecture/Registers/CSR/CsrFile.h"
#include "../Runtime/Managed/IHybridCpuIseEcallBridgeV1.h"
#include "PipelineEvent.h"
#include "../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../Architecture/State/Pipeline/PipelineState.h"
#include "../State/ICpuState.h"
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{
	/// <summary>
	/// Handles first-class non-VMX system and privileged <see cref="PipelineEvent"/> instances.
	/// Production VMX no longer flows through this event plane or its constructor
	/// dependencies; it retires through typed VMX retire outcomes instead.
	/// </summary>
	class PipelineFsmEventHandler final : public std::enable_shared_from_this<PipelineFsmEventHandler>
	{
	private:
		static constexpr unsigned long long CauseMachineEcall = 11;
		static constexpr unsigned long long CauseSupervisorEcall = 9;
		static constexpr unsigned long long CauseUserEcall = 8;
		static constexpr unsigned long long CauseBreakpoint = 3;
		static constexpr unsigned long long CauseIllegalInstruction = 2;

		const std::shared_ptr<CsrFile> _csr;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly IHybridCpuIseEcallBridgeV1? _ecallBridge;
		const std::shared_ptr<IHybridCpuIseEcallBridgeV1> _ecallBridge;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public PipelineFsmEventHandler(CsrFile csr, int vtCount = 4, IHybridCpuIseEcallBridgeV1? ecallBridge = null)
		PipelineFsmEventHandler(std::shared_ptr<CsrFile> csr, int vtCount = 4, std::shared_ptr<IHybridCpuIseEcallBridgeV1> ecallBridge = nullptr);

		PipelineState Handle(std::shared_ptr<PipelineEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState, PrivilegeLevel privilege);

	private:
		void WriteCsr(std::shared_ptr<ICanonicalCpuState> state, unsigned short address, unsigned long long value, PrivilegeLevel privilege);

		unsigned long long ReadCsr(std::shared_ptr<ICanonicalCpuState> state, unsigned short address, PrivilegeLevel privilege);

		PipelineState HandleEcall(std::shared_ptr<EcallEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState, PrivilegeLevel privilege);

		PipelineState HandleEbreak(std::shared_ptr<EbreakEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState);

		PipelineState HandleTrapEntry(std::shared_ptr<TrapEntryEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState);

		PipelineState HandleMret(std::shared_ptr<MretEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState);

		PipelineState HandleSret(std::shared_ptr<SretEvent> evt, PipelineState current, std::shared_ptr<ICanonicalCpuState> cpuState);

		static PipelineState HandleFence(std::shared_ptr<FenceEvent> evt, PipelineState current);

		static PipelineState HandleWfi(std::shared_ptr<WfiEvent> evt, PipelineState current);

		static PipelineState HandleYield(std::shared_ptr<YieldEvent> evt, PipelineState current);

		static PipelineState HandleWfe(std::shared_ptr<WfeEvent> evt, PipelineState current);

		static PipelineState HandleSev(std::shared_ptr<SevEvent> evt, PipelineState current);

		static PipelineState HandlePodBarrier(std::shared_ptr<PodBarrierEvent> evt, PipelineState current);

		static PipelineState HandleVtBarrier(std::shared_ptr<VtBarrierEvent> evt, PipelineState current);
	};
}
