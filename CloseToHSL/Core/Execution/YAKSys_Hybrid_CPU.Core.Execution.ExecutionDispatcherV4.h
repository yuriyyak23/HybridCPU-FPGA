#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Diagnostics/TraceEventKind.h"
#include "Dispatch/ExecutionDispatcherV4.h"
#include "../State/ICpuState.h"
#include "../Memory/IAtomicMemoryUnit.h"
#include "Memory/LoadStore/MemoryUnit.h"
#include "../Architecture/Registers/CSR/CsrFile.h"
#include "../Diagnostics/IV4TraceEventSink.h"
#include "../Diagnostics/TelemetryCounters.h"
#include "../Pipeline/IPipelineEventQueue.h"
#include "../Pipeline/MicroOps/Control/ControlRetireTypes.h"
#include "../Pipeline/MicroOps/System/SystemEventTypes.h"
#include "../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include <string>
#include <limits>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using CpuCore = YAKSys_Hybrid_CPU::Processor::CPU_Core;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;




using namespace YAKSys_Hybrid_CPU::Core;



namespace YAKSys_Hybrid_CPU::Core::Execution
{
	class ExecutionDispatcherV4 final : public std::enable_shared_from_this<ExecutionDispatcherV4>
	{
		// Trace event classification helpers.
		// These switch-based classifiers can collapse into canonical descriptor
		// lookups once OpcodeInfo carries the full authoritative trace surface:
		//   OpcodeInfo.Get((uint)instr.CanonicalOpcode).TraceEventKind

	private:
		static TraceEventKind ClassifyTraceEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result);

		static TraceEventKind ClassifyMemoryEvent(std::shared_ptr<InstructionIR> instr);

