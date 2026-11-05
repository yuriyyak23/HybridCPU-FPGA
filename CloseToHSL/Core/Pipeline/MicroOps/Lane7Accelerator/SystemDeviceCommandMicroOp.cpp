#include "SystemDeviceCommandMicroOp.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../../../Arch/CsrAddresses.h"
#include "../../../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../../../Architecture/State/Pipeline/PipelineState.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../../Execution/ExternalAccelerators/Auth/AcceleratorOwnerDomainGuard.h"
#include "../../../Execution/ExternalAccelerators/Tokens/AcceleratorRegisterAbi.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		const wchar_t* SystemDeviceCommandKindName(SystemDeviceCommandKind kind)
		{
			switch (kind)
			{
				case SystemDeviceCommandKind::QueryCaps: return L"QueryCaps";
				case SystemDeviceCommandKind::Submit: return L"Submit";
				case SystemDeviceCommandKind::Poll: return L"Poll";
				case SystemDeviceCommandKind::Status: return L"Status";
				case SystemDeviceCommandKind::Wait: return L"Wait";
				case SystemDeviceCommandKind::Cancel: return L"Cancel";
				case SystemDeviceCommandKind::Fence: return L"Fence";
				default: return L"Unknown";
			}
		}

		std::optional<std::shared_ptr<AcceleratorDescriptorReference>> MakeDescriptorReference(
			const std::optional<std::shared_ptr<AcceleratorCommandDescriptor>>& descriptor)
		{
			if (!descriptor || !*descriptor)
			{
				return std::nullopt;
			}
			return std::make_shared<AcceleratorDescriptorReference>((*descriptor)->DescriptorReference);
		}
	}

	SystemDeviceCommandMicroOp::SystemDeviceCommandMicroOp(unsigned short opCode, SystemDeviceCommandKind commandKind, Arch::SerializationClass serializationClass, unsigned short destinationRegister, unsigned short tokenRegister, std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> commandDescriptor)
		: CommandKind(commandKind), CommandDescriptor(commandDescriptor), CommandDescriptorReference(MakeDescriptorReference(commandDescriptor)),
		  DestinationRegister(NormalizeOptionalArchRegister(destinationRegister, std::format(L"{} rd", SystemDeviceCommandKindName(commandKind)))),
		  TokenRegister(NormalizeOptionalArchRegister(tokenRegister, std::format(L"{} rs1 token", SystemDeviceCommandKindName(commandKind))))
	{
		setOpCode(opCode);
		setDestRegID(getDestinationRegister());
		setStealable(false);
		setMemoryOp(false);
		setControlFlow(false);
		setHasSideEffects(true);
		setClass(MicroOpClass::Other);
		setWritesRegister(getDestinationRegister() != 0);

		setInstructionClass(Arch::InstructionClass::System);
		setSerializationClass(serializationClass);
		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);

		InitializeMetadata();
	}

	SystemDeviceCommandKind SystemDeviceCommandMicroOp::getCommandKind() const
	{
		return CommandKind;
	}

	std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> SystemDeviceCommandMicroOp::getCommandDescriptor() const
	{
		return CommandDescriptor;
	}

	std::optional<std::shared_ptr<AcceleratorDescriptorReference>> SystemDeviceCommandMicroOp::getCommandDescriptorReference() const
	{
		return CommandDescriptorReference;
	}

	unsigned short SystemDeviceCommandMicroOp::getDestinationRegister() const
	{
		return DestinationRegister;
	}

	unsigned short SystemDeviceCommandMicroOp::getTokenRegister() const
	{
		return TokenRegister;
	}

	std::optional<std::shared_ptr<ExternalAcceleratorRuntimeCommandResult>> SystemDeviceCommandMicroOp::getLastCommandResult() const
	{
		return _lastCommandResult;
	}

	std::optional<std::shared_ptr<AcceleratorTokenAdmissionResult>> SystemDeviceCommandMicroOp::getLastSubmitAdmission() const
	{
		if (!_lastCommandResult || !*_lastCommandResult || !(*_lastCommandResult)->SubmitAdmission)
		{
			return std::nullopt;
		}
		return (*_lastCommandResult)->SubmitAdmission;
	}

	std::optional<std::shared_ptr<AcceleratorTokenLookupResult>> SystemDeviceCommandMicroOp::getLastTokenLookup() const
	{
		if (!_lastCommandResult || !*_lastCommandResult || !(*_lastCommandResult)->TokenLookup)
		{
			return std::nullopt;
		}
		return (*_lastCommandResult)->TokenLookup;
	}

	bool SystemDeviceCommandMicroOp::getUsedLegacyCustomAcceleratorFallback() const
	{
		return false;
	}

	bool SystemDeviceCommandMicroOp::getUsedArithmeticExecutionPlane() const
	{
		return false;
	}

	CanonicalDecodePublicationMode SystemDeviceCommandMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void SystemDeviceCommandMicroOp::InitializeMetadata()
	{
		setReadRegisters(UsesTokenRegister(getCommandKind()) && getTokenRegister() != 0 ? std::vector<int>{static_cast<int>(getTokenRegister())} : std::vector<int>{});
		setWriteRegisters(getWritesRegister() ? std::vector<int>{static_cast<int>(getDestinationRegister())} : std::vector<int>{});
		setReadMemoryRanges(std::vector<MemoryRange>{});
		setWriteMemoryRanges(std::vector<MemoryRange>{});
		setResourceMask(BuildResourceMask(getPlacement().DomainTag, getReadRegisters(), getWriteRegisters()));

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata();
	}

	void SystemDeviceCommandMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	bool SystemDeviceCommandMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		InitializeMetadata();

		std::shared_ptr<ExternalAcceleratorRuntime> runtime = core->GetExternalAcceleratorRuntime();
		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		RejectGuestCompatibilityExecution(core, vtId);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> commandResult;
		switch (getCommandKind())
		{
			case SystemDeviceCommandKind::QueryCaps:
				commandResult = runtime->QueryCaps(BuildOwnerBinding(core, vtId), BuildGuardEvidence(core, vtId));
				break;
			case SystemDeviceCommandKind::Submit:
				commandResult = ExecuteSubmit(runtime);
				break;
			case SystemDeviceCommandKind::Poll:
				commandResult = runtime->Poll(ReadTokenHandle(core, vtId));
				break;
			case SystemDeviceCommandKind::Status:
				commandResult = runtime->Status(ReadTokenHandle(core, vtId));
				break;
			case SystemDeviceCommandKind::Wait:
				commandResult = runtime->Wait(ReadTokenHandle(core, vtId));
				break;
			case SystemDeviceCommandKind::Cancel:
				commandResult = runtime->Cancel(ReadTokenHandle(core, vtId));
				break;
			case SystemDeviceCommandKind::Fence:
				commandResult = ExecuteFenceObserve(runtime, ReadTokenHandle(core, vtId));
				break;
			default:
				throw InvalidOperationException(std::format(L"Unsupported L7-SDC command kind {}.", SystemDeviceCommandKindName(getCommandKind())));
		}

		_lastCommandResult = commandResult;
		if (commandResult && commandResult->RegisterAbi && commandResult->RegisterAbi->WritesRegister)
		{
			_capturedWriteBackValue = commandResult->RegisterAbi->RegisterValue;
		}

		return true;
	}

	void SystemDeviceCommandMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> result = ResolveRetireResult(core);
		std::shared_ptr<AcceleratorRegisterAbiResult> abi = result->RegisterAbi;
		if (!abi)
		{
			throw InvalidOperationException(L"L7-SDC retire result is missing its required register ABI payload.");
		}
		if (abi->RequiresPreciseFault)
		{
			throw InvalidOperationException(abi->Message);
		}

		if (!abi->WritesRegister || !getWritesRegister())
		{
			return;
		}

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestinationRegister(), abi->RegisterValue));
	}

	bool SystemDeviceCommandMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _capturedWriteBackValue;
		return getCommandKind() != SystemDeviceCommandKind::Fence && _lastCommandResult && *_lastCommandResult &&
			(*_lastCommandResult)->RegisterAbi && (*_lastCommandResult)->RegisterAbi->WritesRegister && getWritesRegister();
	}

	void SystemDeviceCommandMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_capturedWriteBackValue = value;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> SystemDeviceCommandMicroOp::ExecuteSubmit(std::shared_ptr<ExternalAcceleratorRuntime> runtime)
	{
		if (!CommandDescriptor || !*CommandDescriptor)
		{
			throw InvalidOperationException(L"ACCEL_SUBMIT execution requires a guard-backed AcceleratorCommandDescriptor sideband from native lane7 decode; descriptorless raw factory execution remains fail-closed.");
		}

		return runtime->Submit(*CommandDescriptor);
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> SystemDeviceCommandMicroOp::ExecuteFenceObserve(std::shared_ptr<ExternalAcceleratorRuntime> runtime, std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		_capturedFenceHandle = handle;
		return runtime->FenceObserve(handle);
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> SystemDeviceCommandMicroOp::ResolveRetireResult(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (getCommandKind() == SystemDeviceCommandKind::Fence)
		{
			_lastCommandResult = core->GetExternalAcceleratorRuntime()->FenceCommit(_capturedFenceHandle);
			if (_lastCommandResult && *_lastCommandResult && (*_lastCommandResult)->RegisterAbi && (*_lastCommandResult)->RegisterAbi->WritesRegister)
			{
				_capturedWriteBackValue = (*_lastCommandResult)->RegisterAbi->RegisterValue;
			}
		}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: return _lastCommandResult ?? throw new InvalidOperationException(string.Format("{0} retire requires a prior execute/capture result.", GetType().Name));
		if (!_lastCommandResult || !*_lastCommandResult)
		{
			throw InvalidOperationException(std::format(L"{} retire requires a prior execute/capture result.", SystemDeviceCommandKindName(getCommandKind())));
		}
		return *_lastCommandResult;
	}

	void SystemDeviceCommandMicroOp::RejectGuestCompatibilityExecution(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		if (core->getCsr()->Read(CsrAddresses::VmxEnable, PrivilegeLevel::Machine) == 0)
		{
			return;
		}

		bool compatibilityGuestExecution;
		try
		{
			compatibilityGuestExecution = core->ReadVirtualThreadPipelineState(vtId) == PipelineState::GuestExecution;
		}
		catch (const ArgumentOutOfRangeException &e1)
		{
			compatibilityGuestExecution = core->HasAnyVirtualThreadPipelineState(PipelineState::GuestExecution);
		}

		if (compatibilityGuestExecution)
		{
			throw InvalidOperationException(L"Guest Lane7 compatibility execution is fail-closed: no runtime-owned Lane7 domain binding is admitted for the frozen VMX frontend.");
		}
	}

	unsigned long long SystemDeviceCommandMicroOp::ReadTokenRegisterValue(std::shared_ptr<Processor::CPU_Core> &core, int ownerVirtualThreadId)
	{
		if (getTokenRegister() == 0)
		{
			return 0;
		}

		return ReadUnifiedScalarSourceOperand(core, ownerVirtualThreadId, getTokenRegister());
	}

	std::shared_ptr<AcceleratorTokenHandle> SystemDeviceCommandMicroOp::ReadTokenHandle(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		unsigned long long rawHandle = ReadUnifiedScalarSourceOperand(core, vtId, getTokenRegister());
		return rawHandle == 0 ? AcceleratorTokenHandle::Invalid : AcceleratorTokenHandle::FromOpaqueValue(rawHandle);
	}

	std::shared_ptr<AcceleratorOwnerBinding> SystemDeviceCommandMicroOp::BuildOwnerBinding(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		if (getOwnerContextId() < 0)
		{
		throw InvalidOperationException(std::format(L"{} owner context id is not materialized.", SystemDeviceCommandKindName(getCommandKind())));
		}

		std::shared_ptr<AcceleratorOwnerBinding> temp_var = std::make_shared<AcceleratorOwnerBinding>();
		temp_var->OwnerVirtualThreadId = static_cast<unsigned short>(vtId);
		temp_var->OwnerContextId = static_cast<unsigned int>(getOwnerContextId());
		temp_var->OwnerCoreId = core->getCoreID();
		temp_var->OwnerPodId = 0;
		temp_var->DomainTag = getPlacement().DomainTag;
		return temp_var;
	}

	std::shared_ptr<AcceleratorGuardEvidence> SystemDeviceCommandMicroOp::BuildGuardEvidence(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		std::shared_ptr<AcceleratorOwnerBinding> ownerBinding = BuildOwnerBinding(core, vtId);
		return AcceleratorGuardEvidence::FromGuardPlane(ownerBinding, ownerBinding->DomainTag);
	}

	std::wstring SystemDeviceCommandMicroOp::GetDescription()
	{
		return std::format(L"L7-SDC {}: OpCode={}, rd=x{}, rs1=x{}", SystemDeviceCommandKindName(getCommandKind()), OpcodeRegistry::GetMnemonicOrHex(getOpCode()), getDestinationRegister(), getTokenRegister());
	}

	bool SystemDeviceCommandMicroOp::UsesTokenRegister(SystemDeviceCommandKind commandKind)
	{
		return (commandKind == SystemDeviceCommandKind::Poll || commandKind == SystemDeviceCommandKind::Status || commandKind == SystemDeviceCommandKind::Wait || commandKind == SystemDeviceCommandKind::Cancel || commandKind == SystemDeviceCommandKind::Fence);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ResourceBitset BuildResourceMask(ulong ownerDomainTag, System.Collections.Generic.IReadOnlyList<int> readRegisters, System.Collections.Generic.IReadOnlyList<int> writeRegisters)
	ResourceBitset SystemDeviceCommandMicroOp::BuildResourceMask(unsigned long long ownerDomainTag, const std::vector<int> &readRegisters, const std::vector<int> &writeRegisters)
	{
		int resourceDomainBucket = static_cast<int>(ownerDomainTag & 0xFULL);
		ResourceBitset mask = ResourceMaskBuilder::ForAccelerator(0) | ResourceMaskBuilder::ForMemoryDomain(resourceDomainBucket);

		for (int index = 0; index < readRegisters.size(); index++)
		{
			int registerId = readRegisters[index];
			ArchRegId architecturalRegisterId;
			mask = mask | (ArchRegId::TryCreate(registerId, architecturalRegisterId)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(architecturalRegisterId))
				: ResourceMaskBuilder::ForRegisterRead(registerId));
		}

		for (int index = 0; index < writeRegisters.size(); index++)
		{
			int registerId = writeRegisters[index];
			ArchRegId architecturalRegisterId;
			mask = mask | (ArchRegId::TryCreate(registerId, architecturalRegisterId)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(architecturalRegisterId))
				: ResourceMaskBuilder::ForRegisterWrite(registerId));
		}

		return mask;
	}

	unsigned short SystemDeviceCommandMicroOp::NormalizeOptionalArchRegister(unsigned short rawRegister, const std::wstring &operandName)
	{
		if (rawRegister == VLIW_Instruction::NoReg || rawRegister == VLIW_Instruction::NoArchReg)
		{
			return 0;
		}

		int archRegId;
		if (!TryNormalizeFlatArchRegId(rawRegister, archRegId))
		{
			throw DecodeProjectionFaultException(std::format(L"L7-SDC {} requires a flat architectural register id.", operandName));
		}

		return static_cast<unsigned short>(archRegId);
	}

	AcceleratorQueryCapsMicroOp::AcceleratorQueryCapsMicroOp(unsigned short destinationRegister) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_QUERY_CAPS, SystemDeviceCommandKind::QueryCaps, Arch::SerializationClass::CsrOrdered, destinationRegister)
	{
	}

	std::optional<std::shared_ptr<AcceleratorGuardDecision>> AcceleratorSubmitMicroOp::getSubmitGuardDecision() const
	{
		return SubmitGuardDecision;
	}

	AcceleratorSubmitMicroOp::AcceleratorSubmitMicroOp(unsigned short destinationRegister) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT, SystemDeviceCommandKind::Submit, Arch::SerializationClass::MemoryOrdered, destinationRegister)
	{
	}

	AcceleratorSubmitMicroOp::AcceleratorSubmitMicroOp(unsigned short destinationRegister, std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT, SystemDeviceCommandKind::Submit, Arch::SerializationClass::MemoryOrdered, destinationRegister, VLIW_Instruction::NoReg, commandDescriptor)
	{
		ArgumentNullException::ThrowIfNull(commandDescriptor);
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> guardEvidence = commandDescriptor->OwnerGuardDecision
			? std::optional<std::shared_ptr<AcceleratorGuardEvidence>>(commandDescriptor->OwnerGuardDecision->Evidence)
			: std::nullopt;
		std::shared_ptr<AcceleratorGuardDecision> submitGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeSubmit(commandDescriptor, guardEvidence);
		if (!submitGuardDecision->IsAllowed)
		{
			throw InvalidOperationException(L"ACCEL_SUBMIT admission requires guard-backed owner/domain evidence. " + submitGuardDecision->Message);
		}

		SubmitGuardDecision = submitGuardDecision;
	}

	AcceleratorSubmitMicroOp::AcceleratorSubmitMicroOp(std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor) : AcceleratorSubmitMicroOp(VLIW_Instruction::NoReg, commandDescriptor)
	{
	}

	AcceleratorPollMicroOp::AcceleratorPollMicroOp(unsigned short destinationRegister, unsigned short tokenRegister) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_POLL, SystemDeviceCommandKind::Poll, Arch::SerializationClass::CsrOrdered, destinationRegister, tokenRegister)
	{
	}

	AcceleratorWaitMicroOp::AcceleratorWaitMicroOp(unsigned short destinationRegister, unsigned short tokenRegister) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_WAIT, SystemDeviceCommandKind::Wait, Arch::SerializationClass::FullSerial, destinationRegister, tokenRegister)
	{
	}

	AcceleratorStatusMicroOp::AcceleratorStatusMicroOp(unsigned short destinationRegister, unsigned short tokenRegister) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_STATUS, SystemDeviceCommandKind::Status, Arch::SerializationClass::CsrOrdered, destinationRegister, tokenRegister)
	{
	}

	AcceleratorCancelMicroOp::AcceleratorCancelMicroOp(unsigned short destinationRegister, unsigned short tokenRegister) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_CANCEL, SystemDeviceCommandKind::Cancel, Arch::SerializationClass::FullSerial, destinationRegister, tokenRegister)
	{
	}

	AcceleratorFenceMicroOp::AcceleratorFenceMicroOp(unsigned short destinationRegister, unsigned short tokenRegister) : SystemDeviceCommandMicroOp(Processor::CPU_Core::IsaOpcodeValues::ACCEL_FENCE, SystemDeviceCommandKind::Fence, Arch::SerializationClass::FullSerial, destinationRegister, tokenRegister)
	{
	}
}

