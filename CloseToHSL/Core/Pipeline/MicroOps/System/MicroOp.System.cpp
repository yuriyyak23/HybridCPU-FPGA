#include "MicroOp.System.h"
#include "../../Safety/ResourceBitset.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../../../../NonRTL/Arch/InstructionClassifier.h"
#include "../../PipelineEvent.h"

using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		const wchar_t *GetSystemEventKindName(SystemEventKind eventKind)
		{
		switch (eventKind)
		{
		case SystemEventKind::Fence: return L"Fence";
		case SystemEventKind::FenceI: return L"FenceI";
		case SystemEventKind::Ecall: return L"Ecall";
		case SystemEventKind::Ebreak: return L"Ebreak";
		case SystemEventKind::Mret: return L"Mret";
		case SystemEventKind::Sret: return L"Sret";
		case SystemEventKind::Wfi: return L"Wfi";
		case SystemEventKind::Wfe: return L"Wfe";
		case SystemEventKind::Sev: return L"Sev";
		case SystemEventKind::Yield: return L"Yield";
		case SystemEventKind::PodBarrier: return L"PodBarrier";
		case SystemEventKind::VtBarrier: return L"VtBarrier";
		default: return L"Unknown";
		}
		}

		const wchar_t *GetSystemEventOrderName(SystemEventOrderGuarantee orderGuarantee)
		{
		switch (orderGuarantee)
		{
		case SystemEventOrderGuarantee::None: return L"None";
		case SystemEventOrderGuarantee::DrainStores: return L"DrainStores";
		case SystemEventOrderGuarantee::DrainMemory: return L"DrainMemory";
		case SystemEventOrderGuarantee::FlushPipeline: return L"FlushPipeline";
		case SystemEventOrderGuarantee::FullSerialTrapBoundary: return L"FullSerialTrapBoundary";
		default: return L"Unknown";
		}
		}
	}

	SystemEventKind SysEventMicroOp::getEventKind() const
	{
		return EventKind;
	}

	void SysEventMicroOp::setEventKind(SystemEventKind value)
	{
		EventKind = value;
	}

	SystemEventOrderGuarantee SysEventMicroOp::getOrderGuarantee() const
	{
		return OrderGuarantee;
	}

	void SysEventMicroOp::setOrderGuarantee(SystemEventOrderGuarantee value)
	{
		OrderGuarantee = value;
	}

	SysEventMicroOp::SysEventMicroOp()
	{
		setStealable(false);
		setHasSideEffects(true);
		setClass(MicroOpClass::Other);
		setResourceMask(ResourceBitset::Zero);

		setInstructionClass(Arch::InstructionClass::System);
		setSerializationClass(Arch::SerializationClass::FullSerial);

		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);
	}

	CanonicalDecodePublicationMode SysEventMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	bool SysEventMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		return true;
	}

	void SysEventMicroOp::InitializeMetadata()
	{
		ApplyCanonicalClassificationAndPlacement();

		const bool isEcall = getEventKind() == SystemEventKind::Ecall;
		setReadRegisters(isEcall
			? std::vector<int>{10, 11, 12, 13, 14, 15, 16, EcallCodeRegister, 18, 19}
			: std::vector<int>{});
		setWriteRegisters(isEcall ? std::vector<int>{10, 11, 12} : std::vector<int>{});
		setWritesRegister(getWriteRegisters().size() != 0);
		setReadMemoryRanges(std::vector<MemoryRange>{});
		setWriteMemoryRanges(std::vector<MemoryRange>{});

		setResourceMask(ResourceBitset::Zero);
		if (getEventKind() == SystemEventKind::Ecall)
		{
			const int ownerVtId = NormalizeExecutionVtId(getOwnerThreadId());
			for (const auto registerId : getReadRegisters())
			{
				setResourceMask(getResourceMask() | ResourceMaskBuilder::ForArchitecturalRegisterRead(
					std::make_shared<ArchRegId>(ArchRegId::Create(registerId)), ownerVtId));
			}
			for (const auto registerId : getWriteRegisters())
			{
				setResourceMask(getResourceMask() | ResourceMaskBuilder::ForArchitecturalRegisterWrite(
					std::make_shared<ArchRegId>(ArchRegId::Create(registerId)), ownerVtId));
			}
		}

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	void SysEventMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	std::optional<std::shared_ptr<Pipeline::PipelineEvent>> SysEventMicroOp::CreatePipelineEvent(
		std::shared_ptr<Processor::CPU_Core> &core,
		unsigned long long bundleSerial)
	{
		const auto vtId = static_cast<unsigned char>(getVirtualThreadId() & 0xFF);
		std::shared_ptr<Pipeline::PipelineEvent> event;
		switch (getEventKind())
		{
		case SystemEventKind::Fence:
		case SystemEventKind::FenceI:
		{
			auto fenceEvent = std::make_shared<Pipeline::FenceEvent>();
			fenceEvent->IsInstructionFence = getEventKind() == SystemEventKind::FenceI;
			event = std::move(fenceEvent);
			break;
		}
		case SystemEventKind::Ecall:
		{
			auto ecallEvent = std::make_shared<Pipeline::EcallEvent>();
			ecallEvent->EcallCode = ReadEcallCode(core, vtId);
			event = std::move(ecallEvent);
			break;
		}
		case SystemEventKind::Ebreak: event = std::make_shared<Pipeline::EbreakEvent>(); break;
		case SystemEventKind::Mret: event = std::make_shared<Pipeline::MretEvent>(); break;
		case SystemEventKind::Sret: event = std::make_shared<Pipeline::SretEvent>(); break;
		case SystemEventKind::Wfi: event = std::make_shared<Pipeline::WfiEvent>(); break;
		case SystemEventKind::Wfe: event = std::make_shared<Pipeline::WfeEvent>(); break;
		case SystemEventKind::Sev: event = std::make_shared<Pipeline::SevEvent>(); break;
		case SystemEventKind::Yield: event = std::make_shared<Pipeline::YieldEvent>(); break;
		case SystemEventKind::PodBarrier: event = std::make_shared<Pipeline::PodBarrierEvent>(); break;
		case SystemEventKind::VtBarrier: event = std::make_shared<Pipeline::VtBarrierEvent>(); break;
		default:
			return std::nullopt;
		}

		event->VtId = vtId;
		event->BundleSerial = bundleSerial;
		return event;
	}
	std::wstring SysEventMicroOp::GetDescription()
	{
		return std::format(L"SysEvent({}, Order={}, OpCode={})",
			GetSystemEventKindName(getEventKind()), GetSystemEventOrderName(getOrderGuarantee()), getOpCode());
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForFence()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::FENCE, SystemEventKind::Fence, SystemEventOrderGuarantee::DrainMemory);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForFenceI()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::FENCE_I, SystemEventKind::FenceI, SystemEventOrderGuarantee::FlushPipeline);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForEcall()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::ECALL, SystemEventKind::Ecall, SystemEventOrderGuarantee::FullSerialTrapBoundary);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForEbreak()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::EBREAK, SystemEventKind::Ebreak, SystemEventOrderGuarantee::FullSerialTrapBoundary);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForMret()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::MRET, SystemEventKind::Mret, SystemEventOrderGuarantee::FullSerialTrapBoundary);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForSret()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::SRET, SystemEventKind::Sret, SystemEventOrderGuarantee::FullSerialTrapBoundary);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForWfi()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::WFI, SystemEventKind::Wfi, SystemEventOrderGuarantee::DrainMemory);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForWfe()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::WFE, SystemEventKind::Wfe, SystemEventOrderGuarantee::DrainMemory);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForSev()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::SEV, SystemEventKind::Sev, SystemEventOrderGuarantee::DrainMemory);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForYield()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::YIELD, SystemEventKind::Yield, SystemEventOrderGuarantee::None);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForPodBarrier()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::POD_BARRIER, SystemEventKind::PodBarrier, SystemEventOrderGuarantee::None);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::ForVtBarrier()
	{
		return Create(Processor::CPU_Core::IsaOpcodeValues::VT_BARRIER, SystemEventKind::VtBarrier, SystemEventOrderGuarantee::None);
	}

	std::shared_ptr<SysEventMicroOp> SysEventMicroOp::Create(unsigned short opcode, SystemEventKind eventKind, SystemEventOrderGuarantee orderGuarantee)
	{
		auto microOp = std::make_shared<SysEventMicroOp>();
		microOp->setOpCode(static_cast<unsigned int>(opcode));
		microOp->setEventKind(eventKind);
		microOp->setOrderGuarantee(orderGuarantee);
		microOp->InitializeMetadata();
		return microOp;
	}

	void SysEventMicroOp::ApplyCanonicalClassificationAndPlacement()
	{
		Arch::InstructionClass publishedInstructionClass;
		Arch::SerializationClass publishedSerializationClass;
		if (Arch::OpcodeRegistry::TryGetPublishedSemantics(getOpCode(), publishedInstructionClass, publishedSerializationClass))
		{
			setInstructionClass(publishedInstructionClass);
			setSerializationClass(publishedSerializationClass);
		}
		else
		{
			// Compatibility fallback for helper-constructed carriers that do not
			// yet carry an authoritative published opcode identity.
			switch (EventKind)
			{
			case SystemEventKind::Yield:
			case SystemEventKind::Wfe:
			case SystemEventKind::Sev:
			case SystemEventKind::PodBarrier:
			case SystemEventKind::VtBarrier:
				setInstructionClass(Arch::InstructionClass::SmtVt);
				break;
			default:
				setInstructionClass(Arch::InstructionClass::System);
				break;
			}
			switch (EventKind)
			{
			case SystemEventKind::Fence:
				setSerializationClass(Arch::SerializationClass::MemoryOrdered);
				break;
			case SystemEventKind::Yield:
				setSerializationClass(Arch::SerializationClass::Free);
				break;
			default:
				setSerializationClass(Arch::SerializationClass::FullSerial);
				break;
			}
		}

		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);
	}

	long long SysEventMicroOp::ReadEcallCode(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId)
	{
		// RISC-V ABI: a7 = x17 (register index 17 in the local core window).
		int vtId = NormalizeExecutionVtId(ownerThreadId);
		unsigned long long value;
		if (!TryReadUnifiedArchValue(core, vtId, EcallCodeRegister, value))
		{
			throw InvalidOperationException(L"ECALL carrier could not resolve authoritative a7/x17 on the current mainline system-event contour; refusing hidden zero-code fallback.");
		}

		return static_cast<long long>(value);
	}

	StreamControlMicroOp::StreamControlMicroOp()
	{
		setStealable(false);
		// Stream-control carriers do not themselves perform a typed data-memory
		// transaction. STREAM_WAIT still follows the lane-7 serializing-boundary
		// contour; unsupported setup/start surfaces are rejected before execute.
		setMemoryOp(false);
		setHasSideEffects(true);
		setClass(MicroOpClass::Other);
		setResourceMask(ResourceBitset::Zero);

	setInstructionClass(Arch::InstructionClass::System);
	setSerializationClass(Arch::SerializationClass::FullSerial);

	  SetPlacement(SlotClass::Unclassified, SlotPinningKind::HardPinned);
	}

	CanonicalDecodePublicationMode StreamControlMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void StreamControlMicroOp::InitializeMetadata()
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ushort opcode = unchecked((ushort)OpCode);
		unsigned short opcode = static_cast<unsigned short>(getOpCode());

		setReadRegisters(std::vector<int>{});
		setWriteRegisters(std::vector<int>{});
		setReadMemoryRanges(std::vector<MemoryRange>{});
		setWriteMemoryRanges(std::vector<MemoryRange>{});
		setResourceMask(ResourceBitset::Zero);

		setInstructionClass(Arch::InstructionClassifier::GetClass(opcode));
		setSerializationClass(Arch::InstructionClassifier::GetSerializationClass(opcode));
		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	bool StreamControlMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		return true;
	}

	std::wstring StreamControlMicroOp::GetDescription()
	{
		return std::format(L"StreamCtrl (OpCode={})", getOpCode());
	}
}
