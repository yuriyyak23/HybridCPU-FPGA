#include "MicroOp.Control.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../../../Arch/CsrAddresses.h"
#include "../../Safety/ResourceBitset.h"
#include "../../../Architecture/Registers/CSR/CsrAccessPolicy.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		ResourceBitset RegisterReadResourceMask(int registerId)
		{
			ArchRegId checkedRegister;
			return ArchRegId::TryCreate(registerId, checkedRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(checkedRegister))
				: ResourceMaskBuilder::ForRegisterRead(registerId);
		}

		ResourceBitset RegisterWriteResourceMask(int registerId)
		{
			ArchRegId checkedRegister;
			return ArchRegId::TryCreate(registerId, checkedRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(checkedRegister))
				: ResourceMaskBuilder::ForRegisterWrite(registerId);
		}

		const char* CsrStorageSurfaceName(CsrStorageSurface surface)
		{
			switch (surface)
			{
				case CsrStorageSurface::None: return "None";
				case CsrStorageSurface::VectorPodPlane: return "VectorPodPlane";
				case CsrStorageSurface::WiredCsrFile: return "WiredCsrFile";
				default: return "Unknown";
			}
		}
	}

	BranchExecutionPayload::BranchExecutionPayload(bool isExecutable, bool conditionMet, bool redirectsControlFlow, unsigned long long resolvedRetireTargetAddress, unsigned long long materializedResultValue)
		: IsExecutable(isExecutable), ConditionMet(conditionMet), RedirectsControlFlow(redirectsControlFlow),
		  ResolvedRetireTargetAddress(resolvedRetireTargetAddress), MaterializedResultValue(materializedResultValue)
	{
	}

	BranchExecutionPayload::BranchExecutionPayload()
		: BranchExecutionPayload(false, false, false, 0, 0)
	{
	}

	bool BranchExecutionPayload::isExecutable() const
	{
		return IsExecutable;
	}

	bool BranchExecutionPayload::getConditionMet() const
	{
		return ConditionMet;
	}

	bool BranchExecutionPayload::getRedirectsControlFlow() const
	{
		return RedirectsControlFlow;
	}

	unsigned long long BranchExecutionPayload::getResolvedRetireTargetAddress() const
	{
		return ResolvedRetireTargetAddress;
	}

	unsigned long long BranchExecutionPayload::getMaterializedResultValue() const
	{
		return MaterializedResultValue;
	}

	BranchExecutionPayload BranchExecutionPayload::Invalid()
	{
		return BranchExecutionPayload(false, false, false, 0, 0);
	}

	BranchExecutionPayload BranchExecutionPayload::CreateConditional(bool conditionMet, unsigned long long resolvedRetireTargetAddress, unsigned long long materializedResultValue)
	{
		return BranchExecutionPayload(true, conditionMet, conditionMet, conditionMet ? resolvedRetireTargetAddress : 0, materializedResultValue);
	}

	BranchExecutionPayload BranchExecutionPayload::CreateUnconditional(unsigned long long resolvedRetireTargetAddress, unsigned long long materializedResultValue)
	{
		return BranchExecutionPayload(true, true, true, resolvedRetireTargetAddress, materializedResultValue);
	}

	unsigned long long BranchMicroOp::getTargetAddress() const
	{
		return TargetAddress;
	}

	void BranchMicroOp::setTargetAddress(unsigned long long value)
	{
		TargetAddress = value;
	}

	bool BranchMicroOp::isConditional() const
	{
		return IsConditional;
	}

	void BranchMicroOp::setConditional(bool value)
	{
		IsConditional = value;
	}

	bool BranchMicroOp::getConditionMet() const
	{
		return ConditionMet;
	}

	void BranchMicroOp::setConditionMet(bool value)
	{
		ConditionMet = value;
	}

	unsigned short BranchMicroOp::getReg1ID() const
	{
		return Reg1ID;
	}

	void BranchMicroOp::setReg1ID(unsigned short value)
	{
		Reg1ID = value;
	}

	unsigned short BranchMicroOp::getReg2ID() const
	{
		return Reg2ID;
	}

	void BranchMicroOp::setReg2ID(unsigned short value)
	{
		Reg2ID = value;
	}

	short BranchMicroOp::getRelativeTargetDisplacement() const
	{
		return RelativeTargetDisplacement;
	}

	void BranchMicroOp::setRelativeTargetDisplacement(short value)
	{
		RelativeTargetDisplacement = value;
	}

	bool BranchMicroOp::getHasRelativeTargetDisplacement() const
	{
		return HasRelativeTargetDisplacement;
	}

	void BranchMicroOp::setHasRelativeTargetDisplacement(bool value)
	{
		HasRelativeTargetDisplacement = value;
	}

	BranchMicroOp::BranchMicroOp()
	{
		// FSP Metadata: Control flow operations CANNOT be stolen
		setStealable(false);
		setControlFlow(true);
		setClass(MicroOpClass::Control);

		// ISA v4 Phase 02: control flow class; branches are Free ordering (no side effects)
		setInstructionClass(Arch::InstructionClass::ControlFlow);
		setSerializationClass(Arch::SerializationClass::Free);

		// Phase 01: Typed-slot taxonomy — branches pinned to lane 7
		SetHardPinnedPlacement(SlotClass::BranchControl, 7);
	}

	CanonicalDecodePublicationMode BranchMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void BranchMicroOp::ApplyCanonicalRuntimeOperandProjection(unsigned char rd, unsigned char rs1, unsigned char rs2)
	{
		setDestRegID(rd == VLIW_Instruction::NoArchReg ? VLIW_Instruction::NoReg : rd);
		setReg1ID(rs1 == VLIW_Instruction::NoArchReg ? VLIW_Instruction::NoReg : rs1);
		setReg2ID(rs2 == VLIW_Instruction::NoArchReg ? VLIW_Instruction::NoReg : rs2);
	}

	void BranchMicroOp::ApplyCanonicalRuntimeTargetProjection(unsigned long long targetAddress)
	{
		setTargetAddress(targetAddress);
	}

	void BranchMicroOp::ApplyCanonicalRuntimeRelativeTargetProjection(short relativeTargetDisplacement)
	{
		setRelativeTargetDisplacement(relativeTargetDisplacement);
		setHasRelativeTargetDisplacement(true);
	}

	short BranchMicroOp::ResolveRelativeTargetDisplacement()
	{
		if (!getHasRelativeTargetDisplacement())
		{
			throw InvalidOperationException(L"BranchMicroOp relative target resolution requires projected DecoderContext immediate handoff. Raw VLIW_Instruction.Immediate fallback is retired from the decoder-to-runtime ABI.");
		}

		return getRelativeTargetDisplacement();
	}

	unsigned long long BranchMicroOp::ResolveConditionalTargetAddress(unsigned long long executionPc)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)((long)executionPc + ResolveRelativeTargetDisplacement()));
		return static_cast<unsigned long long>(static_cast<long long>(executionPc) + ResolveRelativeTargetDisplacement());
	}

	unsigned long long BranchMicroOp::ResolveUnconditionalTargetAddress(unsigned long long executionPc, unsigned long long baseValue)
	{
		unsigned short opcode = static_cast<unsigned short>(getOpCode());
		switch (opcode)
		{
		case Processor::CPU_Core::IsaOpcodeValues::JAL:
			return static_cast<unsigned long long>(static_cast<long long>(executionPc) + ResolveRelativeTargetDisplacement());
		case Processor::CPU_Core::IsaOpcodeValues::JALR:
			return (baseValue + static_cast<unsigned long long>(static_cast<long long>(ResolveRelativeTargetDisplacement()))) & ~1ULL;
		default:
			return getTargetAddress();
		}
	}

	unsigned long long BranchMicroOp::ResolveLinkRegisterValue(unsigned long long executionPc)
	{
		return executionPc + 4;
	}

	void BranchMicroOp::CaptureResolvedRetireTargetAddress(unsigned long long targetAddress)
	{
		_resolvedRetireTargetAddress = targetAddress;
		_hasResolvedRetireTargetAddress = true;
	}

	void BranchMicroOp::ResetResolvedRetireTargetAddress()
	{
		_resolvedRetireTargetAddress = 0;
		_hasResolvedRetireTargetAddress = false;
	}

	BranchExecutionPayload BranchMicroOp::ResolveExecutionPayload(unsigned short opcode, unsigned long long executionPc, unsigned long long operand1, unsigned long long operand2, unsigned long long baseValue)
	{
		BranchExecutionPayload payload = isConditional() ? ResolveConditionalExecutionPayload(opcode, executionPc, operand1, operand2) : ResolveUnconditionalExecutionPayload(opcode, executionPc, baseValue);

		if (!payload.isExecutable())
		{
			return payload;
		}

		setConditionMet(payload.getConditionMet());
		ResetResolvedRetireTargetAddress();
		if (payload.getRedirectsControlFlow())
		{
			CaptureResolvedRetireTargetAddress(payload.getResolvedRetireTargetAddress());
		}

		return payload;
	}

	BranchExecutionPayload BranchMicroOp::ResolveConditionalExecutionPayload(unsigned short opcode, unsigned long long executionPc, unsigned long long operand1, unsigned long long operand2)
	{
		bool branchTaken = EvaluateBranchCondition(opcode, operand1, operand2);
		unsigned long long targetAddress = ResolveConditionalTargetAddress(executionPc);

		return BranchExecutionPayload::CreateConditional(branchTaken, targetAddress, branchTaken ? targetAddress : executionPc + 256);
	}

	BranchExecutionPayload BranchMicroOp::ResolveUnconditionalExecutionPayload(unsigned short opcode, unsigned long long executionPc, unsigned long long baseValue)
	{
		unsigned long long targetPc = 0;
		switch (opcode)
		{
		case Processor::CPU_Core::IsaOpcodeValues::JAL:
			targetPc = ResolveUnconditionalTargetAddress(executionPc);
			break;
		case Processor::CPU_Core::IsaOpcodeValues::JALR:
			targetPc = ResolveUnconditionalTargetAddress(executionPc, baseValue);
			break;
		default:
			targetPc = getTargetAddress();
			break;
		}

		if (targetPc == 0 && opcode != Processor::CPU_Core::IsaOpcodeValues::JAL && opcode != Processor::CPU_Core::IsaOpcodeValues::JALR)
		{
			return BranchExecutionPayload::Invalid();
		}

		return BranchExecutionPayload::CreateUnconditional(targetPc, getWritesRegister() ? ResolveLinkRegisterValue(executionPc) : 0);
	}

	bool BranchMicroOp::EvaluateBranchCondition(unsigned short opcode, unsigned long long operand1, unsigned long long operand2)
	{
		bool temp_var114;
		switch (opcode)
		{

			case Processor::CPU_Core::IsaOpcodeValues::BEQ:
				temp_var114 = operand1 == operand2;
				break;
			case Processor::CPU_Core::IsaOpcodeValues::BNE:
				temp_var114 = operand1 != operand2;
				break;
			case Processor::CPU_Core::IsaOpcodeValues::BLT:
				temp_var114 = static_cast<long long>(operand1) < static_cast<long long>(operand2);
				break;
			case Processor::CPU_Core::IsaOpcodeValues::BGE:
				temp_var114 = static_cast<long long>(operand1) >= static_cast<long long>(operand2);
				break;
			case Processor::CPU_Core::IsaOpcodeValues::BLTU:
				temp_var114 = operand1 < operand2;
				break;
			case Processor::CPU_Core::IsaOpcodeValues::BGEU:
				temp_var114 = operand1 >= operand2;
				break;
			default:
				temp_var114 = false;
				break;
		}
		return temp_var114;
	}

	void BranchMicroOp::InitializeMetadata()
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ushort opcode = unchecked((ushort)OpCode);
		unsigned short opcode = static_cast<unsigned short>(getOpCode());
		bool publishesLinkRegister = ((opcode == Processor::CPU_Core::IsaOpcodeValues::JAL || opcode == Processor::CPU_Core::IsaOpcodeValues::JALR)) && getDestRegID() != 0 && getDestRegID() != VLIW_Instruction::NoReg;

		setWritesRegister(publishesLinkRegister);

		if (isConditional())
		{
			setReadRegisters(std::vector<int> {static_cast<int>(getReg1ID()), static_cast<int>(getReg2ID())});
		}
		else if (opcode == Processor::CPU_Core::IsaOpcodeValues::JALR && getReg1ID() != VLIW_Instruction::NoReg)
		{
			setReadRegisters(std::vector<int> {static_cast<int>(getReg1ID())});
		}
		else
		{
			setReadRegisters(std::vector<int>{});
		}

		setWriteRegisters(publishesLinkRegister ? std::vector<int>{static_cast<int>(getDestRegID())} : std::vector<int>{});

		// Phase 8: Initialize ResourceMask for GRLB
		setResourceMask(ResourceBitset::Zero);
		// Control flow operations read from registers for conditional branches
		if (isConditional())
		{
			setResourceMask(getResourceMask() | RegisterReadResourceMask(getReg1ID()));
			setResourceMask(getResourceMask() | RegisterReadResourceMask(getReg2ID()));
		}
		else if (opcode == Processor::CPU_Core::IsaOpcodeValues::JALR && getReg1ID() != VLIW_Instruction::NoReg)
		{
			setResourceMask(getResourceMask() | RegisterReadResourceMask(getReg1ID()));
		}

		if (publishesLinkRegister)
		{
			setResourceMask(getResourceMask() | RegisterWriteResourceMask(getDestRegID()));
		}

		// Note: Control flow operations are not stealable.
		// so they don't conflict with other operations in FSP scenarios

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	void BranchMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	bool BranchMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Evaluate condition if conditional branch
		if (isConditional())
		{
			// Condition evaluation depends on specific branch type
			// For now, delegate to existing control flow methods
		}

		// Control flow execution handled by existing methods in Decoder
		return true;
	}

	std::wstring BranchMicroOp::GetDescription()
	{
		return std::format(L"Branch: OpCode={}, Target=0x{:X}, {}", std::to_wstring(getOpCode()), getTargetAddress(),
			isConditional() ? (getConditionMet() ? L"Conditional=true" : L"Conditional=false") : L"Unconditional");
	}

	void BranchMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		bool hasArchitecturalDestinationRegister = getDestRegID() != 0 && getDestRegID() != VLIW_Instruction::NoReg;
		bool redirectsControlFlow = !isConditional() || getConditionMet();
		if (!redirectsControlFlow && (!getWritesRegister() || !hasArchitecturalDestinationRegister))
		{
			return;
		}

		VtId checkedOwner;
		int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());

		if (getWritesRegister() && hasArchitecturalDestinationRegister)
		{
			if (!_hasCapturedPrimaryWriteBackResult)
			{
				throw InvalidOperationException(L"BranchMicroOp retire emission requires an execute-stage captured primary write-back value for the architectural link register.");
			}

			AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), _capturedPrimaryWriteBackResult));
		}

		if (!redirectsControlFlow)
		{
			return;
		}

		if (!_hasResolvedRetireTargetAddress)
		{
			throw InvalidOperationException(L"BranchMicroOp retire emission requires an execute-stage resolved control-flow target address. Production control-flow retirement must not reconstruct JAL/JALR redirects from legacy decoder placeholders.");
		}

		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::PcWrite(vtId, _resolvedRetireTargetAddress));
	}

	bool BranchMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _capturedPrimaryWriteBackResult;
		return _hasCapturedPrimaryWriteBackResult && getWritesRegister() && getDestRegID() != 0 && getDestRegID() != VLIW_Instruction::NoReg;
	}

	void BranchMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_capturedPrimaryWriteBackResult = value;
		_hasCapturedPrimaryWriteBackResult = true;
	}

	CsrRetireEffect::CsrRetireEffect(bool clearsArchitecturalExceptionState, CsrStorageSurface storageSurface, unsigned short csrAddress, unsigned long long readValue, bool hasRegisterWriteback, unsigned short destRegId, bool hasCsrWrite, unsigned long long csrWriteValue)
		: ClearsArchitecturalExceptionState(clearsArchitecturalExceptionState), StorageSurface(storageSurface), CsrAddress(csrAddress),
		  ReadValue(readValue), HasRegisterWriteback(hasRegisterWriteback), DestRegId(destRegId), HasCsrWrite(hasCsrWrite), CsrWriteValue(csrWriteValue)
	{
	}

	bool CsrRetireEffect::getClearsArchitecturalExceptionState() const
	{
		return ClearsArchitecturalExceptionState;
	}

	CsrStorageSurface CsrRetireEffect::getStorageSurface() const
	{
		return StorageSurface;
	}

	unsigned short CsrRetireEffect::getCsrAddress() const
	{
		return CsrAddress;
	}

	unsigned long long CsrRetireEffect::getReadValue() const
	{
		return ReadValue;
	}

	bool CsrRetireEffect::getHasRegisterWriteback() const
	{
		return HasRegisterWriteback;
	}

	unsigned short CsrRetireEffect::getDestRegId() const
	{
		return DestRegId;
	}

	bool CsrRetireEffect::getHasCsrWrite() const
	{
		return HasCsrWrite;
	}

	unsigned long long CsrRetireEffect::getCsrWriteValue() const
	{
		return CsrWriteValue;
	}

	CsrRetireEffect CsrRetireEffect::ClearExceptionCounters()
	{
		return CsrRetireEffect(true, CsrStorageSurface::None, 0, 0, false, 0, false, 0);
	}

	CsrRetireEffect CsrRetireEffect::Create(CsrStorageSurface storageSurface, unsigned short csrAddress, unsigned long long readValue, bool hasRegisterWriteback, unsigned short destRegId, bool hasCsrWrite, unsigned long long csrWriteValue)
	{
		return CsrRetireEffect(false, storageSurface, csrAddress, readValue, hasRegisterWriteback, destRegId, hasCsrWrite, csrWriteValue);
	}

	AtomicRetireEffect::AtomicRetireEffect()
		: IsValid(false), Opcode(0), AccessSize(0), Address(0), SourceValue(0),
		  DestinationRegister(0), CoreId(0), VirtualThreadId(0), AcquireOrdering(false), ReleaseOrdering(false)
	{
	}

	AtomicRetireEffect::AtomicRetireEffect(bool isValid, unsigned short opcode, unsigned char accessSize, unsigned long long address, unsigned long long sourceValue, unsigned short destinationRegister, unsigned short coreId, int virtualThreadId, bool acquireOrdering, bool releaseOrdering)
		: IsValid(isValid), Opcode(opcode), AccessSize(accessSize), Address(address), SourceValue(sourceValue),
		  DestinationRegister(destinationRegister), CoreId(coreId), VirtualThreadId(virtualThreadId), AcquireOrdering(acquireOrdering), ReleaseOrdering(releaseOrdering)
	{
	}

	bool AtomicRetireEffect::isValid() const
	{
		return IsValid;
	}

	unsigned short AtomicRetireEffect::getOpcode() const
	{
		return Opcode;
	}

	unsigned char AtomicRetireEffect::getAccessSize() const
	{
		return AccessSize;
	}

	unsigned long long AtomicRetireEffect::getAddress() const
	{
		return Address;
	}

	unsigned long long AtomicRetireEffect::getSourceValue() const
	{
		return SourceValue;
	}

	unsigned short AtomicRetireEffect::getDestinationRegister() const
	{
		return DestinationRegister;
	}

	unsigned short AtomicRetireEffect::getCoreId() const
	{
		return CoreId;
	}

	int AtomicRetireEffect::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	bool AtomicRetireEffect::getAcquireOrdering() const
	{
		return AcquireOrdering;
	}

	bool AtomicRetireEffect::getReleaseOrdering() const
	{
		return ReleaseOrdering;
	}

	bool AtomicRetireEffect::getHasRegisterDestination() const
	{
		return getDestinationRegister() != 0 && getDestinationRegister() != VLIW_Instruction::NoReg;
	}

	AtomicRetireEffect AtomicRetireEffect::Create(unsigned short opcode, unsigned char accessSize, unsigned long long address, unsigned long long sourceValue, unsigned short destinationRegister, unsigned short coreId, int virtualThreadId, bool acquireOrdering, bool releaseOrdering)
	{
		return AtomicRetireEffect(true, opcode, accessSize, address, sourceValue, destinationRegister, coreId, virtualThreadId, acquireOrdering, releaseOrdering);
	}

	AtomicRetireOutcome::AtomicRetireOutcome(bool hasRegisterWriteback, unsigned short registerDestination, unsigned long long registerWritebackValue, bool memoryMutated, YAKSys_Hybrid_CPU::Core::TraceEventKind traceEventKind)
		: HasRegisterWriteback(hasRegisterWriteback), RegisterDestination(registerDestination), RegisterWritebackValue(registerWritebackValue),
		  MemoryMutated(memoryMutated), TraceEventKind(traceEventKind)
	{
	}

	bool AtomicRetireOutcome::getHasRegisterWriteback() const
	{
		return HasRegisterWriteback;
	}

	unsigned short AtomicRetireOutcome::getRegisterDestination() const
	{
		return RegisterDestination;
	}

	unsigned long long AtomicRetireOutcome::getRegisterWritebackValue() const
	{
		return RegisterWritebackValue;
	}

	bool AtomicRetireOutcome::getMemoryMutated() const
	{
		return MemoryMutated;
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind AtomicRetireOutcome::getTraceEventKind() const
	{
		return TraceEventKind;
	}

	AtomicRetireOutcome AtomicRetireOutcome::Create(AtomicRetireEffect const effect, unsigned long long registerWritebackValue, bool hasRegisterWriteback, bool memoryMutated)
	{
		return AtomicRetireOutcome(hasRegisterWriteback, effect.getDestinationRegister(), registerWritebackValue, memoryMutated, ClassifyTraceEventKind(effect, registerWritebackValue));
	}

	YAKSys_Hybrid_CPU::Core::TraceEventKind AtomicRetireOutcome::ClassifyTraceEventKind(AtomicRetireEffect const effect, unsigned long long registerWritebackValue)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return effect.Opcode switch
	//			{
	//				Processor.CPU_Core.IsaOpcodeValues.LR_W or Processor.CPU_Core.IsaOpcodeValues.LR_D => TraceEventKind.LrExecuted,
	//ignore
	//				Processor.CPU_Core.IsaOpcodeValues.SC_W or Processor.CPU_Core.IsaOpcodeValues.SC_D => registerWritebackValue == 0 ? TraceEventKind.ScSucceeded : TraceEventKind.ScFailed,
	//ignore
	//				Processor.CPU_Core.IsaOpcodeValues.AMOSWAP_D or Processor.CPU_Core.IsaOpcodeValues.AMOADD_D or Processor.CPU_Core.IsaOpcodeValues.AMOXOR_D or Processor.CPU_Core.IsaOpcodeValues.AMOAND_D or Processor.CPU_Core.IsaOpcodeValues.AMOOR_D or Processor.CPU_Core.IsaOpcodeValues.AMOMIN_D or Processor.CPU_Core.IsaOpcodeValues.AMOMAX_D or Processor.CPU_Core.IsaOpcodeValues.AMOMINU_D or Processor.CPU_Core.IsaOpcodeValues.AMOMAXU_D => TraceEventKind.AmoDwordExecuted,
	//ignore
	//ignore
	//ignore
	//ignore
	//ignore
	//ignore
	//ignore
	//ignore
	//				_ => TraceEventKind.AmoWordExecuted
	//			};
	}

	unsigned long long CSRMicroOp::getCSRAddress() const
	{
		return CSRAddress;
	}

	void CSRMicroOp::setCSRAddress(unsigned long long value)
	{
		CSRAddress = value;
	}

	unsigned long long CSRMicroOp::getWriteValue() const
	{
		return WriteValue;
	}

	void CSRMicroOp::setWriteValue(unsigned long long value)
	{
		WriteValue = value;
	}

	unsigned short CSRMicroOp::getSrcRegID() const
	{
		return SrcRegID;
	}

	void CSRMicroOp::setSrcRegID(unsigned short value)
	{
		SrcRegID = value;
	}

	bool CSRMicroOp::getReadsCsr() const
	{
		return true;
	}

	bool CSRMicroOp::getWritesCsr() const
	{
		return false;
	}

	bool CSRMicroOp::getWritesFromSourceRegister() const
	{
		return false;
	}

	bool CSRMicroOp::getUsesSourceRegisterWriteValue() const
	{
		return false;
	}

	bool CSRMicroOp::getClearsArchitecturalExceptionState() const
	{
		return false;
	}

	std::wstring CSRMicroOp::getDescriptionVerb() const
	{
		return L"CSR";
	}

	bool CSRMicroOp::HasArchitecturalRegister(unsigned short registerId)
	{
		return registerId != 0 && registerId != VLIW_Instruction::NoReg && registerId != VLIW_Instruction::NoArchReg;
	}

	bool CSRMicroOp::getHasArchitecturalSourceRegister() const
	{
		return HasArchitecturalRegister(getSrcRegID());
	}

	bool CSRMicroOp::getHasArchitecturalDestinationRegister() const
	{
		return HasArchitecturalRegister(getDestRegID());
	}

	CSRMicroOp::CSRMicroOp()
	{
		setStealable(false);
		setHasSideEffects(true);

		setInstructionClass(Arch::InstructionClass::Csr);
		setSerializationClass(Arch::SerializationClass::CsrOrdered);

		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);
	}

	CanonicalDecodePublicationMode CSRMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void CSRMicroOp::InitializeMetadata()
	{
		bool publishesSourceRegisterRead = getWritesFromSourceRegister() && getHasArchitecturalSourceRegister();
		bool publishesRegisterWriteback = getReadsCsr() && IsRegisterWritebackConfigured() && getHasArchitecturalDestinationRegister();

		setWritesRegister(publishesRegisterWriteback);

		setReadRegisters(publishesSourceRegisterRead ? std::vector<int>{static_cast<int>(getSrcRegID())} : std::vector<int>{});

		setWriteRegisters(publishesRegisterWriteback ? std::vector<int>{static_cast<int>(getDestRegID())} : std::vector<int>{});

		setResourceMask(ResourceBitset::Zero);
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForAtomic());

		if (publishesSourceRegisterRead)
		{
			setResourceMask(getResourceMask() | RegisterReadResourceMask(getSrcRegID()));
		}

		if (publishesRegisterWriteback)
		{
			setResourceMask(getResourceMask() | RegisterWriteResourceMask(getDestRegID()));
		}

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	void CSRMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	bool CSRMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (getClearsArchitecturalExceptionState())
		{
			return true;
		}

		if (getReadsCsr())
		{
			CsrStorageSurface storageSurface = ResolveStorageSurface(core, getCSRAddress());
			_readValue = ReadCsr(core, storageSurface, getCSRAddress());
		}

		return true;
	}

	CsrRetireEffect CSRMicroOp::CreateRetireEffect(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (getClearsArchitecturalExceptionState())
		{
			return CsrRetireEffect::ClearExceptionCounters();
		}

		CsrStorageSurface storageSurface = ResolveStorageSurface(core, getCSRAddress());
		unsigned long long priorValue = ReadCsr(core, storageSurface, getCSRAddress());
		bool hasRegisterWriteback = getReadsCsr() && getWritesRegister() && getHasArchitecturalDestinationRegister();
		bool hasCsrWrite = getWritesCsr();
		unsigned long long csrWriteValue = hasCsrWrite ? ResolveWriteValue(core, priorValue) : 0;

		return CsrRetireEffect::Create(storageSurface, static_cast<unsigned short>(getCSRAddress() & 0xFFFF), priorValue, hasRegisterWriteback, getDestRegID(), hasCsrWrite, csrWriteValue);
	}

	void CSRMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (getReadsCsr() && getWritesRegister() && getHasArchitecturalDestinationRegister())
		{
			VtId checkedOwner;
			int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());
			AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), _readValue));
		}
	}

	bool CSRMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _readValue;
		return getReadsCsr() && getWritesRegister() && getHasArchitecturalDestinationRegister();
	}

	void CSRMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_readValue = value;
	}

	std::wstring CSRMicroOp::GetDescription()
	{
		if (getClearsArchitecturalExceptionState())
		{
			return L"CSR_CLEAR";
		}

		return getReadsCsr()
			? std::format(L"{}: CSR=0x{:X}, Dest=R{}", getDescriptionVerb(), getCSRAddress(), getDestRegID())
			: std::format(L"{}: CSR=0x{:X}, Value=0x{:X}", getDescriptionVerb(), getCSRAddress(), getWriteValue());
	}

	unsigned long long CSRMicroOp::ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue)
	{
		return getWriteValue();
	}

	bool CSRMicroOp::IsRegisterWritebackConfigured()
	{
		if (!_registerWritebackCapabilitySeeded || getWritesRegister())
		{
			_registerWritebackConfigured = getWritesRegister();
			_registerWritebackCapabilitySeeded = true;
		}

		return _registerWritebackConfigured;
	}

	unsigned long long CSRMicroOp::ResolveConfiguredWriteValue(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (!getUsesSourceRegisterWriteValue())
		{
			return getWriteValue();
		}

		if (!getHasArchitecturalSourceRegister())
		{
			return getWriteValue();
		}

		VtId checkedOwner;
		int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());
		unsigned long long value;
		return TryReadUnifiedArchValue(core, vtId, getSrcRegID(), value) ? value : getWriteValue();
	}

	CsrStorageSurface CSRMicroOp::ResolveStorageSurface(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long addr)
	{
		int csrAddr = static_cast<int>(addr);

		if (csrAddr >= Processor::CPU_Core::CSR_POD_ID && csrAddr <= Processor::CPU_Core::CSR_NOC_ROUTE_CFG)
		{
			return CsrStorageSurface::VectorPodPlane;
		}

		switch (csrAddr)
		{
			case CsrAddresses::VexcpMask:
			case CsrAddresses::VexcpPri:
			case static_cast<int>(Processor::CPU_Core::VectorCSR::VLIW_STEAL_ENABLE):
			case static_cast<int>(Processor::CPU_Core::VectorCSR::VLIW_STEAL_MASK):
			case static_cast<int>(Processor::CPU_Core::VectorCSR::VLIW_STEAL_POLICY):
				return CsrStorageSurface::VectorPodPlane;
		}

		unsigned short wiredCsrAddress = static_cast<unsigned short>(addr & 0xFFFF);
		if (core->getCsr()->IsRegistered(wiredCsrAddress))
		{
			return CsrStorageSurface::WiredCsrFile;
		}

		throw InvalidOperationException(std::format("CSR address 0x{:03X} does not expose an authoritative CSR storage surface on the current mainline retire path; refusing hidden success/no-op.", wiredCsrAddress));
	}

	unsigned long long CSRMicroOp::ReadCsr(std::shared_ptr<Processor::CPU_Core> &core, CsrStorageSurface storageSurface, unsigned long long addr)
	{
		int csrAddr = static_cast<int>(addr);

		switch (storageSurface)
		{
			case CsrStorageSurface::VectorPodPlane:
				return ReadVectorPodPlaneCsr(core, csrAddr);
			case CsrStorageSurface::WiredCsrFile:
				return core->getCsr()->Read(static_cast<unsigned short>(addr & 0xFFFF), PrivilegeLevel::Machine);
			default:
				throw InvalidOperationException(std::format("Unsupported CSR storage surface {} for read at 0x{:03X}.", CsrStorageSurfaceName(storageSurface), static_cast<unsigned short>(addr & 0xFFFF)));
		}
	}

	void CSRMicroOp::WriteCsr(std::shared_ptr<Processor::CPU_Core> &core, CsrStorageSurface storageSurface, unsigned long long addr, unsigned long long value)
	{
		int csrAddr = static_cast<int>(addr);

		switch (storageSurface)
		{
			case CsrStorageSurface::VectorPodPlane:
				WriteVectorPodPlaneCsr(core, csrAddr, value);
				return;

			case CsrStorageSurface::WiredCsrFile:
				core->getCsr()->Write(static_cast<unsigned short>(addr & 0xFFFF), value, PrivilegeLevel::Machine);
				return;

			default:
				throw InvalidOperationException(std::format("Unsupported CSR storage surface {} for write at 0x{:03X}.", CsrStorageSurfaceName(storageSurface), static_cast<unsigned short>(addr & 0xFFFF)));
		}
	}

	unsigned long long CSRMicroOp::ReadVectorPodPlaneCsr(std::shared_ptr<Processor::CPU_Core> &core, int csrAddr)
	{
		if (csrAddr >= Processor::CPU_Core::CSR_POD_ID && csrAddr <= Processor::CPU_Core::CSR_NOC_ROUTE_CFG)
		{
			return core->ReadPodCSR(csrAddr);
		}

		switch (csrAddr)
		{
			case CsrAddresses::VexcpMask:
				return core->getExceptionStatus()->GetMask();
			case CsrAddresses::VexcpPri:
				return ReadPackedExceptionPriorities(core);
			case static_cast<int>(Processor::CPU_Core::VectorCSR::VLIW_STEAL_ENABLE):
				return core->getVectorConfig()->getFSPEnabled();
			case static_cast<int>(Processor::CPU_Core::VectorCSR::VLIW_STEAL_MASK):
				return core->getVectorConfig()->getFSPStealMask();
			case static_cast<int>(Processor::CPU_Core::VectorCSR::VLIW_STEAL_POLICY):
				return core->getVectorConfig()->getFSPPolicy();
			default:
				throw InvalidOperationException(std::format(
					"CSR address 0x{:03X} is not backed by the vector/pod CSR plane.",
					static_cast<unsigned short>(csrAddr)));
		}
	}

	void CSRMicroOp::WriteVectorPodPlaneCsr(std::shared_ptr<Processor::CPU_Core> &core, int csrAddr, unsigned long long value)
	{
		core->WriteCanonicalVectorPodPlaneCsr(csrAddr, value);
	}

	unsigned long long CSRMicroOp::ReadPackedExceptionPriorities(std::shared_ptr<Processor::CPU_Core> &core)
	{
		unsigned long long packed = 0;
		for (int i = 0; i < 5; i++)
		{
			packed |= static_cast<unsigned long long>(core->getExceptionStatus()->GetPriority(i) & 0x7) << (i * 3);
		}

		return packed;
	}

	bool CsrReadWriteMicroOp::getWritesCsr() const
	{
		return true;
	}

	bool CsrReadWriteMicroOp::getWritesFromSourceRegister() const
	{
		return true;
	}

	bool CsrReadWriteMicroOp::getUsesSourceRegisterWriteValue() const
	{
		return true;
	}

	std::wstring CsrReadWriteMicroOp::getDescriptionVerb() const
	{
		return L"CSR_READ_WRITE";
	}

	unsigned long long CsrReadWriteMicroOp::ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue)
	{
		return ResolveConfiguredWriteValue(core);
	}

	bool CsrReadSetMicroOp::getWritesCsr() const
	{
		return getUsesSourceRegisterWriteValue();
	}

	bool CsrReadSetMicroOp::getWritesFromSourceRegister() const
	{
		return getUsesSourceRegisterWriteValue();
	}

	bool CsrReadSetMicroOp::getUsesSourceRegisterWriteValue() const
	{
		return getSrcRegID() != 0 && getSrcRegID() != VLIW_Instruction::NoReg;
	}

	std::wstring CsrReadSetMicroOp::getDescriptionVerb() const
	{
		return L"CSR_READ_SET";
	}

	unsigned long long CsrReadSetMicroOp::ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue)
	{
		return priorValue | ResolveConfiguredWriteValue(core);
	}

	bool CsrReadClearMicroOp::getWritesCsr() const
	{
		return getUsesSourceRegisterWriteValue();
	}

	bool CsrReadClearMicroOp::getWritesFromSourceRegister() const
	{
		return getUsesSourceRegisterWriteValue();
	}

	bool CsrReadClearMicroOp::getUsesSourceRegisterWriteValue() const
	{
		return getSrcRegID() != 0 && getSrcRegID() != VLIW_Instruction::NoReg;
	}

	std::wstring CsrReadClearMicroOp::getDescriptionVerb() const
	{
		return L"CSR_READ_CLEAR";
	}

	unsigned long long CsrReadClearMicroOp::ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue)
	{
		return priorValue & ~ResolveConfiguredWriteValue(core);
	}

	bool CsrReadWriteImmediateMicroOp::getWritesCsr() const
	{
		return true;
	}

	bool CsrReadWriteImmediateMicroOp::getWritesFromSourceRegister() const
	{
		return false;
	}

	bool CsrReadWriteImmediateMicroOp::getUsesSourceRegisterWriteValue() const
	{
		return false;
	}

	std::wstring CsrReadWriteImmediateMicroOp::getDescriptionVerb() const
	{
		return L"CSR_READ_WRITE_IMM";
	}

	bool CsrReadSetImmediateMicroOp::getWritesCsr() const
	{
		return getWriteValue() != 0;
	}

	bool CsrReadSetImmediateMicroOp::getWritesFromSourceRegister() const
	{
		return false;
	}

	bool CsrReadSetImmediateMicroOp::getUsesSourceRegisterWriteValue() const
	{
		return false;
	}

	std::wstring CsrReadSetImmediateMicroOp::getDescriptionVerb() const
	{
		return L"CSR_READ_SET_IMM";
	}

	unsigned long long CsrReadSetImmediateMicroOp::ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue)
	{
		return priorValue | getWriteValue();
	}

	bool CsrReadClearImmediateMicroOp::getWritesCsr() const
	{
		return getWriteValue() != 0;
	}

	bool CsrReadClearImmediateMicroOp::getWritesFromSourceRegister() const
	{
		return false;
	}

	bool CsrReadClearImmediateMicroOp::getUsesSourceRegisterWriteValue() const
	{
		return false;
	}

	std::wstring CsrReadClearImmediateMicroOp::getDescriptionVerb() const
	{
		return L"CSR_READ_CLEAR_IMM";
	}

	unsigned long long CsrReadClearImmediateMicroOp::ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue)
	{
		return priorValue & ~getWriteValue();
	}

	std::wstring CsrReadCounterMicroOp::getDescriptionVerb() const
	{
		return L"CSR_READ_COUNTER";
	}

	bool CsrClearMicroOp::getReadsCsr() const
	{
		return false;
	}

	bool CsrClearMicroOp::getClearsArchitecturalExceptionState() const
	{
		return true;
	}

	std::wstring CsrClearMicroOp::getDescriptionVerb() const
	{
		return L"CSR_CLEAR";
	}
}
