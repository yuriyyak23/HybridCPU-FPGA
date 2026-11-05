#include "Scalar/ALU/ScalarAluOps.h"
#include "YAKSys_Hybrid_CPU.Core.Execution.ExecutionDispatcherV4.h"
#include "Scalar/ALU/UInt64Multiply.h"
#include <codecvt>
#include <locale>
#include <string>
#include "../Pipeline/MicroOps/System/MicroOp.System.h"
#include "../Pipeline/MicroOps/Control/MicroOp.Control.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../NonRTL/Arch/OpcodeInfo.Types.h"
#include "../Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "../../../NonRTL/Arch/InvalidOpcodeException.h"
#include "../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "Memory/AtomicMemory/YAKSys_Hybrid_CPU.Core.Memory.MainMemoryAtomicMemoryUnit.h"
#include "../Diagnostics/V4TraceEvent.h"
#include "../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../../../Arch/CsrAddresses.h"
#include "../Architecture/Registers/Retire/RetireRecord.h"
#include "Memory/AtomicMemory/AtomicMemoryUnit.h"

// Windows headers can define these compatibility macros. They rewrite the
// ScalarAluOps API names and create references to non-existent _rotl64/_rotr64
// members in this translation unit.
#ifdef RotateLeft64
#undef RotateLeft64
#endif
#ifdef RotateRight64
#undef RotateRight64
#endif

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using CpuCore = YAKSys_Hybrid_CPU::Processor::CPU_Core;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Execution
{
	namespace
	{
		std::string ToNarrow(const std::wstring &value)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(value);
		}

		std::wstring ToWide(const std::string &value)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(value);
		}

		bool HasFlag(InstructionFlags value, InstructionFlags flag)
		{
			return (static_cast<int>(value) & static_cast<int>(flag)) != 0;
		}
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifyTraceEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result)
	{
		switch (instr->Class)
		{
		case InstructionClass::ScalarAlu: return TraceEventKind::AluExecuted;
		case InstructionClass::Memory: return ClassifyMemoryEvent(instr);
		case InstructionClass::ControlFlow: return ClassifyControlFlowEvent(instr, result);
		case InstructionClass::Atomic: return ClassifyAtomicEvent(instr, result);
		case InstructionClass::System: return ClassifySystemEvent(instr);
		case InstructionClass::Csr: return ClassifyCsrEvent(instr);
		case InstructionClass::SmtVt: return ClassifySmtVtEvent(instr);
		case InstructionClass::Vmx: return ClassifyVmxEvent(instr, result);
		default: return TraceEventKind::AluExecuted;
		}
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifyMemoryEvent(std::shared_ptr<InstructionIR> instr)
	{
		return instr->SerializationClass == SerializationClass::MemoryOrdered ? TraceEventKind::StoreExecuted : TraceEventKind::LoadExecuted;
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifyControlFlowEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result)
	{
		unsigned short opcode = ResolveOpcode(instr);
		if (opcode == CpuCore::IsaOpcodeValues::JAL || opcode == CpuCore::IsaOpcodeValues::JALR)
			return TraceEventKind::JumpExecuted;
		return result.getPcRedirected() ? TraceEventKind::BranchTaken : TraceEventKind::BranchNotTaken;
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifyAtomicEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result)
	{
		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(instr->CanonicalOpcode->Value);
		if (opcodeInfo && opcodeInfo.value().InstructionClass == InstructionClass::Atomic)
		{
			InstructionFlags flags = opcodeInfo.value().Flags;
			if (opcodeInfo.value().OperandCount == 1 && HasFlag(flags, InstructionFlags::MemoryRead) && !HasFlag(flags, InstructionFlags::MemoryWrite))
			{
				return TraceEventKind::LrExecuted;
			}

			if (opcodeInfo.value().OperandCount == 2 && !HasFlag(flags, InstructionFlags::MemoryRead) && HasFlag(flags, InstructionFlags::MemoryWrite))
			{
				return result.getValue() == 0 ? TraceEventKind::ScSucceeded : TraceEventKind::ScFailed;
			}

			if ((static_cast<int>(flags) & (static_cast<int>(InstructionFlags::MemoryRead) | static_cast<int>(InstructionFlags::MemoryWrite))) == (static_cast<int>(InstructionFlags::MemoryRead) | static_cast<int>(InstructionFlags::MemoryWrite)))
			{
				unsigned char accessSize;
				if (!InstructionRegistry::TryResolvePublishedAtomicAccessSize(instr, accessSize))
				{
					throw InvalidOperationException(std::format("Published atomic opcode {0} did not resolve an authoritative access size for trace classification.", ToNarrow(opcodeInfo.value().Mnemonic)));
				}

				return accessSize > 4 ? TraceEventKind::AmoDwordExecuted : TraceEventKind::AmoWordExecuted;
			}
		}

		return TraceEventKind::AmoWordExecuted;
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifySystemEvent(std::shared_ptr<InstructionIR> instr)
	{
		SystemEventKind systemEventKind;
		if (InstructionRegistry::TryResolvePublishedSystemEventKind(instr, systemEventKind))
		{
			switch (systemEventKind)
			{
			case SystemEventKind::Ecall:
			case SystemEventKind::Ebreak: return TraceEventKind::TrapTaken;
			case SystemEventKind::Mret:
			case SystemEventKind::Sret: return TraceEventKind::PrivilegeReturn;
			case SystemEventKind::Wfi: return TraceEventKind::WfiEntered;
			default: return TraceEventKind::FenceExecuted;
			}
		}

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(instr->CanonicalOpcode->Value);
		if (opcodeInfo && opcodeInfo.value().InstructionClass == InstructionClass::System)
		{
			if (!opcodeInfo.value().isVector() && !HasFlag(opcodeInfo.value().Flags, InstructionFlags::Privileged) && opcodeInfo.value().OperandCount == 0)
			{
				return TraceEventKind::FenceExecuted;
			}
		}

		return TraceEventKind::FenceExecuted;
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifyCsrEvent(std::shared_ptr<InstructionIR> instr)
	{
		std::shared_ptr<CSRMicroOp> csrMicroOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (InstructionRegistry.TryCreatePublishedCsrMicroOp(in instr, out CSRMicroOp? csrMicroOp) && csrMicroOp != null)
		if (InstructionRegistry::TryCreatePublishedCsrMicroOp(instr, csrMicroOp) && csrMicroOp != nullptr)
		{
			if (std::dynamic_pointer_cast<CsrClearMicroOp>(csrMicroOp) ||
				std::dynamic_pointer_cast<CsrReadWriteMicroOp>(csrMicroOp) ||
				std::dynamic_pointer_cast<CsrReadWriteImmediateMicroOp>(csrMicroOp))
				return TraceEventKind::CsrWrite;
		}

		return TraceEventKind::CsrRead;
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifySmtVtEvent(std::shared_ptr<InstructionIR> instr)
	{
		SystemEventKind systemEventKind;
		if (InstructionRegistry::TryResolvePublishedSystemEventKind(instr, systemEventKind))
		{
			switch (systemEventKind)
			{
			case SystemEventKind::Wfe: return TraceEventKind::VtWfe;
			case SystemEventKind::Sev: return TraceEventKind::VtSev;
			case SystemEventKind::PodBarrier: return TraceEventKind::PodBarrierEntered;
			case SystemEventKind::VtBarrier: return TraceEventKind::VtBarrierEntered;
			default: return TraceEventKind::VtYield;
			}
		}

		return TraceEventKind::VtYield;
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind ExecutionDispatcherV4::ClassifyVmxEvent(std::shared_ptr<InstructionIR> instr, ExecutionResult result)
	{
		VmxOperationKind operationKind;
		if (InstructionRegistry::TryResolvePublishedVmxOperationKind(instr, operationKind))
		{
			switch (operationKind)
			{
			case VmxOperationKind::VmxOn: return TraceEventKind::VmxOn;
			case VmxOperationKind::VmxOff: return TraceEventKind::VmxOff;
			case VmxOperationKind::VmLaunch:
			case VmxOperationKind::VmResume: return result.getVmxFaulted() ? TraceEventKind::VmEntryFailed : TraceEventKind::VmEntry;
			case VmxOperationKind::VmRead: return TraceEventKind::VmcsRead;
			case VmxOperationKind::VmWrite:
			case VmxOperationKind::VmClear:
			case VmxOperationKind::VmPtrLd: return TraceEventKind::VmcsWrite;
			default: return TraceEventKind::VmxOn;
			}
		}

		return TraceEventKind::VmxOn;
	}

	unsigned long long ExecutionDispatcherV4::GetTracePayload(std::shared_ptr<InstructionIR> instr, TraceEventKind kind)
	{
		unsigned long long temp_var29;
		switch (kind)
		{
			case TraceEventKind::CsrRead:
			case TraceEventKind::CsrWrite:
				temp_var29 = ResolveCsrTracePayload(instr);
				break;
			default:
				temp_var29 = 0ULL;
				break;
		}
		return temp_var29;
	}

	unsigned long long ExecutionDispatcherV4::ResolveCsrTracePayload(std::shared_ptr<InstructionIR> instr)
	{
		std::shared_ptr<CSRMicroOp> csrMicroOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (InstructionRegistry.TryCreatePublishedCsrMicroOp(in instr, out CSRMicroOp? csrMicroOp) && csrMicroOp != null)
		if (InstructionRegistry::TryCreatePublishedCsrMicroOp(instr, csrMicroOp) && csrMicroOp != nullptr)
		{
			return std::dynamic_pointer_cast<CsrClearMicroOp>(csrMicroOp) != nullptr ? 0ULL : csrMicroOp->getCSRAddress() & 0xFFFULL;
		}

		return static_cast<unsigned long long>(instr->Imm & 0xFFF);
	}

	inline unsigned long long ExecutionDispatcherV4::ReadExecutionRegister(std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId, unsigned short regId)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)state.ReadRegister(vtId, regId));
		return static_cast<unsigned long long>(state->ReadRegister(vtId, regId));
	}

	inline unsigned long long ExecutionDispatcherV4::ReadExecutionPc(std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		return state->ReadPc(vtId);
	}

	inline std::shared_ptr<InvalidOperationException> ExecutionDispatcherV4::CreateUnsupportedVectorConfigEagerExecuteSurfaceException(std::shared_ptr<InstructionIR> instr)
	{
		return std::make_shared<InvalidOperationException>(std::format("Vector-config opcode {0} does not expose an authoritative eager execute contour. ", ToNarrow(FormatOpcode(ResolveOpcode(instr)))) + "Direct callers must use the canonical mainline retire path so lane-7 VectorConfig state updates and optional rd writeback stay coupled on the authoritative system-singleton carrier.");
	}

	inline std::shared_ptr<InvalidOperationException> ExecutionDispatcherV4::CreateUnsupportedVectorConfigRetireWindowPublicationSurfaceException(std::shared_ptr<InstructionIR> instr)
	{
		return std::make_shared<InvalidOperationException>(std::format("Vector-config opcode {0} does not expose an authoritative retire-window publication contour. ", ToNarrow(FormatOpcode(ResolveOpcode(instr)))) + "Direct callers must use the canonical mainline retire path so deferred VectorConfig state publication and serializing-boundary follow-through remain coupled on the authoritative lane-7 carrier.");
	}

	inline std::shared_ptr<Core::Pipeline::PipelineEvent> ExecutionDispatcherV4::ThrowUnsupportedRetireWindowPublicationSystemOpcode(std::shared_ptr<InstructionIR> instr)
	{
		throw InvalidOperationException(std::format("System opcode {0} does not have an authoritative retire-window publication carrier; pipeline execution remains the supported path.", ToNarrow(FormatOpcode(ResolveOpcode(instr)))));
	}

	inline std::shared_ptr<Core::Pipeline::PipelineEvent> ExecutionDispatcherV4::ThrowUnsupportedRetireWindowPublicationSmtVtOpcode(std::shared_ptr<InstructionIR> instr)
	{
		throw InvalidOperationException(std::format("SMT/VT opcode {0} does not expose an authoritative path through retire-window publication here; pipeline execution remains the supported path.", ToNarrow(FormatOpcode(ResolveOpcode(instr)))));
	}

	inline ExecutionResult ExecutionDispatcherV4::ThrowUnsupportedEagerStreamControlOpcode(std::shared_ptr<InstructionIR> instr)
	{
		bool requiresSerializingBoundaryFollowThrough;
		if (InstructionRegistry::TryResolvePublishedStreamControlRetireContour(instr, requiresSerializingBoundaryFollowThrough))
		{
			if (requiresSerializingBoundaryFollowThrough)
			{
				throw InvalidOperationException(std::wstring(L"STREAM_WAIT does not expose an authoritative eager execute contour inside ExecutionDispatcherV4. ") + L"Direct callers must use CaptureRetireWindowPublications(...) or the mainline retire path instead of relying on an inner-unit stub success.");
			}

			std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(instr->CanonicalOpcode->Value);

			throw InvalidOperationException(std::format("Stream-control opcode {0} does not expose an authoritative eager execute contour inside ExecutionDispatcherV4. ", (opcodeInfo ? ToNarrow(opcodeInfo.value().Mnemonic) : ToNarrow(FormatOpcode(ResolveOpcode(instr))))) + "Direct callers must reject/defer it until an explicit retire/apply contour lands instead of relying on an inner-unit stub success.");
		}

		unsigned short opcode = ResolveOpcode(instr);

		throw InvalidOperationException(std::format("Stream-control opcode {0} does not expose an authoritative eager execute contour inside ExecutionDispatcherV4. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must reject/defer it until an explicit retire/apply contour lands instead of relying on an inner-unit stub success.");
	}

	inline bool ExecutionDispatcherV4::RequiresWiredPipelineEventQueueForEagerExecute(std::shared_ptr<InstructionIR> instr)
	{
		if (instr->Class != InstructionClass::SmtVt)
		{
			return false;
		}

		bool requiresSerializingBoundaryFollowThrough;
		if (InstructionRegistry::TryResolvePublishedStreamControlRetireContour(instr, requiresSerializingBoundaryFollowThrough))
		{
			return !requiresSerializingBoundaryFollowThrough;
		}

		SystemEventKind systemEventKind;
		return InstructionRegistry::TryResolvePublishedSystemEventKind(instr, systemEventKind);
	}

	inline bool ExecutionDispatcherV4::IsPipelineOnlyVectorConfigSystemSurface(unsigned short opcode)
	{
		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(opcode));
		if (opcodeInfo)
		{
			return opcodeInfo.value().InstructionClass == InstructionClass::System && opcodeInfo.value().isVector();
		}

		return false;
	}

	inline bool ExecutionDispatcherV4::IsRetireWindowPublicationOnlySystemEagerExecuteSurface(unsigned short opcode)
	{
		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(opcode));
		if (opcodeInfo && opcodeInfo.value().InstructionClass == InstructionClass::System)
		{
			return !opcodeInfo.value().isVector() && opcodeInfo.value().OperandCount == 0;
		}

		return false;
	}

	inline bool ExecutionDispatcherV4::IsRetireWindowPublicationOnlySmtVtEagerExecuteSurface(std::shared_ptr<InstructionIR> instr)
	{
		return instr->Class == InstructionClass::SmtVt;
	}

	inline bool ExecutionDispatcherV4::IsRetireWindowPublicationOnlyVmxEagerExecuteSurface(unsigned short opcode)
	{
		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(opcode));
		if (opcodeInfo)
		{
			return opcodeInfo.value().InstructionClass == InstructionClass::Vmx;
		}

		return false;
	}

	inline bool ExecutionDispatcherV4::IsPipelineOnlyMemoryEagerExecuteSurface(unsigned short opcode)
	{
		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(opcode));
		if (opcodeInfo)
		{
			return opcodeInfo.value().InstructionClass == InstructionClass::Memory && opcodeInfo.value().isVector();
		}

		return (opcode == CpuCore::IsaOpcodeValues::MTILE_LOAD || opcode == CpuCore::IsaOpcodeValues::MTILE_STORE);
	}

	inline bool ExecutionDispatcherV4::RequiresRetireWindowPublicationSerializingBoundaryFollowThrough(std::shared_ptr<InstructionIR> instr)
	{
		return instr->SerializationClass == SerializationClass::FullSerial || instr->SerializationClass == SerializationClass::VmxSerial;
	}

	std::shared_ptr<IAtomicMemoryUnit> ExecutionDispatcherV4::CreateDefaultAtomicMemoryUnit()
	{
		return std::make_shared<MainMemoryAtomicMemoryUnit>(Processor::MainMemory);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionDispatcherV4(IAtomicMemoryBus? atomicBus = null, System.Nullable<IAtomicMemoryUnit> atomicMemoryUnit = null, System.Nullable<MemoryUnit> memoryUnit = null, System.Nullable<CsrFile> csrFile = null, System.Nullable<IV4TraceEventSink> traceSink = null, System.Nullable<TelemetryCounters> telemetry = null, System.Nullable<IPipelineEventQueue> pipelineEventQueue = null)
	ExecutionDispatcherV4::ExecutionDispatcherV4(std::shared_ptr<IAtomicMemoryBus> atomicBus, std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit, std::shared_ptr<MemoryUnit> memoryUnit, std::shared_ptr<CsrFile> csrFile, std::shared_ptr<IV4TraceEventSink> traceSink, std::shared_ptr<TelemetryCounters> telemetry, std::shared_ptr<IPipelineEventQueue> pipelineEventQueue) : _atomicBus(atomicBus), _atomicMemoryUnit((atomicMemoryUnit != nullptr) ? atomicMemoryUnit : CreateDefaultAtomicMemoryUnit()), _memoryUnit(memoryUnit), _csrFile(csrFile), _traceSink((traceSink != nullptr) ? traceSink : NullV4TraceEventSink::Instance), _telemetry(telemetry), _pipelineEventQueue((pipelineEventQueue != nullptr) ? pipelineEventQueue : NullPipelineEventQueue::Instance)
	{
	}

	inline unsigned short ExecutionDispatcherV4::ResolveOpcode(std::shared_ptr<InstructionIR> instr)
	{
		return instr->CanonicalOpcode->Value;
	}

	inline std::wstring ExecutionDispatcherV4::FormatOpcode(unsigned short opcode)
	{
		return OpcodeRegistry::GetMnemonicOrHex(opcode);
	}

	inline bool ExecutionDispatcherV4::CanRouteToConfiguredExecutionSurface(std::shared_ptr<InstructionIR> instr)
	{
		unsigned short opcode = ResolveOpcode(instr);

		bool requiresBoundaryFollowThrough = false;
		if (InstructionRegistry::TryResolvePublishedStreamControlRetireContour(instr, requiresBoundaryFollowThrough))
		{
			return false;
		}

		if (IsPipelineOnlyVectorExceptionControlCsrSurface(opcode))
		{
			return false;
		}

		if (IsPipelineOnlyVectorConfigSystemSurface(opcode))
		{
			return false;
		}

		if (RequiresWiredCsrFileForExecutionSurface(instr))
		{
			return false;
		}

		if (IsRetireWindowPublicationOnlySystemEagerExecuteSurface(opcode) || IsRetireWindowPublicationOnlySmtVtEagerExecuteSurface(instr) || IsRetireWindowPublicationOnlyVmxEagerExecuteSurface(opcode) || IsPipelineOnlyMemoryEagerExecuteSurface(opcode))
		{
			return false;
		}

		if (RequiresWiredPipelineEventQueueForEagerExecute(instr) && std::dynamic_pointer_cast<NullPipelineEventQueue>(_pipelineEventQueue) != nullptr)
		{
			return false;
		}

		bool temp_var33;
		switch (instr->Class)
		{
			case InstructionClass::Atomic:
				temp_var33 = false;
				break;
			case InstructionClass::Vmx:
				temp_var33 = true;
				break;
			default:
				temp_var33 = true;
				break;
		}
		return temp_var33;
	}

	inline void ExecutionDispatcherV4::EnforceConfiguredExecutionSurfaceContract(std::shared_ptr<InstructionIR> instr)
	{
		unsigned short opcode = ResolveOpcode(instr);

		if (CanRouteToConfiguredExecutionSurface(instr))
		{
			return;
		}

		if (instr->Class == InstructionClass::Atomic)
		{
			throw InvalidOperationException(std::format("Atomic opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative retire-window publication follow-through. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must use CaptureRetireWindowPublications(...) or the mainline retire path instead of publishing eager success/trace on the Atomic plane.");
		}

		bool requiresSerializingBoundaryFollowThrough;
		if (InstructionRegistry::TryResolvePublishedStreamControlRetireContour(instr, requiresSerializingBoundaryFollowThrough))
		{
			if (requiresSerializingBoundaryFollowThrough)
			{
				throw InvalidOperationException(std::wstring(L"STREAM_WAIT reached ExecutionDispatcherV4 eager execute surface without an authoritative retire-window publication follow-through. ") + L"Direct callers must use CaptureRetireWindowPublications(...) or the mainline retire path so the serializing-boundary effect is applied explicitly instead of publishing stub success/trace.");
			}

			throw InvalidOperationException(std::format("Stream-control opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative direct follow-through. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must reject/defer it instead of publishing stub success/trace until an explicit retire/apply contour lands.");
		}

		if (IsPipelineOnlyVectorExceptionControlCsrSurface(opcode))
		{
			throw InvalidOperationException(std::format("Vector exception control opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative direct follow-through. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must decode it through the canonical mainline CSR/materializer surface and retire it on the lane-7 pipeline path instead of publishing eager success/trace on a non-CsrFile-backed CSR plane.");
		}

		if (IsPipelineOnlyVectorConfigSystemSurface(opcode))
		{
			throw CreateUnsupportedVectorConfigEagerExecuteSurfaceException(instr);
		}

		if (RequiresWiredCsrFileForExecutionSurface(instr))
		{
			throw CreateMissingCsrFileSurfaceException(instr);
		}

		if (IsPipelineOnlyMemoryEagerExecuteSurface(opcode))
		{
			throw InvalidOperationException(std::format("Memory opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative direct retire/apply follow-through. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must reject/defer it instead of publishing EA-only success/trace on a non-representable memory surface; pipeline execution remains the supported path.");
		}

		if (IsRetireWindowPublicationOnlySystemEagerExecuteSurface(opcode))
		{
			throw InvalidOperationException(std::format("System opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative retire-window publication follow-through. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must use CaptureRetireWindowPublications(...) or the mainline retire path instead of enqueueing the typed event directly while still publishing eager success/trace.");
		}

		if (IsRetireWindowPublicationOnlySmtVtEagerExecuteSurface(instr))
		{
			throw InvalidOperationException(std::format("SMT/VT opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative retire-window publication follow-through. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must use CaptureRetireWindowPublications(...) or the mainline retire path instead of enqueueing the typed event directly while still publishing eager success/trace.");
		}

		if (IsRetireWindowPublicationOnlyVmxEagerExecuteSurface(opcode))
		{
			throw InvalidOperationException(std::format("VMX opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative retire-window publication follow-through. Direct callers must use CaptureRetireWindowPublications(...) or the mainline retire path instead of mutating compatibility-visible state through eager success/trace publication.", ToNarrow(FormatOpcode(opcode))));
		}

		if (RequiresWiredPipelineEventQueueForEagerExecute(instr) && std::dynamic_pointer_cast<NullPipelineEventQueue>(_pipelineEventQueue) != nullptr)
		{
			throw InvalidOperationException(std::format("Opcode {0} reached ExecutionDispatcherV4 eager execute surface without a wired pipeline-event queue. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must provide an IPipelineEventQueue or use CaptureRetireWindowPublications(...) / the mainline retire path instead of dropping the typed event on NullPipelineEventQueue while still publishing success/trace.");
		}

		throw InvalidOperationException(std::format("Opcode {0} reached ExecutionDispatcherV4 without a supported execution surface. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must reject/defer it through the explicit surface contract before execution.");
	}

	inline void ExecutionDispatcherV4::EnforceConfiguredRetireWindowPublicationSurfaceContract(std::shared_ptr<InstructionIR> instr)
	{
		unsigned short opcode = ResolveOpcode(instr);

		if (IsPipelineOnlyVectorExceptionControlCsrSurface(opcode))
		{
			throw InvalidOperationException(std::format("Vector exception control opcode {0} reached ExecutionDispatcherV4.CaptureRetireWindowPublications(...) without an authoritative retire-window publication contour. ", ToNarrow(FormatOpcode(opcode))) + "Direct callers must use the canonical mainline retire path so the non-CsrFile-backed CSR write and serializing-boundary follow-through stay coupled on the authoritative lane-7 pipeline contour.");
		}

		if (IsPipelineOnlyVectorConfigSystemSurface(opcode))
		{
			throw CreateUnsupportedVectorConfigRetireWindowPublicationSurfaceException(instr);
		}

		if (RequiresWiredCsrFileForExecutionSurface(instr))
		{
			throw CreateMissingCsrFileSurfaceException(instr);
		}

	}

	inline bool ExecutionDispatcherV4::RequiresWiredCsrFileForExecutionSurface(std::shared_ptr<InstructionIR> instr)
	{
		if (instr->Class != InstructionClass::Csr || _csrFile != nullptr)
		{
			return false;
		}

		std::shared_ptr<CSRMicroOp> csrMicroOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (InstructionRegistry.TryCreatePublishedCsrMicroOp(in instr, out CSRMicroOp? csrMicroOp))
		if (InstructionRegistry::TryCreatePublishedCsrMicroOp(instr, csrMicroOp))
		{
			return std::dynamic_pointer_cast<CsrClearMicroOp>(csrMicroOp) == nullptr;
		}

		return true;
	}

	inline bool ExecutionDispatcherV4::IsPipelineOnlyVectorExceptionControlCsrSurface(unsigned short opcode)
	{
		return (opcode == CpuCore::IsaOpcodeValues::VSETVEXCPMASK || opcode == CpuCore::IsaOpcodeValues::VSETVEXCPPRI);
	}

	std::shared_ptr<InvalidOperationException> ExecutionDispatcherV4::CreateMissingCsrFileSurfaceException(std::shared_ptr<InstructionIR> instr)
	{
		unsigned short opcode = ResolveOpcode(instr);

		return std::make_shared<InvalidOperationException>(std::format("CSR opcode {0} reached ExecutionDispatcherV4 without a wired CsrFile. ", ToNarrow(FormatOpcode(opcode))) + "This dispatcher does not expose an authoritative eager or retire-window publication contour for non-CsrFile-backed CSR surfaces; " + "direct callers must wire the CsrFile and use CaptureRetireWindowPublications(...) or the mainline retire path.");
	}

	inline ExecutionResult ExecutionDispatcherV4::Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state)
	{
		return Execute(instr, state, 0, 0);
	}

	ExecutionResult ExecutionDispatcherV4::Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned long long bundleSerial, unsigned char vtId)
	{
		EnforceConfiguredExecutionSurfaceContract(instr);

		ExecutionResult result;
		switch (instr->Class)
		{
		case InstructionClass::ScalarAlu: result = ExecuteScalarAlu(instr, state, vtId); break;
		case InstructionClass::Memory: result = ExecuteMemory(instr, state, vtId); break;
		case InstructionClass::ControlFlow: result = ExecuteControlFlow(instr, state, vtId); break;
		case InstructionClass::Atomic: result = ExecuteAtomic(instr, state, vtId); break;
		case InstructionClass::System: result = ExecuteSystem(instr, state, bundleSerial, vtId); break;
		case InstructionClass::Csr: result = ExecuteCsr(instr, state, vtId); break;
		case InstructionClass::SmtVt: result = ExecuteSmtVt(instr, state, bundleSerial, vtId); break;
		case InstructionClass::Vmx: result = ExecuteVmx(instr, state, vtId); break;
		default: throw std::logic_error("Unknown instruction class in execution dispatcher.");
		}

		TraceEventKind kind = ClassifyTraceEvent(instr, result);
		V4TraceEvent evt = V4TraceEvent::Create(bundleSerial, vtId, state->GetCurrentPipelineState(), kind, GetTracePayload(instr, kind));
		_traceSink->RecordV4Event(evt);
		if (_telemetry != nullptr)
		{
			_telemetry->ApplyTraceEvent(evt);
		}
		return result;
	}

	inline void ExecutionDispatcherV4::CaptureRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch)
	{
		CaptureRetireWindowPublications(instr, state, retireBatch, 0, 0);
	}

	void ExecutionDispatcherV4::CaptureRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned long long bundleSerial, unsigned char vtId)
	{
		EnforceConfiguredRetireWindowPublicationSurfaceContract(instr);

		switch (instr->Class)
		{
			case InstructionClass::ScalarAlu:
				CaptureScalarAluRetireWindowPublications(instr, state, retireBatch, vtId);
				return;

			case InstructionClass::ControlFlow:
				CaptureControlFlowRetireWindowPublications(instr, state, retireBatch, vtId);
				return;

			case InstructionClass::Csr:
				CaptureCsrRetireWindowPublications(instr, state, retireBatch, vtId);
				return;

			case InstructionClass::System:
				CaptureSystemRetireWindowPublications(instr, state, retireBatch, bundleSerial, vtId);
				return;

			case InstructionClass::Vmx:
				CaptureVmxRetireWindowPublications(instr, state, retireBatch, vtId);
				return;

			case InstructionClass::SmtVt:
				CaptureSmtVtRetireWindowPublications(instr, state, retireBatch, bundleSerial, vtId);
				return;

			case InstructionClass::Memory:
				CaptureMemoryRetireWindowPublications(instr, state, retireBatch, vtId);
				return;

			case InstructionClass::Atomic:
				CaptureAtomicRetireWindowPublications(instr, state, retireBatch, vtId);
				return;

			default:
				throw std::logic_error(std::format("Unknown InstructionClass {0}", static_cast<int>(instr->Class)));
		}
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteCsr(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		CsrRetireEffect effect = ResolveCsrEffect(instr, state, vtId);
		ApplyCsrEffect(effect, state, vtId);
		return ExecutionResult::Ok(effect.getReadValue());

	}

	YAKSys_Hybrid_CPU::Core::CsrRetireEffect ExecutionDispatcherV4::ResolveCsrEffect(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);

		if (opcode == CpuCore::IsaOpcodeValues::CSR_CLEAR)
		{
			return CsrRetireEffect::ClearExceptionCounters();
		}

		bool known = (opcode == CpuCore::IsaOpcodeValues::RDCYCLE || opcode == CpuCore::IsaOpcodeValues::CSRRW || opcode == CpuCore::IsaOpcodeValues::CSRRS || opcode == CpuCore::IsaOpcodeValues::CSRRC || opcode == CpuCore::IsaOpcodeValues::CSRRWI || opcode == CpuCore::IsaOpcodeValues::CSRRSI || opcode == CpuCore::IsaOpcodeValues::CSRRCI);
		if (!known)
		{
			throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in CSR execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}

		if (_csrFile == nullptr)
		{
			throw CreateMissingCsrFileSurfaceException(instr);
		}

		constexpr PrivilegeLevel privilege = PrivilegeLevel::Machine;
		ValidateCounterReadCsrInstruction(instr, opcode);

		unsigned short csrAddr = opcode == CpuCore::IsaOpcodeValues::RDCYCLE ? CsrAddresses::Cycle : static_cast<unsigned short>(instr->Imm & 0xFFF);
		unsigned long long oldValue = _csrFile->Read(csrAddr, privilege);
		bool hasRegisterWriteback = instr->Rd != 0;
		unsigned short destRegId = instr->Rd;
		unsigned long long immediateValue = static_cast<unsigned long long>(instr->Rs1 & 0x1F);

		switch (opcode)
		{
		case CpuCore::IsaOpcodeValues::RDCYCLE:
			return CsrRetireEffect::Create(CsrStorageSurface::WiredCsrFile, csrAddr, oldValue, hasRegisterWriteback, destRegId, false, 0);
		case CpuCore::IsaOpcodeValues::CSRRW:
			return CsrRetireEffect::Create(CsrStorageSurface::WiredCsrFile, csrAddr, oldValue, hasRegisterWriteback, destRegId, true, ReadExecutionRegister(state, vtId, instr->Rs1));
		case CpuCore::IsaOpcodeValues::CSRRS:
			return CsrRetireEffect::Create(CsrStorageSurface::WiredCsrFile, csrAddr, oldValue, hasRegisterWriteback, destRegId, instr->Rs1 != 0,
				instr->Rs1 != 0 ? oldValue | ReadExecutionRegister(state, vtId, instr->Rs1) : 0);
		case CpuCore::IsaOpcodeValues::CSRRC:
			return CsrRetireEffect::Create(CsrStorageSurface::WiredCsrFile, csrAddr, oldValue, hasRegisterWriteback, destRegId, instr->Rs1 != 0,
				instr->Rs1 != 0 ? oldValue & ~ReadExecutionRegister(state, vtId, instr->Rs1) : 0);
		case CpuCore::IsaOpcodeValues::CSRRWI:
			return CsrRetireEffect::Create(CsrStorageSurface::WiredCsrFile, csrAddr, oldValue, hasRegisterWriteback, destRegId, true, immediateValue);
		case CpuCore::IsaOpcodeValues::CSRRSI:
			return CsrRetireEffect::Create(CsrStorageSurface::WiredCsrFile, csrAddr, oldValue, hasRegisterWriteback, destRegId, immediateValue != 0,
				immediateValue != 0 ? oldValue | immediateValue : 0);
		case CpuCore::IsaOpcodeValues::CSRRCI:
			return CsrRetireEffect::Create(CsrStorageSurface::WiredCsrFile, csrAddr, oldValue, hasRegisterWriteback, destRegId, immediateValue != 0,
				immediateValue != 0 ? oldValue & ~immediateValue : 0);
		default:
			throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in CSR execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}
	}

	void ExecutionDispatcherV4::ValidateCounterReadCsrInstruction(std::shared_ptr<InstructionIR> instr, unsigned short opcode)
	{
		if (opcode != CpuCore::IsaOpcodeValues::RDCYCLE)
		{
			return;
		}

		if (instr->Rs1 != 0 || instr->Rs2 != 0 || instr->Imm != 0 || (instr->CsrAddress.has_value() && instr->CsrAddress.value() != CsrAddresses::Cycle))
		{
			throw InvalidOpcodeException(L"RDCYCLE requires canonical rd-only CSR counter-read payload: rs1=x0, rs2=x0, Immediate=0, CsrAddress=0xC00.", FormatOpcode(opcode), -1, false);
		}
	}

	void ExecutionDispatcherV4::CaptureCsrRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId)
	{
		std::optional<CsrRetireEffect> effect;
		try
		{
			effect.emplace(ResolveCsrEffect(instr, state, vtId));
		}
		catch (const CsrUnknownAddressException &ex)
		{
			unsigned short opcode = ResolveOpcode(instr);
			throw InvalidOperationException(ToWide(std::format("CSR opcode {0} address 0x{1:03X} does not expose an authoritative direct compat retire contract here; pipeline execution remains the authoritative path for non-CsrFile-backed CSR surfaces. Cause: {2}", ToNarrow(FormatOpcode(opcode)), (instr->Imm & 0xFFF), ex.what())));
		}

		if (effect->getHasRegisterWriteback())
		{
			retireBatch.AppendRetireRecord(RetireRecord::RegisterWrite(vtId, effect->getDestRegId(), effect->getReadValue()));
			effect.emplace(CsrRetireEffect::Create(effect->getStorageSurface(), effect->getCsrAddress(), effect->getReadValue(), false, 0, effect->getHasCsrWrite(), effect->getCsrWriteValue()));
		}

		if (!effect->getClearsArchitecturalExceptionState() && !effect->getHasCsrWrite() && !effect->getHasRegisterWriteback())
		{
			return;
		}

		retireBatch.CaptureRetireWindowCsrEffect(*effect);
	}

	void ExecutionDispatcherV4::ApplyCsrEffect(CsrRetireEffect effect, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		if (effect.getClearsArchitecturalExceptionState())
		{
			state->ClearExceptionCounters();
		}

		if (effect.getHasRegisterWriteback())
		{
			state->WriteRegister(vtId, effect.getDestRegId(), effect.getReadValue());
		}

		if (effect.getHasCsrWrite() && _csrFile != nullptr)
		{
			_csrFile->Write(effect.getCsrAddress(), effect.getCsrWriteValue(), PrivilegeLevel::Machine);
		}
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteSmtVt(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned long long bundleSerial, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);

		switch (opcode)
		{
			case CpuCore::IsaOpcodeValues::YIELD:
			{
				std::shared_ptr<YieldEvent> temp_var = std::make_shared<YieldEvent>();
				temp_var->VtId = vtId;
				temp_var->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var);
			}

			case CpuCore::IsaOpcodeValues::WFE:
			{
				std::shared_ptr<WfeEvent> temp_var2 = std::make_shared<WfeEvent>();
				temp_var2->VtId = vtId;
				temp_var2->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var2);
			}

			case CpuCore::IsaOpcodeValues::SEV:
			{
				std::shared_ptr<SevEvent> temp_var3 = std::make_shared<SevEvent>();
				temp_var3->VtId = vtId;
				temp_var3->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var3);
			}

			case CpuCore::IsaOpcodeValues::POD_BARRIER:
			{
				std::shared_ptr<PodBarrierEvent> temp_var4 = std::make_shared<PodBarrierEvent>();
				temp_var4->VtId = vtId;
				temp_var4->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var4);
			}

			case CpuCore::IsaOpcodeValues::VT_BARRIER:
			{
				std::shared_ptr<VtBarrierEvent> temp_var5 = std::make_shared<VtBarrierEvent>();
				temp_var5->VtId = vtId;
				temp_var5->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var5);
			}

			case CpuCore::IsaOpcodeValues::STREAM_WAIT:
				return ThrowUnsupportedEagerStreamControlOpcode(instr);

			default:
				throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in SmtVt execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}
	}

	void ExecutionDispatcherV4::CaptureSmtVtRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned long long bundleSerial, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);

		if (opcode == CpuCore::IsaOpcodeValues::STREAM_WAIT)
		{
			retireBatch.NoteSerializingBoundary();
			return;
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			Core.Pipeline.PipelineEvent pipelineEvent = opcode switch
	//			{
	//				IsaOpcodeValues.YIELD => new YieldEvent
	//				{
	//					VtId = vtId, BundleSerial = bundleSerial
	//				}, IsaOpcodeValues.WFE => new WfeEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				IsaOpcodeValues.SEV => new SevEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				IsaOpcodeValues.POD_BARRIER => new PodBarrierEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				IsaOpcodeValues.VT_BARRIER => new VtBarrierEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				_ => throw new InvalidOpcodeException(
	//					$"Invalid opcode {FormatOpcode(opcode)} in SmtVt execution unit",
	//					FormatOpcode(opcode),
	//					-1,
	//					false)
	//			};

		std::shared_ptr<Core::Pipeline::PipelineEvent> pipelineEvent;
		switch (opcode)
		{
		case CpuCore::IsaOpcodeValues::YIELD: pipelineEvent = std::make_shared<Core::Pipeline::YieldEvent>(); break;
		case CpuCore::IsaOpcodeValues::WFE: pipelineEvent = std::make_shared<Core::Pipeline::WfeEvent>(); break;
		case CpuCore::IsaOpcodeValues::SEV: pipelineEvent = std::make_shared<Core::Pipeline::SevEvent>(); break;
		case CpuCore::IsaOpcodeValues::POD_BARRIER: pipelineEvent = std::make_shared<Core::Pipeline::PodBarrierEvent>(); break;
		case CpuCore::IsaOpcodeValues::VT_BARRIER: pipelineEvent = std::make_shared<Core::Pipeline::VtBarrierEvent>(); break;
		default: throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in SmtVt execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}
		pipelineEvent->VtId = vtId;
		pipelineEvent->BundleSerial = bundleSerial;
		retireBatch.CaptureRetireWindowPipelineEvent(pipelineEvent, ResolveRetireWindowPublicationSmtVtOrderGuarantee(opcode), ReadExecutionPc(state, vtId), vtId, RequiresRetireWindowPublicationSerializingBoundaryFollowThrough(instr));
	}

	inline Core::SystemEventOrderGuarantee ExecutionDispatcherV4::ResolveRetireWindowPublicationSmtVtOrderGuarantee(unsigned short opcode)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opcode switch
	//			{
	//				IsaOpcodeValues.WFE or IsaOpcodeValues.SEV => Core.SystemEventOrderGuarantee.DrainMemory,
	//ignore
	//				IsaOpcodeValues.YIELD or IsaOpcodeValues.POD_BARRIER or IsaOpcodeValues.VT_BARRIER => Core.SystemEventOrderGuarantee.None,
	//ignore
	//ignore
	//				_ => Core.SystemEventOrderGuarantee.None
	//			};
		if (opcode == CpuCore::IsaOpcodeValues::WFE || opcode == CpuCore::IsaOpcodeValues::SEV)
			return Core::SystemEventOrderGuarantee::DrainMemory;
		return Core::SystemEventOrderGuarantee::None;
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteMemory(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);

		// Validate the opcode is a known ISA v4 memory opcode
		bool known = (opcode == CpuCore::IsaOpcodeValues::LB || opcode == CpuCore::IsaOpcodeValues::LBU || opcode == CpuCore::IsaOpcodeValues::LH || opcode == CpuCore::IsaOpcodeValues::LHU || opcode == CpuCore::IsaOpcodeValues::LW || opcode == CpuCore::IsaOpcodeValues::LWU || opcode == CpuCore::IsaOpcodeValues::LD || opcode == CpuCore::IsaOpcodeValues::SB || opcode == CpuCore::IsaOpcodeValues::SH || opcode == CpuCore::IsaOpcodeValues::SW || opcode == CpuCore::IsaOpcodeValues::SD || opcode == CpuCore::IsaOpcodeValues::VGATHER || opcode == CpuCore::IsaOpcodeValues::VSCATTER || opcode == CpuCore::IsaOpcodeValues::MTILE_LOAD || opcode == CpuCore::IsaOpcodeValues::MTILE_STORE);

		if (!known)
		{
			throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in Memory execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}

		// EA-04: typed scalar eager execute resolves access facts only.
		bool isTypedScalar = (opcode == CpuCore::IsaOpcodeValues::LB || opcode == CpuCore::IsaOpcodeValues::LBU || opcode == CpuCore::IsaOpcodeValues::LH || opcode == CpuCore::IsaOpcodeValues::LHU || opcode == CpuCore::IsaOpcodeValues::LW || opcode == CpuCore::IsaOpcodeValues::LWU || opcode == CpuCore::IsaOpcodeValues::LD || opcode == CpuCore::IsaOpcodeValues::SB || opcode == CpuCore::IsaOpcodeValues::SH || opcode == CpuCore::IsaOpcodeValues::SW || opcode == CpuCore::IsaOpcodeValues::SD);

		if (isTypedScalar && _memoryUnit != nullptr)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			auto [rs1Value, rs2Value] = ResolveMemoryOperandValues(instr, state, vtId);
			ResolvedMemoryAccess resolved = _memoryUnit->ResolveArchitecturalAccess(instr, rs1Value, rs2Value);
			return ExecutionResult::Ok(resolved.getEffectiveAddress());
		}

		// EA-only mode: return effective address; LSU completes the access
		unsigned long long eaFallback = static_cast<unsigned long long>(static_cast<long long>(ReadExecutionRegister(state, vtId, instr->Rs1)) + instr->Imm);
		return ExecutionResult::Ok(eaFallback);
	}

	void ExecutionDispatcherV4::CaptureMemoryRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);

		bool supportsRetireWindowPublication = (opcode == CpuCore::IsaOpcodeValues::LB || opcode == CpuCore::IsaOpcodeValues::LBU || opcode == CpuCore::IsaOpcodeValues::LH || opcode == CpuCore::IsaOpcodeValues::LHU || opcode == CpuCore::IsaOpcodeValues::LW || opcode == CpuCore::IsaOpcodeValues::LWU || opcode == CpuCore::IsaOpcodeValues::LD || opcode == CpuCore::IsaOpcodeValues::SB || opcode == CpuCore::IsaOpcodeValues::SH || opcode == CpuCore::IsaOpcodeValues::SW || opcode == CpuCore::IsaOpcodeValues::SD);

		if (!supportsRetireWindowPublication)
		{
			ThrowUnsupportedRetireWindowPublicationMemoryOpcode(instr);
			return;
		}

		if (_memoryUnit == nullptr)
		{
			ThrowMissingMemoryUnitForRetireWindowPublication(instr);
			return;
		}

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
		auto [rs1Value, rs2Value] = ResolveMemoryOperandValues(instr, state, vtId);
		ResolvedMemoryAccess resolved = _memoryUnit->ResolveArchitecturalAccess(instr, rs1Value, rs2Value);

		if (resolved.getHasStoreCommit())
		{
			retireBatch.CaptureRetireWindowScalarMemoryStore(resolved.getEffectiveAddress(), resolved.getStoreData(), resolved.getAccessSize());
			return;
		}

		if (!resolved.getHasRegisterWrite())
		{
			return;
		}

		retireBatch.AppendRetireRecord(RetireRecord::RegisterWrite(vtId, resolved.getRegisterDestination(), resolved.getRegisterWriteValue()));
	}

	inline bool ExecutionDispatcherV4::RequiresMemoryStoreSourceValue(std::shared_ptr<InstructionIR> instr)
	{
		unsigned short opcode = ResolveOpcode(instr);
		return opcode == CpuCore::IsaOpcodeValues::SB || opcode == CpuCore::IsaOpcodeValues::SH || opcode == CpuCore::IsaOpcodeValues::SW || opcode == CpuCore::IsaOpcodeValues::SD;
	}

	std::pair<unsigned long long, unsigned long long> ExecutionDispatcherV4::ResolveMemoryOperandValues(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		unsigned long long rs1Value = instr->HasAbsoluteAddressing ? 0ULL : ReadExecutionRegister(state, vtId, instr->Rs1);
		unsigned long long rs2Value = RequiresMemoryStoreSourceValue(instr) ? ReadExecutionRegister(state, vtId, instr->Rs2) : 0ULL;
		return { rs1Value, rs2Value };
	}

	ExecutionResult ExecutionDispatcherV4::ResolveControlFlowResult(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);
		unsigned long long pc = ReadExecutionPc(state, vtId);
		unsigned long long rs1 = ReadExecutionRegister(state, vtId, instr->Rs1);
		unsigned long long rs2 = ReadExecutionRegister(state, vtId, instr->Rs2);
		long long imm = instr->Imm;
		bool branchTaken =
			(opcode == CpuCore::IsaOpcodeValues::BEQ && rs1 == rs2) ||
			(opcode == CpuCore::IsaOpcodeValues::BNE && rs1 != rs2) ||
			(opcode == CpuCore::IsaOpcodeValues::BLT && static_cast<long long>(rs1) < static_cast<long long>(rs2)) ||
			(opcode == CpuCore::IsaOpcodeValues::BGE && static_cast<long long>(rs1) >= static_cast<long long>(rs2)) ||
			(opcode == CpuCore::IsaOpcodeValues::BLTU && rs1 < rs2) ||
			(opcode == CpuCore::IsaOpcodeValues::BGEU && rs1 >= rs2);
		if (branchTaken)
			return ExecutionResult::Redirect(pc + static_cast<unsigned long long>(imm), 0);
		if (opcode == CpuCore::IsaOpcodeValues::BEQ || opcode == CpuCore::IsaOpcodeValues::BNE ||
			opcode == CpuCore::IsaOpcodeValues::BLT || opcode == CpuCore::IsaOpcodeValues::BGE ||
			opcode == CpuCore::IsaOpcodeValues::BLTU || opcode == CpuCore::IsaOpcodeValues::BGEU)
			return ExecutionResult::Ok();

		switch (opcode)
		{
			// Unconditional jumps (JAL / JALR)
			case CpuCore::IsaOpcodeValues::JAL:
			{
					unsigned long long retAddr = pc + 4;
					return ExecutionResult::Redirect(pc + static_cast<unsigned long long>(imm), retAddr);
			}
			case CpuCore::IsaOpcodeValues::JALR:
			{
					unsigned long long retAddr = pc + 4;
					unsigned long long target = (rs1 + static_cast<unsigned long long>(imm)) & ~1ULL;
					return ExecutionResult::Redirect(target, retAddr);
			}

			default:
				throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in ControlFlow execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteControlFlow(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);
		ExecutionResult result = ResolveControlFlowResult(instr, state, vtId);
		if ((opcode == CpuCore::IsaOpcodeValues::JAL || opcode == CpuCore::IsaOpcodeValues::JALR) && instr->Rd != 0)
		{
			state->WriteRegister(vtId, instr->Rd, result.getValue());
		}

		return result;
	}

	void ExecutionDispatcherV4::CaptureControlFlowRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);
		ExecutionResult result = ResolveControlFlowResult(instr, state, vtId);

		if ((opcode == CpuCore::IsaOpcodeValues::JAL || opcode == CpuCore::IsaOpcodeValues::JALR) && instr->Rd != 0)
		{
			retireBatch.AppendRetireRecord(RetireRecord::RegisterWrite(vtId, instr->Rd, result.getValue()));
		}

		if (result.getPcRedirected())
		{
			retireBatch.AppendRetireRecord(RetireRecord::PcWrite(vtId, result.getNewPc()));
		}
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteAtomic(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		throw InvalidOperationException(std::format("Atomic opcode {0} reached ExecutionDispatcherV4 eager execute surface without an authoritative retire-window publication follow-through. ", ToNarrow(FormatOpcode(ResolveOpcode(instr)))) + "Direct callers must use CaptureRetireWindowPublications(...) or the mainline retire path.");
	}

	inline void ExecutionDispatcherV4::ThrowUnsupportedRetireWindowPublicationMemoryOpcode(std::shared_ptr<InstructionIR> instr)
	{
		throw InvalidOperationException(std::format("Memory opcode {0} does not expose an authoritative path through retire-window publication here; pipeline execution remains the supported path.", ToNarrow(FormatOpcode(ResolveOpcode(instr)))));
	}

	inline void ExecutionDispatcherV4::ThrowMissingMemoryUnitForRetireWindowPublication(std::shared_ptr<InstructionIR> instr)
	{
		throw InvalidOperationException(std::format("Memory opcode {0} requires a wired MemoryUnit for an authoritative path through retire-window publication; pipeline execution remains the supported path.", ToNarrow(FormatOpcode(ResolveOpcode(instr)))));
	}

	unsigned long long ExecutionDispatcherV4::ResolveScalarAluValue(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);
		unsigned long long rs1 = ReadExecutionRegister(state, vtId, instr->Rs1);
		unsigned long long rs2 = ReadExecutionRegister(state, vtId, instr->Rs2);
		long long imm = instr->Imm;

		unsigned long long temp_var36;
		switch (opcode)
		{
			// ── Reg-reg arithmetic ───────────────────────────────────────
			case CpuCore::IsaOpcodeValues::ADD:
				temp_var36 = rs1 + rs2;
				break;
			case CpuCore::IsaOpcodeValues::ADDW:
				temp_var36 = ScalarAluOps::AddWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SUBW:
				temp_var36 = ScalarAluOps::SubWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SLLW:
				temp_var36 = ScalarAluOps::ShiftLeftWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SRLW:
				temp_var36 = ScalarAluOps::ShiftRightLogicalWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SRAW:
				temp_var36 = ScalarAluOps::ShiftRightArithmeticWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SUB:
				temp_var36 = rs1 - rs2;
				break;
			case CpuCore::IsaOpcodeValues::MUL:
				temp_var36 = rs1 * rs2;
				break;
			case CpuCore::IsaOpcodeValues::MULW:
				temp_var36 = ScalarAluOps::MultiplyWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::DIVW:
				temp_var36 = ScalarAluOps::DivideWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::DIVUW:
				temp_var36 = ScalarAluOps::DivideUnsignedWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::REMW:
				temp_var36 = ScalarAluOps::RemainderWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::REMUW:
				temp_var36 = ScalarAluOps::RemainderUnsignedWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SEXT_W:
				temp_var36 = ScalarAluOps::SignExtendWordFromScalar(rs1);
				break;
			case CpuCore::IsaOpcodeValues::ZEXT_W:
				temp_var36 = ScalarAluOps::ZeroExtendWordFromScalar(rs1);
				break;
			case CpuCore::IsaOpcodeValues::DIV:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var36 = rs2 != 0 ? rs1 / rs2 : unchecked((ulong)-1L);
				temp_var36 = rs2 != 0 ? rs1 / rs2 : static_cast<unsigned long long>(-1LL);
				break;

			// ── ISA v4 M-extension: high-half multiply ────────────────────
			case CpuCore::IsaOpcodeValues::MULH:
				temp_var36 = YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSigned64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::MULHU:
				temp_var36 = YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighUnsigned64(rs1, rs2);
				break;
			// MULHSU: rs1 signed, rs2 unsigned — zero-extend rs2 into Int128
			case CpuCore::IsaOpcodeValues::MULHSU:
				temp_var36 = YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSignedUnsigned64(rs1, rs2);
				break;

			// ── ISA v4 M-extension: unsigned divide and remainder ──────────
			// ISA spec: DIVU returns 2^XLEN-1 on div-by-zero;
			//           REM  returns dividend on div-by-zero;
			//           REMU returns dividend on div-by-zero.
			case CpuCore::IsaOpcodeValues::DIVU:
				temp_var36 = rs2 != 0 ? rs1 / rs2 : std::numeric_limits<unsigned long long>::max();
				break;
			case CpuCore::IsaOpcodeValues::REM:
				temp_var36 = rs2 != 0 ? static_cast<unsigned long long>(static_cast<long long>(rs1) % static_cast<long long>(rs2)) : rs1;
				break;
			case CpuCore::IsaOpcodeValues::REMU:
				temp_var36 = rs2 != 0 ? rs1 % rs2 : rs1;
				break;

			// ── Logical ───────────────────────────────────────────────────
			case CpuCore::IsaOpcodeValues::AND:
				temp_var36 = rs1 & rs2;
				break;
			case CpuCore::IsaOpcodeValues::OR:
				temp_var36 = rs1 | rs2;
				break;
			case CpuCore::IsaOpcodeValues::XOR:
				temp_var36 = rs1 ^ rs2;
				break;
			case CpuCore::IsaOpcodeValues::CZERO_EQZ:
				temp_var36 = ScalarAluOps::ConditionalZeroIfEqualZero(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::CZERO_NEZ:
				temp_var36 = ScalarAluOps::ConditionalZeroIfNotEqualZero(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::CLZ:
				temp_var36 = ScalarAluOps::CountLeadingZeros64(rs1);
				break;
			case CpuCore::IsaOpcodeValues::CTZ:
				temp_var36 = ScalarAluOps::CountTrailingZeros64(rs1);
				break;
			case CpuCore::IsaOpcodeValues::CPOP:
				temp_var36 = ScalarAluOps::CountPopulation64(rs1);
				break;
			case CpuCore::IsaOpcodeValues::SEXT_B:
				temp_var36 = ScalarAluOps::SignExtendByteFromScalar(rs1);
				break;
			case CpuCore::IsaOpcodeValues::SEXT_H:
				temp_var36 = ScalarAluOps::SignExtendHalfFromScalar(rs1);
				break;
			case CpuCore::IsaOpcodeValues::ZEXT_H:
				temp_var36 = ScalarAluOps::ZeroExtendHalfFromScalar(rs1);
				break;
			case CpuCore::IsaOpcodeValues::ROL:
				temp_var36 = ScalarAluOps::RotateLeft64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::ROR:
				temp_var36 = ScalarAluOps::RotateRight64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::ROLI:
				temp_var36 = ScalarAluOps::RotateLeft64(rs1, static_cast<unsigned long long>(imm));
				break;
			case CpuCore::IsaOpcodeValues::RORI:
				temp_var36 = ScalarAluOps::RotateRight64(rs1, static_cast<unsigned long long>(imm));
				break;
			case CpuCore::IsaOpcodeValues::BSET:
				temp_var36 = ScalarAluOps::BitSet64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::BCLR:
				temp_var36 = ScalarAluOps::BitClear64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::BINV:
				temp_var36 = ScalarAluOps::BitInvert64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::BEXT:
				temp_var36 = ScalarAluOps::BitExtract64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::BSETI:
				temp_var36 = ScalarAluOps::BitSet64(rs1, static_cast<unsigned long long>(imm));
				break;
			case CpuCore::IsaOpcodeValues::BCLRI:
				temp_var36 = ScalarAluOps::BitClear64(rs1, static_cast<unsigned long long>(imm));
				break;
			case CpuCore::IsaOpcodeValues::BINVI:
				temp_var36 = ScalarAluOps::BitInvert64(rs1, static_cast<unsigned long long>(imm));
				break;
			case CpuCore::IsaOpcodeValues::BEXTI:
				temp_var36 = ScalarAluOps::BitExtract64(rs1, static_cast<unsigned long long>(imm));
				break;
			case CpuCore::IsaOpcodeValues::ANDN:
				temp_var36 = ScalarAluOps::AndNot64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::ORN:
				temp_var36 = ScalarAluOps::OrNot64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::XNOR:
				temp_var36 = ScalarAluOps::Xnor64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::MIN:
				temp_var36 = ScalarAluOps::MinSigned64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::MAX:
				temp_var36 = ScalarAluOps::MaxSigned64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::MINU:
				temp_var36 = ScalarAluOps::MinUnsigned64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::MAXU:
				temp_var36 = ScalarAluOps::MaxUnsigned64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::REV8:
				temp_var36 = ScalarAluOps::ReverseBytes64(rs1);
				break;
			case CpuCore::IsaOpcodeValues::BREV8:
				temp_var36 = ScalarAluOps::ReverseBitsInEachByte64(rs1);
				break;
			case CpuCore::IsaOpcodeValues::SH1ADD:
				temp_var36 = ScalarAluOps::ShiftLeftOneAdd(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SH2ADD:
				temp_var36 = ScalarAluOps::ShiftLeftTwoAdd(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SH3ADD:
				temp_var36 = ScalarAluOps::ShiftLeftThreeAdd(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::ADD_UW:
				temp_var36 = ScalarAluOps::AddUnsignedWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SH1ADD_UW:
				temp_var36 = ScalarAluOps::ShiftLeftOneAddUnsignedWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SH2ADD_UW:
				temp_var36 = ScalarAluOps::ShiftLeftTwoAddUnsignedWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SH3ADD_UW:
				temp_var36 = ScalarAluOps::ShiftLeftThreeAddUnsignedWord(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::SLLI_UW:
				temp_var36 = ScalarAluOps::ShiftLeftUnsignedWordImmediate(rs1, static_cast<unsigned long long>(imm));
				break;
			case CpuCore::IsaOpcodeValues::CLMUL:
				temp_var36 = ScalarAluOps::CarryLessMultiplyLow64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::CLMULH:
				temp_var36 = ScalarAluOps::CarryLessMultiplyHigh64(rs1, rs2);
				break;
			case CpuCore::IsaOpcodeValues::CLMULR:
				temp_var36 = ScalarAluOps::CarryLessMultiplyReverse64(rs1, rs2);
				break;

			// ── Shifts (register-register; immediate-based shifts use Imm) ─
			case CpuCore::IsaOpcodeValues::SLL:
				temp_var36 = rs1 << static_cast<int>(rs2 & 0x3F);
				break;
			case CpuCore::IsaOpcodeValues::SRL:
				temp_var36 = rs1 >> static_cast<int>(rs2 & 0x3F);
				break;
			case CpuCore::IsaOpcodeValues::SRA:
				temp_var36 = static_cast<unsigned long long>(static_cast<long long>(rs1) >> static_cast<int>(rs2 & 0x3F));
				break;

			// ── Immediate ALU ─────────────────────────────────────────────
			case CpuCore::IsaOpcodeValues::ADDI:
				temp_var36 = rs1 + static_cast<unsigned long long>(imm);
				break;
			case CpuCore::IsaOpcodeValues::ADDIW:
				temp_var36 = ScalarAluOps::AddWordImmediate(rs1, imm);
				break;
			case CpuCore::IsaOpcodeValues::SLLIW:
				temp_var36 = ScalarAluOps::ShiftLeftWordImmediate(rs1, imm);
				break;
			case CpuCore::IsaOpcodeValues::SRLIW:
				temp_var36 = ScalarAluOps::ShiftRightLogicalWordImmediate(rs1, imm);
				break;
			case CpuCore::IsaOpcodeValues::SRAIW:
				temp_var36 = ScalarAluOps::ShiftRightArithmeticWordImmediate(rs1, imm);
				break;
			case CpuCore::IsaOpcodeValues::ANDI:
				temp_var36 = rs1 & static_cast<unsigned long long>(imm);
				break;
			case CpuCore::IsaOpcodeValues::ORI:
				temp_var36 = rs1 | static_cast<unsigned long long>(imm);
				break;
			case CpuCore::IsaOpcodeValues::XORI:
				temp_var36 = rs1 ^ static_cast<unsigned long long>(imm);
				break;
			case CpuCore::IsaOpcodeValues::SLLI:
				temp_var36 = rs1 << static_cast<int>(static_cast<unsigned long long>(imm) & 0x3F);
				break;
			case CpuCore::IsaOpcodeValues::SRLI:
				temp_var36 = rs1 >> static_cast<int>(static_cast<unsigned long long>(imm) & 0x3F);
				break;
			case CpuCore::IsaOpcodeValues::SRAI:
				temp_var36 = static_cast<unsigned long long>(static_cast<long long>(rs1) >> static_cast<int>(static_cast<unsigned long long>(imm) & 0x3F));
				break;

			// ── Compare / set ─────────────────────────────────────────────
			case CpuCore::IsaOpcodeValues::SLT:
				temp_var36 = static_cast<long long>(rs1) < static_cast<long long>(rs2) ? 1ULL : 0ULL;
				break;
			case CpuCore::IsaOpcodeValues::SLTU:
				temp_var36 = rs1 < rs2 ? 1ULL : 0ULL;
				break;
			case CpuCore::IsaOpcodeValues::SLTI:
				temp_var36 = static_cast<long long>(rs1) < imm ? 1ULL : 0ULL;
				break;
			case CpuCore::IsaOpcodeValues::SLTIU:
				temp_var36 = rs1 < static_cast<unsigned long long>(imm) ? 1ULL : 0ULL;
				break;

			// ── Upper immediate ───────────────────────────────────────────
			case CpuCore::IsaOpcodeValues::LUI:
				temp_var36 = static_cast<unsigned long long>(imm << 12);
				break;
			case CpuCore::IsaOpcodeValues::AUIPC:
				temp_var36 = ReadExecutionPc(state, vtId) + static_cast<unsigned long long>(imm << 12);
				break;

			default:
				throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in ScalarAlu execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
				break;
		}
		return temp_var36;
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteScalarAlu(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		unsigned long long result = ResolveScalarAluValue(instr, state, vtId);
		if (instr->Rd != 0)
		{
			state->WriteRegister(vtId, instr->Rd, result);
		}
		return ExecutionResult::Ok(result);
	}

	void ExecutionDispatcherV4::CaptureScalarAluRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId)
	{
		if (instr->Rd == 0)
		{
			return;
		}

		retireBatch.AppendRetireRecord(RetireRecord::RegisterWrite(vtId, instr->Rd, ResolveScalarAluValue(instr, state, vtId)));
	}

	void ExecutionDispatcherV4::CaptureAtomicRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);
		AtomicRetireEffect effect = YAKSys_Hybrid_CPU::Core::Memory::AtomicMemoryUnitExtensions::ResolveRetireEffect(_atomicMemoryUnit, opcode, instr->Rd, ReadExecutionRegister(state, vtId, instr->Rs1), ReadExecutionRegister(state, vtId, instr->Rs2), state->GetCoreID(), vtId, instr->AcquireOrdering, instr->ReleaseOrdering);
		retireBatch.CaptureRetireWindowAtomicEffect(effect);
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteSystem(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned long long bundleSerial, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);
		ValidateCanonicalFenceInstruction(instr, opcode);

		switch (opcode)
		{
			case CpuCore::IsaOpcodeValues::FENCE:
			{
				std::shared_ptr<FenceEvent> temp_var = std::make_shared<FenceEvent>();
				temp_var->VtId = vtId;
				temp_var->BundleSerial = bundleSerial;
				temp_var->IsInstructionFence = false;
				return EnqueuePipelineEvent(temp_var);
			}

			case CpuCore::IsaOpcodeValues::FENCE_I:
			{
				std::shared_ptr<FenceEvent> temp_var2 = std::make_shared<FenceEvent>();
				temp_var2->VtId = vtId;
				temp_var2->BundleSerial = bundleSerial;
				temp_var2->IsInstructionFence = true;
				return EnqueuePipelineEvent(temp_var2);
			}

			case CpuCore::IsaOpcodeValues::ECALL:
			{
					// a7 (x17) holds the system-call number per RISC-V ABI.
					long long ecallCode = static_cast<long long>(ReadExecutionRegister(state, vtId, 17));
				std::shared_ptr<EcallEvent> temp_var3 = std::make_shared<EcallEvent>();
				temp_var3->VtId = vtId;
				temp_var3->BundleSerial = bundleSerial;
				temp_var3->EcallCode = ecallCode;
				return EnqueuePipelineEvent(temp_var3);
			}

			case CpuCore::IsaOpcodeValues::EBREAK:
			{
				std::shared_ptr<EbreakEvent> temp_var4 = std::make_shared<EbreakEvent>();
				temp_var4->VtId = vtId;
				temp_var4->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var4);
			}

			case CpuCore::IsaOpcodeValues::MRET:
			{
				std::shared_ptr<MretEvent> temp_var5 = std::make_shared<MretEvent>();
				temp_var5->VtId = vtId;
				temp_var5->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var5);
			}

			case CpuCore::IsaOpcodeValues::SRET:
			{
				std::shared_ptr<SretEvent> temp_var6 = std::make_shared<SretEvent>();
				temp_var6->VtId = vtId;
				temp_var6->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var6);
			}

			case CpuCore::IsaOpcodeValues::WFI:
			{
				std::shared_ptr<WfiEvent> temp_var7 = std::make_shared<WfiEvent>();
				temp_var7->VtId = vtId;
				temp_var7->BundleSerial = bundleSerial;
				return EnqueuePipelineEvent(temp_var7);
			}

			case CpuCore::IsaOpcodeValues::STREAM_SETUP:
			case CpuCore::IsaOpcodeValues::STREAM_START:
			case CpuCore::IsaOpcodeValues::STREAM_WAIT:
				return ThrowUnsupportedEagerStreamControlOpcode(instr);

			default:
				throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in System execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}
	}

	void ExecutionDispatcherV4::CaptureSystemRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned long long bundleSerial, unsigned char vtId)
	{
		unsigned short opcode = ResolveOpcode(instr);
		ValidateCanonicalFenceInstruction(instr, opcode);

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			Core.Pipeline.PipelineEvent systemEvent = opcode switch
	//			{
	//				IsaOpcodeValues.FENCE => new FenceEvent
	//				{
	//					VtId = vtId, BundleSerial = bundleSerial, IsInstructionFence = false
	//				}, IsaOpcodeValues.FENCE_I => new FenceEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial,
	//					IsInstructionFence = true
	//				},
	//				IsaOpcodeValues.ECALL => new EcallEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial,
	//					EcallCode = (long)ReadExecutionRegister(state, vtId, 17)
	//				},
	//				IsaOpcodeValues.EBREAK => new EbreakEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				IsaOpcodeValues.MRET => new MretEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				IsaOpcodeValues.SRET => new SretEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				IsaOpcodeValues.WFI => new WfiEvent
	//				{
	//					VtId = vtId,
	//					BundleSerial = bundleSerial
	//				},
	//				IsaOpcodeValues.STREAM_SETUP or
	//				IsaOpcodeValues.STREAM_START or
	//				IsaOpcodeValues.STREAM_WAIT
	//					=> ThrowUnsupportedRetireWindowPublicationSystemOpcode(instr),
	//				_ => throw new InvalidOpcodeException(
	//					$"Invalid opcode {FormatOpcode(opcode)} in System execution unit",
	//					FormatOpcode(opcode),
	//					-1,
	//					false)
	//			};

		std::shared_ptr<Core::Pipeline::PipelineEvent> systemEvent;
		switch (opcode)
		{
		case CpuCore::IsaOpcodeValues::FENCE:
		case CpuCore::IsaOpcodeValues::FENCE_I:
		{
			auto event = std::make_shared<Core::Pipeline::FenceEvent>();
			event->IsInstructionFence = opcode == CpuCore::IsaOpcodeValues::FENCE_I;
			systemEvent = event;
			break;
		}
		case CpuCore::IsaOpcodeValues::ECALL:
		{
			auto event = std::make_shared<Core::Pipeline::EcallEvent>();
			event->EcallCode = static_cast<long long>(ReadExecutionRegister(state, vtId, 17));
			systemEvent = event;
			break;
		}
		case CpuCore::IsaOpcodeValues::EBREAK: systemEvent = std::make_shared<Core::Pipeline::EbreakEvent>(); break;
		case CpuCore::IsaOpcodeValues::MRET: systemEvent = std::make_shared<Core::Pipeline::MretEvent>(); break;
		case CpuCore::IsaOpcodeValues::SRET: systemEvent = std::make_shared<Core::Pipeline::SretEvent>(); break;
		case CpuCore::IsaOpcodeValues::WFI: systemEvent = std::make_shared<Core::Pipeline::WfiEvent>(); break;
		case CpuCore::IsaOpcodeValues::STREAM_SETUP:
		case CpuCore::IsaOpcodeValues::STREAM_START:
		case CpuCore::IsaOpcodeValues::STREAM_WAIT:
			ThrowUnsupportedRetireWindowPublicationSystemOpcode(instr);
		default:
			throw InvalidOpcodeException(ToWide(std::format("Invalid opcode {0} in System execution unit", ToNarrow(FormatOpcode(opcode)))), FormatOpcode(opcode), -1, false);
		}
		systemEvent->VtId = vtId;
		systemEvent->BundleSerial = bundleSerial;
		retireBatch.CaptureRetireWindowPipelineEvent(systemEvent, ResolveRetireWindowPublicationSystemOrderGuarantee(opcode), ReadExecutionPc(state, vtId), vtId, RequiresRetireWindowPublicationSerializingBoundaryFollowThrough(instr));
	}

	inline Core::SystemEventOrderGuarantee ExecutionDispatcherV4::ResolveRetireWindowPublicationSystemOrderGuarantee(unsigned short opcode)
	{
		if (opcode == CpuCore::IsaOpcodeValues::FENCE || opcode == CpuCore::IsaOpcodeValues::WFI)
			return Core::SystemEventOrderGuarantee::DrainMemory;
		if (opcode == CpuCore::IsaOpcodeValues::FENCE_I)
			return Core::SystemEventOrderGuarantee::FlushPipeline;
		if (opcode == CpuCore::IsaOpcodeValues::ECALL || opcode == CpuCore::IsaOpcodeValues::EBREAK ||
			opcode == CpuCore::IsaOpcodeValues::MRET || opcode == CpuCore::IsaOpcodeValues::SRET)
			return Core::SystemEventOrderGuarantee::FullSerialTrapBoundary;
		return Core::SystemEventOrderGuarantee::None;
	}

	void ExecutionDispatcherV4::ValidateCanonicalFenceInstruction(std::shared_ptr<InstructionIR> instr, unsigned short opcode)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((opcode != CpuCore::IsaOpcodeValues::FENCE && opcode != CpuCore::IsaOpcodeValues::FENCE_I))
		{
			return;
		}

		if (instr->Imm == 0 && !instr->HasAbsoluteAddressing && !instr->AcquireOrdering && !instr->ReleaseOrdering)
		{
			return;
		}

		throw InvalidOperationException(std::format("Opcode {0} reached ExecutionDispatcherV4 with unsupported Phase 10 fence IR payload. ", ToNarrow(FormatOpcode(opcode))) + "Current runtime-owned FENCE/FENCE.I semantics accept only the canonical zero-payload form; " + "IR immediates, absolute addressing, and aq/rl bits are not ordering authority.");
	}

	void ExecutionDispatcherV4::CaptureVmxRetireWindowPublications(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, CpuCore::RetireWindowBatch &retireBatch, unsigned char vtId)
	{
		VmxRetireEffect effect = CreateRemovedFrontendFaultEffect(instr);

		if (effect.isValid())
		{
			retireBatch.CaptureRetireWindowVmxEffect(effect, vtId);
		}
	}

	ExecutionResult ExecutionDispatcherV4::ExecuteVmx(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
		return ExecutionResult::VmxFault();
	}

	YAKSys_Hybrid_CPU::Core::VmxRetireEffect ExecutionDispatcherV4::CreateRemovedFrontendFaultEffect(std::shared_ptr<InstructionIR> instr)
	{
		VmxOperationKind operation;
		if (!InstructionRegistry::TryResolvePublishedVmxOperationKind(instr, operation))
		{
			throw InvalidOperationException(std::format("Unknown frozen VMX opcode {0}.", ToNarrow(OpcodeRegistry::GetMnemonicOrHex(instr->CanonicalOpcode->Value))));
		}

		return VmxRetireEffect::Fault(operation, VmExitReason::SecurityPolicyViolation);
	}

	inline ExecutionResult ExecutionDispatcherV4::EnqueuePipelineEvent(std::shared_ptr<PipelineEvent> evt)
	{
		_pipelineEventQueue->Enqueue(evt);
		return ExecutionResult::Ok();
	}
}





