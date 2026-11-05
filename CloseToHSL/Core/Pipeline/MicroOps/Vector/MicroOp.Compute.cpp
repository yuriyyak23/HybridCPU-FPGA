#include "MicroOp.Compute.h"
#include "../../../Architecture/Common/StreamEngineId.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../Execution/StreamEngine/Classification/YAKSys_Hybrid_CPU.Execution.StreamEngine.h"
#include "../../Safety/ResourceBitset.h"
#include "../../../Execution/Scalar/ALU/ScalarAluOps.h"
#include <algorithm>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned short ScalarALUMicroOp::getSrc1RegID() const
	{
		return Src1RegID;
	}

	void ScalarALUMicroOp::setSrc1RegID(unsigned short value)
	{
		Src1RegID = value;
	}

	unsigned short ScalarALUMicroOp::getSrc2RegID() const
	{
		return Src2RegID;
	}

	void ScalarALUMicroOp::setSrc2RegID(unsigned short value)
	{
		Src2RegID = value;
	}

	unsigned long long ScalarALUMicroOp::getImmediate() const
	{
		return Immediate;
	}

	void ScalarALUMicroOp::setImmediate(unsigned long long value)
	{
		Immediate = value;
	}

	bool ScalarALUMicroOp::getUsesImmediate() const
	{
		return UsesImmediate;
	}

	void ScalarALUMicroOp::setUsesImmediate(bool value)
	{
		UsesImmediate = value;
	}

	ScalarALUMicroOp::ScalarALUMicroOp()
	{
		setClass(MicroOpClass::Alu);

		// ISA v4 Phase 02: instruction classification
		setInstructionClass(Arch::InstructionClass::ScalarAlu);
		setSerializationClass(Arch::SerializationClass::Free);

		// Phase 01: Typed-slot taxonomy
		SetClassFlexiblePlacement(SlotClass::AluClass);
	}

	CanonicalDecodePublicationMode ScalarALUMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void ScalarALUMicroOp::InitializeMetadata()
	{
		constexpr unsigned short noReg = VLIW_Instruction::NoReg;

		// Read from source registers — skip NoReg sentinels
		std::vector<int> readRegs;
		if (getSrc1RegID() != noReg)
		{
			readRegs.push_back(getSrc1RegID());
		}
		if (!getUsesImmediate() && getSrc2RegID() != noReg)
		{
			readRegs.push_back(getSrc2RegID());
		}
		setReadRegisters(readRegs);

		// Write to destination register — skip NoReg sentinels
		if (getWritesRegister() && getDestRegID() != noReg)
		{
			setWriteRegisters(std::vector<int> {static_cast<int>(getDestRegID())});
		}

		// Phase 8: Initialize ResourceMask for GRLB
		setResourceMask(ResourceBitset::Zero);
		// Add register read resources (guard against NoReg)
		if (getSrc1RegID() != noReg)
		{
			ArchRegId source1Register;
			const ResourceBitset source1Resource = ArchRegId::TryCreate(getSrc1RegID(), source1Register)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(source1Register))
				: ResourceMaskBuilder::ForRegisterRead(getSrc1RegID());
			setResourceMask(getResourceMask() | source1Resource);
		}
		if (!getUsesImmediate() && getSrc2RegID() != noReg)
		{
			ArchRegId source2Register;
			const ResourceBitset source2Resource = ArchRegId::TryCreate(getSrc2RegID(), source2Register)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(source2Register))
				: ResourceMaskBuilder::ForRegisterRead(getSrc2RegID());
			setResourceMask(getResourceMask() | source2Resource);
		}
		// Add register write resources (guard against NoReg)
		if (getWritesRegister() && getDestRegID() != noReg)
		{
			ArchRegId destinationRegister;
			const ResourceBitset destinationResource = ArchRegId::TryCreate(getDestRegID(), destinationRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(destinationRegister))
				: ResourceMaskBuilder::ForRegisterWrite(getDestRegID());
			setResourceMask(getResourceMask() | destinationResource);
		}

		// Phase: Safety Tags & Certificates - Mandatory SafetyMask initialization
		// SafetyMask must be computed for all operations to enable fast verification
		RefreshAdmissionMetadata(shared_from_this());
	}

	void ScalarALUMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	bool ScalarALUMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Blueprint §6 / §8: Read operands via Physical Register File through RenameMap.
		// "вместо чтения Processor.CPU_Core.IntRegisters, получать Phys = RenameMap[OwnerThreadId][SrcReg]
		//  и читать PhysicalRegisters[Phys]."
		VtId checkedOwner;
		int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());
		unsigned long long op1 = ReadUnifiedScalarSourceOperand(core, vtId, getSrc1RegID());
		unsigned long long op2 = getUsesImmediate() ? getImmediate() : ReadUnifiedScalarSourceOperand(core, vtId, getSrc2RegID());
		unsigned long long executionPc = core->ResolveCurrentScalarMicroOpExecutionPc();

		// Execute based on opcode
		_result = ExecuteScalarOp(getOpCode(), op1, op2, executionPc);
		return true;
	}

	bool ScalarALUMicroOp::ExecuteWithResolvedPipelineOperands(unsigned long long source1, unsigned long long source2, unsigned long long executionPc)
	{
		_result = ExecuteScalarOp(getOpCode(), source1, source2, executionPc);
		return true;
	}

	void ScalarALUMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!getWritesRegister())
		{
			return;
		}

		VtId checkedOwner;
		int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());
		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), _result));
	}

	bool ScalarALUMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _result;
		return getWritesRegister();
	}

	void ScalarALUMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_result = value;
	}

	unsigned long long ScalarALUMicroOp::ExecuteScalarOp(unsigned int opCode, unsigned long long op1, unsigned long long op2, unsigned long long executionPc)
	{
		return ScalarAluOps::Compute(opCode, op1, op2, op2, executionPc);
	}

	std::wstring ScalarALUMicroOp::GetDescription()
	{
		return std::format(L"ScalarALU: OpCode={}, Dest=R{}, Src1=R{}, ", getOpCode(), getDestRegID(), getSrc1RegID()) +
			(getUsesImmediate() ? std::format(L"Imm=0x{:X}", getImmediate()) : std::format(L"Src2=R{}", getSrc2RegID()));
	}

	VLIW_Instruction VectorALUMicroOp::getInstruction() const
	{
		return Instruction;
	}

	void VectorALUMicroOp::setInstruction(VLIW_Instruction value)
	{
		Instruction = value;
	}

	VectorALUMicroOp::VectorALUMicroOp()
	{
		// Memory ranges will be populated by StreamEngine during execution
		setMemoryOp(true);
		setClass(MicroOpClass::Vector);

		// ISA v4 Phase 02: vector ops use ScalarAlu class (extension slot, not mandatory core)
		setInstructionClass(Arch::InstructionClass::ScalarAlu);
		setSerializationClass(Arch::SerializationClass::Free);

		// Phase 01: Typed-slot taxonomy — stream/vector ops routed through ALU lanes
		SetClassFlexiblePlacement(SlotClass::AluClass);
	}

	CanonicalDecodePublicationMode VectorALUMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::ProjectorPublishes;
	}

	void VectorALUMicroOp::InitializeMetadata()
	{
		// Vector operations may use vector registers, but metadata is complex
		// For now, mark as empty - will be enhanced in Phase 3
		setReadRegisters({});
		setWriteRegisters({});

		// Memory ranges will be determined by StreamEngine based on addressing mode
		// These will be populated during Execute phase

		// Phase 8: Initialize ResourceMask for GRLB
		// Vector operations use stream engine 0 by default
		std::shared_ptr<StreamEngineId> streamEngine = std::make_shared<StreamEngineId>(StreamEngineId::Zero);
		setResourceMask(ResourceMaskBuilder::ForStreamEngine(streamEngine));
		// Also mark as memory operation (both load and store for stream ops)
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForLoad());
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForStore());

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	bool VectorALUMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Delegate to StreamEngine for execution
		int vtId = std::clamp(getOwnerThreadId(), 0, Processor::CPU_Core::SmtWays - 1);
		ResetStagedRegisterWrites();
		YAKSys_Hybrid_CPU::Execution::StreamEngine::Execute(core, getInstruction(), vtId);
		return true;
	}

	void VectorALUMicroOp::ResetStagedRegisterWrites()
	{
		setWritesRegister(false);
		setDestRegID(VLIW_Instruction::NoReg);
	}

	std::wstring VectorALUMicroOp::GetDescription()
	{
		return std::format(L"VectorALU: OpCode={}, StreamLength={}", getOpCode(), getInstruction().getStreamLength());
	}
}
