#include "MicroOp.IO.h"
#include "../../Safety/ResourceBitset.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../Certificates/SafetyMask128.h"
#include "../../../Runtime/Completion/Commit/NeutralArchitecturalCompletionFacts.h"
#include "../../../Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		ResourceBitset VmxRegisterReadResource(int registerId)
		{
			ArchRegId architecturalRegister;
			return ArchRegId::TryCreate(registerId, architecturalRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(architecturalRegister))
				: ResourceMaskBuilder::ForRegisterRead(registerId);
		}

		ResourceBitset VmxRegisterWriteResource(int registerId)
		{
			ArchRegId architecturalRegister;
			return ArchRegId::TryCreate(registerId, architecturalRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(architecturalRegister))
				: ResourceMaskBuilder::ForRegisterWrite(registerId);
		}
	}

	unsigned int TrapMicroOp::getUndecodedOpCode() const
	{
		return UndecodedOpCode;
	}

	void TrapMicroOp::setUndecodedOpCode(unsigned int value)
	{
		UndecodedOpCode = value;
	}

	std::wstring TrapMicroOp::getTrapReason() const
	{
		return TrapReason;
	}

	void TrapMicroOp::setTrapReason(const std::wstring &value)
	{
		TrapReason = value;
	}

	int TrapMicroOp::getProjectedMemoryBankIntent() const
	{
		return ProjectedMemoryBankIntent;
	}

	void TrapMicroOp::setProjectedMemoryBankIntent(int value)
	{
		ProjectedMemoryBankIntent = value;
	}

	TrapMicroOp::TrapMicroOp()
	{
		// Traps redirect control flow to the exception/trap-handler vector.
		setStealable(false);
		setControlFlow(true);

		setReadRegisters({});
		setWriteRegisters({});
		setReadMemoryRanges({});
		setWriteMemoryRanges({});

		// Trap consumes no compute resources but occupies the system-singleton lane.
		setResourceMask(ResourceBitset::Zero);

		setInstructionClass(Arch::InstructionClass::System);
		setSerializationClass(Arch::SerializationClass::FullSerial);

		// Trap is fully serialised — must be routed to the system-singleton slot.
		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);

		// Conflicts with everything: traps must not be reordered or stolen.
		setSafetyMask(SafetyMask128::All);
	}

	CanonicalDecodePublicationMode TrapMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::ProjectorPublishes;
	}

	bool TrapMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Trap is recorded; the pipeline FSM handles redirection to the handler vector.
		return true;
	}

	std::shared_ptr<Pipeline::TrapEntryEvent> TrapMicroOp::CreatePipelineEvent(unsigned long long bundleSerial)
	{
		unsigned char vtId = static_cast<unsigned char>(getVirtualThreadId() & 0xFF);
		std::shared_ptr<Pipeline::TrapEntryEvent> temp_var = std::make_shared<Pipeline::TrapEntryEvent>();
		temp_var->VtId = vtId;
		temp_var->BundleSerial = bundleSerial;
		temp_var->CauseCode = IllegalInstructionCauseCode;
		temp_var->FaultAddress = 0;
		temp_var->HasFaultAddress = false;
		temp_var->FaultAddressSemantic = NeutralFaultAddressSemantic::None;
		return temp_var;
	}

	std::wstring TrapMicroOp::GetDescription()
	{
		return getTrapReason() != L""
			? std::format(L"TRAP: UndecodedOpCode=0x{:X} ({})", getUndecodedOpCode(), getTrapReason())
			: std::format(L"TRAP: UndecodedOpCode=0x{:X}", getUndecodedOpCode());
	}

	std::shared_ptr<VirtualizationAdmissionCertificate> VmxMicroOp::getVirtualizationAdmission() const
	{
		return VirtualizationAdmission;
	}

	void VmxMicroOp::setVirtualizationAdmission(std::shared_ptr<VirtualizationAdmissionCertificate> value)
	{
		VirtualizationAdmission = value;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::VirtualizationOperandSnapshot> VmxMicroOp::getVirtualizationOperandSnapshot() const
	{
		return VirtualizationOperandSnapshot;
	}

	void VmxMicroOp::setVirtualizationOperandSnapshot(std::shared_ptr<YAKSys_Hybrid_CPU::Core::VirtualizationOperandSnapshot> value)
	{
		VirtualizationOperandSnapshot = value;
	}

	std::optional<std::shared_ptr<DomainHypercallExecutionResult>> VmxMicroOp::getExactHypercallExecutionResult() const
	{
		return ExactHypercallExecutionResult;
	}

	void VmxMicroOp::setExactHypercallExecutionResult(const std::optional<std::shared_ptr<DomainHypercallExecutionResult>> &value)
	{
		ExactHypercallExecutionResult = value;
	}

	std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> VmxMicroOp::getExactHypercallExecutionReceipt() const
	{
		const auto result = getExactHypercallExecutionResult();
		return result && *result ? (*result)->Receipt : nullptr;
	}

	std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> VmxMicroOp::getExactHypercallCompletionPublication() const
	{
		return ExactHypercallCompletionPublication;
	}

	void VmxMicroOp::setExactHypercallCompletionPublication(const std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> &value)
	{
		ExactHypercallCompletionPublication = value;
	}

	std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> VmxMicroOp::getExactHypercallRetireGrant() const
	{
		return ExactHypercallRetireGrant;
	}

	void VmxMicroOp::setExactHypercallRetireGrant(std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> value)
	{
		ExactHypercallRetireGrant = value;
	}

	unsigned long long VmxMicroOp::getExactHypercallRetireWindowIdentity() const
	{
		return ExactHypercallRetireWindowIdentity;
	}

	void VmxMicroOp::setExactHypercallRetireWindowIdentity(unsigned long long value)
	{
		ExactHypercallRetireWindowIdentity = value;
	}

	unsigned long long VmxMicroOp::getExactHypercallRetireOrderEpoch() const
	{
		return ExactHypercallRetireOrderEpoch;
	}

	void VmxMicroOp::setExactHypercallRetireOrderEpoch(unsigned long long value)
	{
		ExactHypercallRetireOrderEpoch = value;
	}

	bool VmxMicroOp::getHasVmReadScalarResultReceipt() const
	{
		return _vmReadScalarResultReceipt != nullptr;
	}

	std::optional<std::shared_ptr<Pipeline::MicroOps::InstructionIR>> VmxMicroOp::getInstruction() const
	{
		return Instruction;
	}

	void VmxMicroOp::setInstruction(const std::optional<std::shared_ptr<Pipeline::MicroOps::InstructionIR>> &value)
	{
		Instruction = value;
	}

	unsigned char VmxMicroOp::getRd() const
	{
		return Rd;
	}

	void VmxMicroOp::setRd(unsigned char value)
	{
		Rd = value;
	}

	unsigned char VmxMicroOp::getRs1() const
	{
		return Rs1;
	}

	void VmxMicroOp::setRs1(unsigned char value)
	{
		Rs1 = value;
	}

	unsigned char VmxMicroOp::getRs2() const
	{
		return Rs2;
	}

	void VmxMicroOp::setRs2(unsigned char value)
	{
		Rs2 = value;
	}

	VmxMicroOp::VmxMicroOp()
	{
		setClass(MicroOpClass::Other);
		setResourceMask(ResourceBitset::Zero);

		setInstructionClass(Arch::InstructionClass::Vmx);
		setSerializationClass(Arch::SerializationClass::VmxSerial);

		// VMX instructions are pinned to the system-singleton slot (lane 7)
		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);
	}

	CanonicalDecodePublicationMode VmxMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void VmxMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	bool VmxMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		const auto publishedInstruction = getInstruction();
		std::shared_ptr<InstructionIR> instr = publishedInstruction && *publishedInstruction
			? *publishedInstruction
			: BuildInstructionIR();
		VmxOperationKind operation;
		if (!InstructionRegistry::TryResolvePublishedVmxOperationKind(instr, operation))
		{
			throw InvalidOperationException(std::format(
				L"Unknown frozen VMX opcode {}.",
				Arch::OpcodeRegistry::GetMnemonicOrHex(instr->CanonicalOpcode->Value)));
		}

		if (operation == VmxOperationKind::VmRead && _vmReadScalarResultReceipt != nullptr && _vmReadScalarResultReceipt->TryValidateSpeculative(core->getCurrentVirtualizationRestoreGeneration()))
		{
			_vmReadScalarResult = _vmReadScalarResultReceipt->getValue();
			_vmReadScalarResultReady = true;
			_resolvedRetireEffect.emplace(VmxRetireEffect{});
			return true;
		}

		const auto exactHypercallExecutionResult = getExactHypercallExecutionResult();
		if (operation == VmxOperationKind::VmCall && _exactHypercallDispatch != nullptr &&
			(!exactHypercallExecutionResult || !*exactHypercallExecutionResult))
		{
			std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> publication;
			setExactHypercallExecutionResult(_exactHypercallDispatch->Execute(shared_from_this(), publication));
			setExactHypercallCompletionPublication(publication);
		}

		_resolvedRetireEffect.emplace(VmxRetireEffect::Fault(operation, VmExitReason::SecurityPolicyViolation));
		return true;
	}

	std::wstring VmxMicroOp::GetDescription()
	{
		return std::format(L"VMX: OpCode=0x{:X} ({})", getOpCode(), Arch::OpcodeRegistry::GetMnemonicOrHex(getOpCode()));
	}

	VmxRetireEffect VmxMicroOp::CreateRetireEffect()
	{
		return _resolvedRetireEffect.value_or(VmxRetireEffect{});
	}

	bool VmxMicroOp::TryResolveFrozenOperation(VmxOperationKind &operation)
	{
		const auto publishedInstruction = getInstruction();
		std::shared_ptr<InstructionIR> instruction = publishedInstruction && *publishedInstruction
			? *publishedInstruction
			: BuildInstructionIR();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (instruction.CanonicalOpcode.Value != unchecked((ushort)OpCode))
		if (instruction->CanonicalOpcode->Value != static_cast<unsigned short>(getOpCode()))
		{
			operation = static_cast<YAKSys_Hybrid_CPU::Core::VmxOperationKind>(0);
			return false;
		}

		return InstructionRegistry::TryResolvePublishedVmxOperationKind(instruction, operation);
	}

	void VmxMicroOp::AttachVirtualizationAdmission(std::shared_ptr<VirtualizationAdmissionCertificate> certificate)
	{
		ArgumentNullException::ThrowIfNull(certificate);
		if (getVirtualizationAdmission() != nullptr)
		{
			throw InvalidOperationException(L"A VmxMicroOp cannot receive a second virtualization admission certificate.");
		}

		setVirtualizationAdmission(certificate);
	}

	void VmxMicroOp::AttachVirtualizationOperandSnapshot(std::shared_ptr<YAKSys_Hybrid_CPU::Core::VirtualizationOperandSnapshot> snapshot)
	{
		ArgumentNullException::ThrowIfNull(snapshot);
		if (getVirtualizationAdmission() == nullptr)
		{
			throw InvalidOperationException(L"A VmxMicroOp cannot receive operands before E1.");
		}
		if (getVirtualizationOperandSnapshot() != nullptr)
		{
			throw InvalidOperationException(L"A VmxMicroOp cannot receive a second operand snapshot.");
		}

		setVirtualizationOperandSnapshot(snapshot);
	}

	void VmxMicroOp::AttachExactHypercallExecutionDispatch(std::shared_ptr<DomainHypercallCanonicalComposition::ExecutionDispatch> dispatch)
	{
		ArgumentNullException::ThrowIfNull(dispatch);
		if (getVirtualizationAdmission() == nullptr || getVirtualizationOperandSnapshot() == nullptr)
		{
			throw InvalidOperationException(L"Exact hypercall dispatch requires canonical E1 and operands.");
		}
		if (_exactHypercallDispatch != nullptr)
		{
			throw InvalidOperationException(L"A VmxMicroOp cannot receive a second exact hypercall dispatch.");
		}

		_exactHypercallDispatch = dispatch;
	}

	void VmxMicroOp::AttachExactHypercallRetireGrant(std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> grant, unsigned long long retireWindowIdentity, unsigned long long orderEpoch)
	{
		ArgumentNullException::ThrowIfNull(grant);
		if (getExactHypercallRetireGrant() != nullptr)
		{
			throw InvalidOperationException(L"A VmxMicroOp cannot receive a second E6 retire grant.");
		}
		setExactHypercallRetireGrant(grant);
		setExactHypercallRetireWindowIdentity(retireWindowIdentity);
		setExactHypercallRetireOrderEpoch(orderEpoch);
	}

	void VmxMicroOp::AttachVmReadScalarResultReceipt(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long fieldSelector)
	{
		ArgumentNullException::ThrowIfNull(receipt);
		if (_vmReadScalarResultReceipt != nullptr)
		{
			throw InvalidOperationException(L"A VMREAD carrier cannot receive a second scalar-result receipt.");
		}
		if (!receipt->MatchesCarrier(shared_from_this(), fieldSelector))
		{
			throw InvalidOperationException(L"VMREAD scalar-result receipt mismatched the canonical attempt, VT, field, domain, or destination carrier.");
		}
		_vmReadScalarResultReceipt = receipt;
	}

	bool VmxMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _vmReadScalarResult;
		return _vmReadScalarResultReady && _vmReadScalarResultReceipt != nullptr;
	}

	void VmxMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		if (_vmReadScalarResultReady)
		{
			_vmReadScalarResult = value;
		}
	}

	void VmxMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!_vmReadScalarResultReady || _vmReadScalarResultReceipt == nullptr)
		{
			return;
		}
		if (_vmReadScalarResult != _vmReadScalarResultReceipt->getValue())
		{
			throw InvalidOperationException(L"VMREAD scalar writeback value no longer matches its attempt-bound projection receipt.");
		}
		if (!_vmReadScalarResultReceipt->TryConsumeAtRetire(core->getCurrentVirtualizationRestoreGeneration()))
		{
			throw InvalidOperationException(L"VMREAD scalar-result receipt is stale, replayed, revoked, restored, or already consumed.");
		}

		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(NormalizeExecutionVtId(getOwnerThreadId()), _vmReadScalarResultReceipt->getDestinationRegister(), _vmReadScalarResult));
	}

	bool VmxMicroOp::HasArchitecturalRegister(unsigned char registerId)
	{
		return registerId != 0 && registerId != VLIW_Instruction::NoArchReg;
	}

	unsigned char VmxMicroOp::NormalizeInstructionIrRegister(unsigned char registerId)
	{
		return registerId == VLIW_Instruction::NoArchReg ? static_cast<unsigned char>(0) : registerId;
	}

	void VmxMicroOp::InitializeMetadata()
	{
		std::vector<int> readRegs;
		setResourceMask(ResourceBitset::Zero);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ushort opcode = unchecked((ushort)OpCode);
		unsigned short opcode = static_cast<unsigned short>(getOpCode());
		setWritesRegister(((opcode == Processor::CPU_Core::IsaOpcodeValues::VMREAD || opcode == Processor::CPU_Core::IsaOpcodeValues::VMPTRST || opcode == Processor::CPU_Core::IsaOpcodeValues::VMFUNC)) && HasArchitecturalRegister(getRd()));

		switch (opcode)
		{
			case Processor::CPU_Core::IsaOpcodeValues::VMREAD:
				if (HasArchitecturalRegister(getRs1()))
				{
					readRegs.push_back(getRs1());
					setResourceMask(getResourceMask() | VmxRegisterReadResource(getRs1()));
				}
				break;

			case Processor::CPU_Core::IsaOpcodeValues::VMWRITE:
				if (HasArchitecturalRegister(getRs1()))
				{
					readRegs.push_back(getRs1());
					setResourceMask(getResourceMask() | VmxRegisterReadResource(getRs1()));
				}

				if (HasArchitecturalRegister(getRs2()))
				{
					readRegs.push_back(getRs2());
					setResourceMask(getResourceMask() | VmxRegisterReadResource(getRs2()));
				}
				break;

			case Processor::CPU_Core::IsaOpcodeValues::VMCLEAR:
			case Processor::CPU_Core::IsaOpcodeValues::VMPTRLD:
				if (HasArchitecturalRegister(getRs1()))
				{
					readRegs.push_back(getRs1());
					setResourceMask(getResourceMask() | VmxRegisterReadResource(getRs1()));
				}
				break;

			case Processor::CPU_Core::IsaOpcodeValues::VMCALL:
			case Processor::CPU_Core::IsaOpcodeValues::INVEPT:
			case Processor::CPU_Core::IsaOpcodeValues::INVVPID:
			case Processor::CPU_Core::IsaOpcodeValues::VMFUNC:
			case Processor::CPU_Core::IsaOpcodeValues::VMSAVEX:
			case Processor::CPU_Core::IsaOpcodeValues::VMRESTX:
				if (HasArchitecturalRegister(getRs1()))
				{
					readRegs.push_back(getRs1());
					setResourceMask(getResourceMask() | VmxRegisterReadResource(getRs1()));
				}

				if (HasArchitecturalRegister(getRs2()))
				{
					readRegs.push_back(getRs2());
					setResourceMask(getResourceMask() | VmxRegisterReadResource(getRs2()));
				}
				break;
		}

		setReadRegisters(readRegs);
		setWriteRegisters(getWritesRegister()
			? std::vector<int>{static_cast<int>(getRd())}
			: std::vector<int>{});

		if (getWritesRegister())
		{
			setResourceMask(getResourceMask() | VmxRegisterWriteResource(getRd()));
		}

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	std::shared_ptr<Pipeline::MicroOps::InstructionIR> VmxMicroOp::BuildInstructionIR()
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ushort opcode = unchecked((ushort)OpCode);
		unsigned short opcode = static_cast<unsigned short>(getOpCode());
		std::shared_ptr<Pipeline::MicroOps::InstructionIR> temp_var = std::make_shared<Pipeline::MicroOps::InstructionIR>();
		temp_var->CanonicalOpcode = std::make_shared<Processor::CPU_Core::IsaOpcode>(opcode);
		temp_var->Class = Arch::InstructionClass::Vmx;
		temp_var->SerializationClass = Arch::SerializationClass::VmxSerial;
		temp_var->Rd = NormalizeInstructionIrRegister(getRd());
		temp_var->Rs1 = NormalizeInstructionIrRegister(getRs1());
		temp_var->Rs2 = NormalizeInstructionIrRegister(getRs2());
		temp_var->Imm = 0;
		temp_var->VmxPayload = VmxInstructionPayload::FromDecodedRegisters(opcode, NormalizeInstructionIrRegister(getRd()), NormalizeInstructionIrRegister(getRs1()), NormalizeInstructionIrRegister(getRs2()));
		return temp_var;
	}
}
