#include "InstructionClassifier.h"
#include "Generated/GeneratedIsaShadowCatalog.g.h"

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Arch
{

	InstructionClass InstructionClassifier::GetClass(std::shared_ptr<Processor::CPU_Core::IsaOpcode> opcode)
	{
		return GetClass(opcode->Value);
	}

	InstructionClass InstructionClassifier::GetClass(unsigned short opcode)
	{
		std::shared_ptr<GeneratedIsaDescriptor> descriptor;
		if (GeneratedIsaCatalog::TryGetDescriptor(opcode, descriptor))
		{
			return descriptor->StaticClass;
		}

		return GetClass(static_cast<Processor::CPU_Core::InstructionsEnum>(opcode));
	}

	InstructionClass InstructionClassifier::GetClass(Processor::CPU_Core::InstructionsEnum opcode)
	{
		std::shared_ptr<GeneratedIsaDescriptor> descriptor;
		if (GeneratedIsaCatalog::TryGetDescriptor(static_cast<unsigned int>(opcode), descriptor))
		{
			return descriptor->StaticClass;
		}

	if (opcode == Processor::CPU_Core::InstructionsEnum::ADD) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ADDW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SUBW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLLW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRLW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRAW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SUB) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MUL) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MULW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DIVW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DIVUW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::REMW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::REMUW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SEXT_W) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ZEXT_W) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DIV) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLL) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRL) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRA) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::XOR) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::OR) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AND) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ADDI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ADDIW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ANDI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ORI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::XORI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLTI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLTIU) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLLI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRLI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRAI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLLIW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRLIW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRAIW) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLT) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SLTU) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LUI) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AUIPC) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MULH) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MULHU) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MULHSU) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DIVU) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::REM) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::REMU) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LB) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LBU) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LH) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LHU) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LW) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LWU) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LD) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SB) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SH) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SW) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SD) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DmaStreamCompute) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DSC_STATUS) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DSC_QUERY_CAPS) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::JAL) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::JALR) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::BEQ) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::BNE) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::BLT) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::BGE) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::BLTU) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::BGEU) return InstructionClass::ControlFlow;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LR_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SC_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LR_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SC_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOADD_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOSWAP_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOOR_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOAND_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOXOR_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMIN_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAX_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMINU_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAXU_W) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOADD_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOSWAP_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOOR_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOAND_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOXOR_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMIN_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAX_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMINU_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAXU_D) return InstructionClass::Atomic;
		if (opcode == Processor::CPU_Core::InstructionsEnum::FENCE) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::FENCE_I) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ECALL) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::EBREAK) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MRET) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRET) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::WFI) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVL) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVLI) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETIVLI) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_QUERY_CAPS) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_SUBMIT) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_POLL) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_WAIT) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_CANCEL) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_FENCE) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_STATUS) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRW) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRS) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRC) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRWI) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRSI) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRCI) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSR_CLEAR) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVEXCPMASK) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVEXCPPRI) return InstructionClass::Csr;
		if (opcode == Processor::CPU_Core::InstructionsEnum::STREAM_WAIT) return InstructionClass::SmtVt;
		if (opcode == Processor::CPU_Core::InstructionsEnum::STREAM_SETUP) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::STREAM_START) return InstructionClass::System;
		if (opcode == Processor::CPU_Core::InstructionsEnum::YIELD) return InstructionClass::SmtVt;
		if (opcode == Processor::CPU_Core::InstructionsEnum::WFE) return InstructionClass::SmtVt;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SEV) return InstructionClass::SmtVt;
		if (opcode == Processor::CPU_Core::InstructionsEnum::POD_BARRIER) return InstructionClass::SmtVt;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VT_BARRIER) return InstructionClass::SmtVt;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMXON) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMXOFF) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMLAUNCH) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMRESUME) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMREAD) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMWRITE) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMCLEAR) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMPTRLD) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMPTRST) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMCALL) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::INVEPT) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::INVVPID) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMFUNC) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMSAVEX) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMRESTX) return InstructionClass::Vmx;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VGATHER) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSCATTER) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSLIDE1UP) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VPERM2) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VTRANSPOSE) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VDOT_FP8) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VDOT_WIDE) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MTILE_LOAD) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MTILE_STORE) return InstructionClass::Memory;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MTILE_MACC) return InstructionClass::ScalarAlu;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MTRANSPOSE) return InstructionClass::ScalarAlu;
		return InstructionClass::ScalarAlu;
	}

	SerializationClass InstructionClassifier::GetSerializationClass(std::shared_ptr<Processor::CPU_Core::IsaOpcode> opcode)
	{
		return GetSerializationClass(opcode->Value);
	}

	SerializationClass InstructionClassifier::GetSerializationClass(unsigned short opcode)
	{
		std::shared_ptr<GeneratedIsaDescriptor> descriptor;
		if (GeneratedIsaCatalog::TryGetDescriptor(opcode, descriptor))
		{
			return descriptor->Serialization;
		}

		return GetSerializationClass(static_cast<Processor::CPU_Core::InstructionsEnum>(opcode));
	}

	SerializationClass InstructionClassifier::GetSerializationClass(Processor::CPU_Core::InstructionsEnum opcode)
	{
		std::shared_ptr<GeneratedIsaDescriptor> descriptor;
		if (GeneratedIsaCatalog::TryGetDescriptor(static_cast<unsigned int>(opcode), descriptor))
		{
			return descriptor->Serialization;
		}

	if (opcode == Processor::CPU_Core::InstructionsEnum::ECALL) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::EBREAK) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MRET) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SRET) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::WFI) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVL) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVLI) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETIVLI) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::FENCE_I) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::STREAM_WAIT) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::STREAM_SETUP) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::STREAM_START) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVEXCPMASK) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSETVEXCPPRI) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_WAIT) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_CANCEL) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_FENCE) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::FENCE) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SB) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SH) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SW) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SD) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DmaStreamCompute) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DSC_STATUS) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::DSC_QUERY_CAPS) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_SUBMIT) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VSCATTER) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::MTILE_STORE) return SerializationClass::MemoryOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LR_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SC_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::LR_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SC_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOADD_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOSWAP_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOOR_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOAND_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOXOR_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMIN_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAX_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMINU_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAXU_W) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOADD_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOSWAP_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOOR_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOAND_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOXOR_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMIN_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAX_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMINU_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::AMOMAXU_D) return SerializationClass::AtomicSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::YIELD) return SerializationClass::Free;
		if (opcode == Processor::CPU_Core::InstructionsEnum::WFE) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::SEV) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::POD_BARRIER) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VT_BARRIER) return SerializationClass::FullSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMXON) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMXOFF) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMLAUNCH) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMRESUME) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMREAD) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMWRITE) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMCLEAR) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMPTRLD) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMPTRST) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMCALL) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::INVEPT) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::INVVPID) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMFUNC) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMSAVEX) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::VMRESTX) return SerializationClass::VmxSerial;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRW) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRS) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRC) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRWI) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRSI) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSRRCI) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::CSR_CLEAR) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_QUERY_CAPS) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_POLL) return SerializationClass::CsrOrdered;
		if (opcode == Processor::CPU_Core::InstructionsEnum::ACCEL_STATUS) return SerializationClass::CsrOrdered;
		return SerializationClass::Free;
	}
}
