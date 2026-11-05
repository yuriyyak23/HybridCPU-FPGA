#include "MicroOp.Misc.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../Safety/ResourceBitset.h"
#include "../../../Execution/Memory/AtomicMemory/AtomicMemoryUnit.h"
#include "../../../Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "../../Scheduling/SlotPlacementMetadata.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include <algorithm>
#include "../../../../../NonRTL/Arch/SerializationClass.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	NopMicroOp::NopMicroOp()
	{
		// NOP doesn't read or write anything
		setReadRegisters({});
		setWriteRegisters({});
		setReadMemoryRanges({});
		setWriteMemoryRanges({});

		// Phase 8: Initialize ResourceMask for GRLB
		// NOP uses no resources
		setResourceMask(0);

		// Phase 01: Typed-slot taxonomy — NOP placeholder, no class affinity
		SetClassFlexiblePlacement(SlotClass::Unclassified);
	}

	CanonicalDecodePublicationMode NopMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	bool NopMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		return true;
	}

	std::wstring NopMicroOp::GetDescription()
	{
		return L"NOP";
	}

	VLIW_Instruction GenericMicroOp::getInstruction() const
	{
		return Instruction;
	}

	void GenericMicroOp::setInstruction(VLIW_Instruction value)
	{
		Instruction = value;
	}

	GenericMicroOp::GenericMicroOp()
	{
		// FSP Metadata: Generic operations — conservative, not stealable until properly classified
		setStealable(false);

		// Cannot determine register/memory dependencies without instruction details
		setReadRegisters({});
		setWriteRegisters({});
		setReadMemoryRanges({});
		setWriteMemoryRanges({});

		// Phase 8: Initialize ResourceMask for GRLB
		// Conservative: assume it needs all resources (atomic operation)
		setResourceMask(ResourceMaskBuilder::ForAtomic());

		// Phase 01: Typed-slot taxonomy — conservative fallback
		SetPlacement(SlotClass::Unclassified, SlotPinningKind::HardPinned);
	}

	CanonicalDecodePublicationMode GenericMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::ProjectorPublishes;
	}

	bool GenericMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		// These operations are executed via legacy methods in decoder
		// This is a transitional MicroOp for instructions not yet fully migrated
		return true;
	}

	std::wstring GenericMicroOp::GetDescription()
	{
		return std::format(L"Generic: OpCode={}", getOpCode());
	}

	unsigned long long AtomicMicroOp::getAddress() const
	{
		return Address;
	}

	void AtomicMicroOp::setAddress(unsigned long long value)
	{
		Address = value;
	}

	unsigned short AtomicMicroOp::getBaseRegID() const
	{
		return BaseRegID;
	}

	void AtomicMicroOp::setBaseRegID(unsigned short value)
	{
		BaseRegID = value;
	}

	unsigned short AtomicMicroOp::getSrcRegID() const
	{
		return SrcRegID;
	}

	void AtomicMicroOp::setSrcRegID(unsigned short value)
	{
		SrcRegID = value;
	}

	unsigned char AtomicMicroOp::getSize() const
	{
		return Size;
	}

	void AtomicMicroOp::setSize(unsigned char value)
	{
		Size = value;
	}

	bool AtomicMicroOp::getAcquireOrdering() const
	{
		return AcquireOrdering;
	}

	void AtomicMicroOp::setAcquireOrdering(bool value)
	{
		AcquireOrdering = value;
	}

	bool AtomicMicroOp::getReleaseOrdering() const
	{
		return ReleaseOrdering;
	}

	void AtomicMicroOp::setReleaseOrdering(bool value)
	{
		ReleaseOrdering = value;
	}

	unsigned long long AtomicMicroOp::getMemoryAddress() const
	{
		return getAddress();
	}

	AtomicMicroOp::AtomicMicroOp()
	{
		setMemoryOp(true);
		setHasSideEffects(true);
		setClass(MicroOpClass::Lsu);

		setInstructionClass(Arch::InstructionClass::Atomic);
		setSerializationClass(Arch::SerializationClass::AtomicSerial);

		SetClassFlexiblePlacement(SlotClass::LsuClass);
	}

	void AtomicMicroOp::InitializeMetadata()
	{
		constexpr unsigned short noReg = VLIW_Instruction::NoReg;

		std::vector<int> readRegs;
		if (getBaseRegID() != noReg)
		{
			readRegs.push_back(getBaseRegID());
		}

		if (getUsesSourceRegister() && getSrcRegID() != noReg)
		{
			readRegs.push_back(getSrcRegID());
		}

		setReadRegisters(readRegs);
		setWriteRegisters(getWritesRegister() && getDestRegID() != 0 && getDestRegID() != noReg
			? std::vector<int>{static_cast<int>(getDestRegID())}
			: std::vector<int>{});
		const auto memoryRange = MemoryRange{getAddress(), static_cast<unsigned long long>(std::max(getSize(), static_cast<unsigned char>(4)))};
		setReadMemoryRanges({memoryRange});
		setWriteMemoryRanges({memoryRange});

		setResourceMask(ResourceBitset::Zero);
		if (getBaseRegID() != noReg)
		{
			ArchRegId baseRegister;
			const ResourceBitset baseReadResource = ArchRegId::TryCreate(getBaseRegID(), baseRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(baseRegister))
				: ResourceMaskBuilder::ForRegisterRead(getBaseRegID());
			setResourceMask(getResourceMask() | baseReadResource);
		}

		if (getUsesSourceRegister() && getSrcRegID() != noReg)
		{
			ArchRegId sourceRegister;
			const ResourceBitset sourceReadResource = ArchRegId::TryCreate(getSrcRegID(), sourceRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(sourceRegister))
				: ResourceMaskBuilder::ForRegisterRead(getSrcRegID());
			setResourceMask(getResourceMask() | sourceReadResource);
		}

		if (getWritesRegister() && getDestRegID() != 0 && getDestRegID() != noReg)
		{
			ArchRegId destinationRegister;
			const ResourceBitset destinationWriteResource = ArchRegId::TryCreate(getDestRegID(), destinationRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(destinationRegister))
				: ResourceMaskBuilder::ForRegisterWrite(getDestRegID());
			setResourceMask(getResourceMask() | destinationWriteResource);
		}

		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForAtomic());
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForMemoryDomain(getOwnerThreadId()));

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	void AtomicMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	bool AtomicMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		VtId checkedOwner;
		int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());
		unsigned long long address = ReadUnifiedScalarSourceOperand(core, vtId, getBaseRegID());
		unsigned long long sourceValue = getUsesSourceRegister() ? ReadUnifiedScalarSourceOperand(core, vtId, getSrcRegID()) : 0;

		setAddress(address);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: _resolvedRetireEffect = core.AtomicMemoryUnit.ResolveRetireEffect(unchecked((ushort)OpCode), DestRegID, address, sourceValue, (int)core.CoreID, vtId, AcquireOrdering, ReleaseOrdering);
		_resolvedRetireEffect.emplace(YAKSys_Hybrid_CPU::Core::Memory::AtomicMemoryUnitExtensions::ResolveRetireEffect(core->getAtomicMemoryUnit(), static_cast<unsigned short>(getOpCode()), getDestRegID(), address, sourceValue, static_cast<int>(core->getCoreID()), vtId, getAcquireOrdering(), getReleaseOrdering()));
		return true;
	}

	AtomicRetireEffect AtomicMicroOp::CreateRetireEffect()
	{
		return _resolvedRetireEffect.value_or(AtomicRetireEffect{});
	}

	std::wstring AtomicMicroOp::GetDescription()
	{
		return std::format(L"Atomic: OpCode={}, Addr=0x{:X}, Size={}, Dest=R{}", getOpCode(), getAddress(), getSize(), getDestRegID());
	}

	bool AtomicMicroOp::getUsesSourceRegister() const
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::IsaOpcodeValues::LR_W) &&
			getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::IsaOpcodeValues::LR_D);
	}

	std::shared_ptr<ICustomAccelerator> CustomAcceleratorMicroOp::getAccelerator() const
	{
		return Accelerator;
	}

	void CustomAcceleratorMicroOp::setAccelerator(std::shared_ptr<ICustomAccelerator> value)
	{
		Accelerator = value;
	}

	std::vector<unsigned long long> CustomAcceleratorMicroOp::getOperands() const
	{
		return Operands;
	}

	void CustomAcceleratorMicroOp::setOperands(const std::vector<unsigned long long> &value)
	{
		Operands = value;
	}

	std::vector<unsigned char> CustomAcceleratorMicroOp::getConfig() const
	{
		return Config;
	}

	void CustomAcceleratorMicroOp::setConfig(const std::vector<unsigned char> &value)
	{
		Config = value;
	}

	CustomAcceleratorMicroOp::CustomAcceleratorMicroOp()
	{
		// Retained compatibility metadata only; not active execution authority.
		setClass(MicroOpClass::Other);

		// ISA v4 Phase 02: custom accelerators are System class, FullSerial ordering
		setInstructionClass(Arch::InstructionClass::System);
		setSerializationClass(Arch::SerializationClass::FullSerial);

		// Phase 01: Typed-slot taxonomy — will be configurable per-accelerator in future phases
		SetPlacement(SlotClass::Unclassified, SlotPinningKind::HardPinned);
	}

	CanonicalDecodePublicationMode CustomAcceleratorMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::ProjectorPublishes;
	}

	void CustomAcceleratorMicroOp::InitializeMetadata(int acceleratorId, std::vector<int> &inputRegIds, int outputRegId)
	{
		// Set register dependencies
		setReadRegisters(inputRegIds);
		if (getWritesRegister())
		{
			setWriteRegisters(std::vector<int> {outputRegId});
		}

		// Phase 8: Initialize ResourceMask for GRLB
		setResourceMask(ResourceBitset::Zero);

		// Add accelerator resource
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForAccelerator(acceleratorId));

		// Add register read resources
		for (auto regId : inputRegIds)
		{
			ArchRegId inputRegister;
			const ResourceBitset inputReadResource = ArchRegId::TryCreate(regId, inputRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(inputRegister))
				: ResourceMaskBuilder::ForRegisterRead(regId);
			setResourceMask(getResourceMask() | inputReadResource);
		}

		// Add register write resource
		if (getWritesRegister())
		{
			ArchRegId outputRegister;
			const ResourceBitset outputWriteResource = ArchRegId::TryCreate(outputRegId, outputRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(outputRegister))
				: ResourceMaskBuilder::ForRegisterWrite(outputRegId);
			setResourceMask(getResourceMask() | outputWriteResource);
		}

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	bool CustomAcceleratorMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		throw InstructionRegistry::CreateUnsupportedCustomAcceleratorException(getOpCode());
	}

	void CustomAcceleratorMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (getWritesRegister() && !_results.empty() && _results.size() > 0 && getDestRegID() != VLIW_Instruction::NoReg)
		{
			int vtId = NormalizeExecutionVtId(getOwnerThreadId());
			AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), _results[0]));
		}
	}

	bool CustomAcceleratorMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		if (getWritesRegister() && !_results.empty() && _results.size() > 0)
		{
			value = _results[0];
			return true;
		}

		value = 0;
		return false;
	}

	void CustomAcceleratorMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		if (_results.empty())
		{
			_results = {value};
			return;
		}

		_results[0] = value;
	}

	std::wstring CustomAcceleratorMicroOp::GetDescription()
	{
		return std::format(L"CustomAccel[{}]: OpCode=0x{:X}", getAccelerator()->getName(), getOpCode());
	}

	HaltMicroOp::HaltMicroOp()
	{
		setStealable(false);
		setControlFlow(true);
		setHasSideEffects(true);
		setClass(MicroOpClass::Other);
		setResourceMask(ResourceBitset::Zero);

		// ISA v4 Phase 02: halt is System class, FullSerial serialization
		setInstructionClass(Arch::InstructionClass::System);
		setSerializationClass(Arch::SerializationClass::FullSerial);

		// Phase 01: Typed-slot taxonomy — system singleton pinned to lane 7
		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);
		PublishExplicitStructuralSafetyMask();
	}

	CanonicalDecodePublicationMode HaltMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::ProjectorPublishes;
	}

	bool HaltMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		core->RedirectActiveExecutionForControlFlow(core->GetBoundMainMemoryLength());
		return true;
	}

	std::wstring HaltMicroOp::GetDescription()
	{
		return L"HALT (CPU_Stop)";
	}

	VLIW_Instruction MoveMicroOp::getInstruction() const
	{
		return Instruction;
	}

	void MoveMicroOp::setInstruction(VLIW_Instruction value)
	{
		Instruction = value;
	}

	MoveMicroOp::MoveMicroOp()
	{
		setClass(MicroOpClass::Alu);
		setResourceMask(ResourceBitset::Zero);

		// ISA v4 Phase 02: Move is ScalarAlu class, Free ordering
		setInstructionClass(Arch::InstructionClass::ScalarAlu);
		setSerializationClass(Arch::SerializationClass::Free);

		// Phase 01: Typed-slot taxonomy — static classification by predominant pattern (DT=0,1).
		// DT=2,3 runtime reclassification to LSU deferred to future phases.
		SetClassFlexiblePlacement(SlotClass::AluClass);
	}

	CanonicalDecodePublicationMode MoveMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void MoveMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
		ApplyCanonicalDirectFactoryProjection();
	}

	void MoveMicroOp::ApplyCanonicalRuntimeMoveShapeProjection(unsigned char dataType, unsigned short reg1Id, unsigned short reg2Id, unsigned long long primaryPayload)
	{
		ThrowIfUnsupportedRetainedRegisterContour(dataType);
		_projectedDataType = dataType;
		_projectedReg1Id = reg1Id;
		_projectedReg2Id = reg2Id;
		_projectedPrimaryPayload = primaryPayload;
		_hasProjectedMoveShape = true;
	}

	unsigned char MoveMicroOp::ResolveMoveDataType()
	{
		return _hasProjectedMoveShape ? _projectedDataType : getInstruction().getDataType();
	}

	unsigned short MoveMicroOp::ResolveMoveReg1Id()
	{
		return _hasProjectedMoveShape ? _projectedReg1Id : getInstruction().getReg1ID();
	}

	unsigned short MoveMicroOp::ResolveMoveReg2Id()
	{
		return _hasProjectedMoveShape ? _projectedReg2Id : getInstruction().getReg2ID();
	}

	unsigned long long MoveMicroOp::ResolveMovePrimaryPayload()
	{
		return _hasProjectedMoveShape ? _projectedPrimaryPayload : getInstruction().getSrc2Pointer();
	}

	void MoveMicroOp::InitializeMetadata()
	{
		constexpr unsigned short noReg = VLIW_Instruction::NoReg;
		unsigned char dataType = ResolveMoveDataType();
		unsigned short reg1Id = ResolveMoveReg1Id();
		unsigned short reg2Id = ResolveMoveReg2Id();

		std::vector<int> readRegs;
		std::vector<int> writeRegs;

		switch (dataType)
		{
			case 0:
				if (reg1Id != noReg)
				{
					readRegs.push_back(reg1Id);
				}
				if (reg2Id != noReg)
				{
					writeRegs.push_back(reg2Id);
				}
				break;
			case 1:
			case 3:
				if (reg1Id != noReg)
				{
					writeRegs.push_back(reg1Id);
				}
				break;
			case 2:
				if (reg1Id != noReg)
				{
					readRegs.push_back(reg1Id);
				}
				break;
			case 4:
			case 5:
				throw CreateUnsupportedRetainedRegisterContourException(dataType);
			default:
				throw InvalidOperationException(std::format(L"Move DT={} is unsupported on the runtime MoveMicroOp surface.", dataType));
		}

		setReadRegisters(readRegs);
		setWriteRegisters(writeRegs);

		setResourceMask(ResourceBitset::Zero);
		for (int i = 0; i < readRegs.size(); i++)
		{
			ArchRegId readRegister;
			const ResourceBitset readResource = ArchRegId::TryCreate(readRegs[i], readRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(readRegister))
				: ResourceMaskBuilder::ForRegisterRead(readRegs[i]);
			setResourceMask(getResourceMask() | readResource);
		}
		for (int i = 0; i < writeRegs.size(); i++)
		{
			ArchRegId writeRegister;
			const ResourceBitset writeResource = ArchRegId::TryCreate(writeRegs[i], writeRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(writeRegister))
				: ResourceMaskBuilder::ForRegisterWrite(writeRegs[i]);
			setResourceMask(getResourceMask() | writeResource);
		}

		if (dataType == 2)
		{
			setResourceMask(getResourceMask() | ResourceMaskBuilder::ForStore());
		}
		else if (dataType == 3)
		{
			setResourceMask(getResourceMask() | ResourceMaskBuilder::ForLoad());
		}
	}

	void MoveMicroOp::ApplyCanonicalDirectFactoryProjection()
	{
		unsigned char dataType = ResolveMoveDataType();
		bool isStoreShape = dataType == 2;
		bool isLoadShape = dataType == 3;
		bool isMemoryMoveFamilyOp = isMemoryOp() || isStoreShape || isLoadShape;
		bool writesRegister = getWriteRegisters().size() > 0;

		// Defensive classification only: if a manually projected MoveMicroOp still
		// carries a retired DT=2/3 memory shape, keep metadata aligned with the
		// memory contour until the fail-closed execute guard rejects it.
		setClass(isMemoryMoveFamilyOp ? MicroOpClass::Lsu : MicroOpClass::Alu);
		setHasSideEffects(isStoreShape);

		SlotPlacementMetadata placement{};
		placement.RequiredSlotClass = isMemoryMoveFamilyOp ? SlotClass::LsuClass : SlotClass::AluClass;
		placement.PinningKind = SlotPinningKind::ClassFlexible;
		placement.PinnedLaneId = 0;
		placement.DomainTag = getPlacement().DomainTag;
		const std::vector<int> readRegisters = getReadRegisters();
		const std::vector<int> writeRegisters = getWriteRegisters();
		ApplyCanonicalDecodeProjection(isMemoryMoveFamilyOp ? Arch::InstructionClass::Memory : Arch::InstructionClass::ScalarAlu,
			Arch::SerializationClass::Free, placement, isMemoryMoveFamilyOp, false, writesRegister, readRegisters, writeRegisters);
	}

	void MoveMicroOp::ResetStagedRegisterWrites()
	{
		_primaryWriteValue = 0;
	}

	std::shared_ptr<InvalidOperationException> MoveMicroOp::CreateRetiredMemoryContourException(unsigned char dataType, unsigned long long address)
	{
		return std::make_shared<InvalidOperationException>(std::format(L"Retired legacy Move DT={} memory contour reached MoveMicroOp.Execute() at 0x{:X}. ", dataType, address) + L"This path must fail closed and be canonicalized to Load/Store before runtime execution.");
	}

	std::shared_ptr<InvalidOperationException> MoveMicroOp::CreateUnsupportedRetainedRegisterContourException(unsigned char dataType)
	{
		switch (dataType)
		{
			case 4:
				return std::make_shared<InvalidOperationException>(L"Retained legacy Move DT=4 dual-write contour is unsupported and must fail closed before runtime materialization.");
			case 5:
				return std::make_shared<InvalidOperationException>(L"Retained legacy Move DT=5 triple-destination contour is unsupported and must fail closed before runtime materialization.");
			default:
				return std::make_shared<InvalidOperationException>(std::format(L"Move DT={} is unsupported on the runtime MoveMicroOp surface.", dataType));
		}
	}

	void MoveMicroOp::ThrowIfUnsupportedRetainedRegisterContour(unsigned char dataType)
	{
		if (dataType == 4 || dataType == 5)
		{
			throw CreateUnsupportedRetainedRegisterContourException(dataType);
		}
	}

	void MoveMicroOp::StagePrimaryRegisterWrite(unsigned short destRegId, unsigned long long value)
	{
		setDestRegID(destRegId);
		setWritesRegister(destRegId != VLIW_Instruction::NoReg);
		_primaryWriteValue = value;
	}

	bool MoveMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		unsigned char dt = ResolveMoveDataType();
		unsigned short reg1 = ResolveMoveReg1Id();
		unsigned short reg2 = ResolveMoveReg2Id();
		unsigned long long addr = ResolveMovePrimaryPayload(); // word2 = memory address or immediate
		int vtId = std::clamp(getOwnerThreadId(), 0, Processor::CPU_Core::SmtWays - 1);

		switch (dt)
		{
			case 0: // reg-to-reg Move
				if (reg1 != VLIW_Instruction::NoReg && reg2 != VLIW_Instruction::NoReg)
				{
					unsigned long long srcVal = core->ReadArch(vtId, reg1);
					StagePrimaryRegisterWrite(reg2, srcVal);
				}
				break;

			case 1: // Move_Num (immediate → reg)
				if (reg1 != VLIW_Instruction::NoReg)
				{
					StagePrimaryRegisterWrite(reg1, addr);
				}
				break;

			case 2: // Retired store contour must already be canonicalized to StoreMicroOp.
			case 3: // Retired load contour must already be canonicalized to LoadMicroOp.
				throw CreateRetiredMemoryContourException(dt, addr);

			case 4:
			case 5:
				throw CreateUnsupportedRetainedRegisterContourException(dt);
			default:
				throw InvalidOperationException(std::format(L"Move DT={} is unsupported on the runtime MoveMicroOp surface.", dt));
		}

		return true;
	}

	void MoveMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!getWritesRegister() || getDestRegID() == VLIW_Instruction::NoReg)
		{
			return;
		}

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		unsigned long long primaryValue = ResolveMoveDataType() == 3 ? _loadedValue : _primaryWriteValue;
		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), primaryValue));
	}

	bool MoveMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		if (!getWritesRegister() || getDestRegID() == VLIW_Instruction::NoReg)
		{
			value = 0;
			return false;
		}

		value = ResolveMoveDataType() == 3 ? _loadedValue : _primaryWriteValue;
		return true;
	}

	void MoveMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		if (ResolveMoveDataType() == 3)
		{
			_loadedValue = value;
		}
		else
		{
			_primaryWriteValue = value;
		}
	}

	std::wstring MoveMicroOp::GetDescription()
	{
		return std::format(L"Move: DT={}, Reg1={}", ResolveMoveDataType(), ResolveMoveReg1Id());
	}

	bool IncrDecrMicroOp::isDecrement() const
	{
		return IsDecrement;
	}

	void IncrDecrMicroOp::setDecrement(bool value)
	{
		IsDecrement = value;
	}

	IncrDecrMicroOp::IncrDecrMicroOp()
	{
		setWritesRegister(true);
		setClass(MicroOpClass::Alu);
		setResourceMask(ResourceBitset::Zero);

		// ISA v4 Phase 02: incr/decr is ScalarAlu class, Free ordering
		setInstructionClass(Arch::InstructionClass::ScalarAlu);
		setSerializationClass(Arch::SerializationClass::Free);

		// Phase 01: Typed-slot taxonomy
		SetClassFlexiblePlacement(SlotClass::AluClass);
	}

	CanonicalDecodePublicationMode IncrDecrMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::ProjectorPublishes;
	}

	bool IncrDecrMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (getDestRegID() != VLIW_Instruction::NoReg)
		{
			int vtId = std::clamp(getOwnerThreadId(), 0, Processor::CPU_Core::SmtWays - 1);
			unsigned long long val = core->ReadArch(vtId, getDestRegID());
			_result = isDecrement() ? val - 1 : val + 1;
		}
		return true;
	}

	void IncrDecrMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!getWritesRegister() || getDestRegID() == VLIW_Instruction::NoReg)
		{
			return;
		}

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), _result));
	}

	bool IncrDecrMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _result;
		return getWritesRegister() && getDestRegID() != VLIW_Instruction::NoReg;
	}

	void IncrDecrMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_result = value;
	}

	std::wstring IncrDecrMicroOp::GetDescription()
	{
		return std::format(L"{} R{}", (isDecrement() ? L"DEC" : L"INC"), getDestRegID());
	}
}