		static TraceEventKind ClassifyControlFlowEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result);

		static TraceEventKind ClassifyAtomicEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result);

		static TraceEventKind ClassifySystemEvent(std::shared_ptr<InstructionIR> instr);

		static TraceEventKind ClassifyCsrEvent(std::shared_ptr<InstructionIR> instr);

		static TraceEventKind ClassifySmtVtEvent(std::shared_ptr<InstructionIR> instr);

		static TraceEventKind ClassifyVmxEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result);

		static unsigned long long GetTracePayload(std::shared_ptr<InstructionIR> instr, TraceEventKind kind);

		static unsigned long long ResolveCsrTracePayload(std::shared_ptr<InstructionIR> instr);

		inline static unsigned long long ReadExecutionRegister(std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId, unsigned short regId);

		inline static unsigned long long ReadExecutionPc(std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		inline static std::shared_ptr<InvalidOperationException> CreateUnsupportedVectorConfigEagerExecuteSurfaceException(std::shared_ptr<InstructionIR> instr);

		inline static std::shared_ptr<InvalidOperationException> CreateUnsupportedVectorConfigRetireWindowPublicationSurfaceException(std::shared_ptr<InstructionIR> instr);

		inline static std::shared_ptr<Core::Pipeline::PipelineEvent> ThrowUnsupportedRetireWindowPublicationSystemOpcode(std::shared_ptr<InstructionIR> instr);

		inline static std::shared_ptr<Core::Pipeline::PipelineEvent> ThrowUnsupportedRetireWindowPublicationSmtVtOpcode(std::shared_ptr<InstructionIR> instr);

		inline static ExecutionResult ThrowUnsupportedEagerStreamControlOpcode(std::shared_ptr<InstructionIR> instr);

		inline static bool RequiresWiredPipelineEventQueueForEagerExecute(std::shared_ptr<InstructionIR> instr);



		inline static bool IsPipelineOnlyVectorConfigSystemSurface(unsigned short opcode);

		inline static bool IsRetireWindowPublicationOnlySystemEagerExecuteSurface(unsigned short opcode);

		inline static bool IsRetireWindowPublicationOnlySmtVtEagerExecuteSurface(std::shared_ptr<InstructionIR> instr);

		inline static bool IsRetireWindowPublicationOnlyVmxEagerExecuteSurface(unsigned short opcode);

		inline static bool IsPipelineOnlyMemoryEagerExecuteSurface(unsigned short opcode);

		inline static bool RequiresRetireWindowPublicationSerializingBoundaryFollowThrough(std::shared_ptr<InstructionIR> instr);


// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly IAtomicMemoryBus? _atomicBus;
		const std::shared_ptr<IAtomicMemoryBus> _atomicBus;
		const std::shared_ptr<IAtomicMemoryUnit> _atomicMemoryUnit;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MemoryUnit? _memoryUnit;
		const std::shared_ptr<MemoryUnit> _memoryUnit;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly CsrFile? _csrFile;
		const std::shared_ptr<CsrFile> _csrFile;
		const std::shared_ptr<IV4TraceEventSink> _traceSink;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly TelemetryCounters? _telemetry;
		const std::shared_ptr<TelemetryCounters> _telemetry;
		const std::shared_ptr<IPipelineEventQueue> _pipelineEventQueue;

		static std::shared_ptr<IAtomicMemoryUnit> CreateDefaultAtomicMemoryUnit();

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionDispatcherV4(IAtomicMemoryBus? atomicBus = null, System.Nullable<IAtomicMemoryUnit> atomicMemoryUnit = null, System.Nullable<MemoryUnit> memoryUnit = null, System.Nullable<CsrFile> csrFile = null, System.Nullable<IV4TraceEventSink> traceSink = null, System.Nullable<TelemetryCounters> telemetry = null, System.Nullable<IPipelineEventQueue> pipelineEventQueue = null)
		ExecutionDispatcherV4(std::shared_ptr<IAtomicMemoryBus> atomicBus = nullptr, std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit = nullptr, std::shared_ptr<MemoryUnit> memoryUnit = nullptr, std::shared_ptr<CsrFile> csrFile = nullptr, std::shared_ptr<IV4TraceEventSink> traceSink = nullptr, std::shared_ptr<TelemetryCounters> telemetry = nullptr, std::shared_ptr<IPipelineEventQueue> pipelineEventQueue = nullptr);

	private:
		inline static unsigned short ResolveOpcode(std::shared_ptr<InstructionIR> instr);

		inline static std::wstring FormatOpcode(unsigned short opcode);

	public:
		inline bool CanRouteToConfiguredExecutionSurface(std::shared_ptr<InstructionIR> instr);

	private:
		inline void EnforceConfiguredExecutionSurfaceContract(std::shared_ptr<InstructionIR> instr);

		inline void EnforceConfiguredRetireWindowPublicationSurfaceContract(std::shared_ptr<InstructionIR> instr);

		inline bool RequiresWiredCsrFileForExecutionSurface(std::shared_ptr<InstructionIR> instr);

		inline static bool IsPipelineOnlyVectorExceptionControlCsrSurface(unsigned short opcode);

		static std::shared_ptr<InvalidOperationException> CreateMissingCsrFileSurfaceException(std::shared_ptr<InstructionIR> instr);

	public:
		inline ExecutionResult Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state);

		ExecutionResult Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned long long bundleSerial, unsigned char vtId);

		inline void CaptureRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch);

		void CaptureRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned long long bundleSerial, unsigned char vtId);



	private:
		ExecutionResult ExecuteCsr(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		CsrRetireEffect ResolveCsrEffect(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		static void ValidateCounterReadCsrInstruction(std::shared_ptr<InstructionIR> instr, unsigned short opcode);

		void CaptureCsrRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId);

		void ApplyCsrEffect(CsrRetireEffect effect, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		ExecutionResult ExecuteSmtVt(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned long long bundleSerial, unsigned char vtId);

		static void CaptureSmtVtRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned long long bundleSerial, unsigned char vtId);

		inline static Core::SystemEventOrderGuarantee ResolveRetireWindowPublicationSmtVtOrderGuarantee(unsigned short opcode);



		// ── Memory execution unit ─────────────────────────────────────────────

		/// <summary>
		/// Memory execution unit.  When a <see cref="MemoryUnit"/> is attached,
		/// typed load/store operations (LB–LD, SB–SD) are fully executed with
		/// correct sign/zero extension and alignment checking (Phase 07).
		/// When no memory unit is available (legacy or EA-only mode), the method
		/// validates the opcode and returns the effective address for the LSU
		/// pipeline stage to complete.
		/// </summary>
		ExecutionResult ExecuteMemory(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		void CaptureMemoryRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Rs1Value, ulong Rs2Value) ResolveMemoryOperandValues(InstructionIR instr, ICanonicalCpuState state, byte vtId)
//		{
//			ulong rs1Value = instr.HasAbsoluteAddressing ? 0UL : ReadExecutionRegister(state, vtId, instr.Rs1);
//			ulong rs2Value = RequiresMemoryStoreSourceValue(instr) ? ReadExecutionRegister(state, vtId, instr.Rs2) : 0UL;
//			return (rs1Value, rs2Value);
//		}

		inline static bool RequiresMemoryStoreSourceValue(std::shared_ptr<InstructionIR> instr);

		static std::pair<unsigned long long, unsigned long long> ResolveMemoryOperandValues(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		// ── ControlFlow execution unit ────────────────────────────────────────

		static ExecutionResult ResolveControlFlowResult(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		static ExecutionResult ExecuteControlFlow(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		static void CaptureControlFlowRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId);

		// ── Atomic execution unit ─────────────────────────────────────────────

		ExecutionResult ExecuteAtomic(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		// ── System execution unit ─────────────────────────────────────────────


		// ── ScalarAlu execution unit ─────────────────────────────────────────

		inline static void ThrowUnsupportedRetireWindowPublicationMemoryOpcode(std::shared_ptr<InstructionIR> instr);

		inline static void ThrowMissingMemoryUnitForRetireWindowPublication(std::shared_ptr<InstructionIR> instr);

		static unsigned long long ResolveScalarAluValue(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		static ExecutionResult ExecuteScalarAlu(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		static void CaptureScalarAluRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId);

		void CaptureAtomicRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId);



		ExecutionResult ExecuteSystem(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned long long bundleSerial, unsigned char vtId);

		// ── CSR execution unit (Phase 08 — full implementation) ──────────────

		static void CaptureSystemRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned long long bundleSerial, unsigned char vtId);

		inline static Core::SystemEventOrderGuarantee ResolveRetireWindowPublicationSystemOrderGuarantee(unsigned short opcode);

		static void ValidateCanonicalFenceInstruction(std::shared_ptr<InstructionIR> instr, unsigned short opcode);


		void CaptureVmxRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId);

		ExecutionResult ExecuteVmx(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		static VmxRetireEffect CreateRemovedFrontendFaultEffect(std::shared_ptr<InstructionIR> instr);

		inline ExecutionResult EnqueuePipelineEvent(std::shared_ptr<PipelineEvent> evt);
	};
}
