#include "VmxSpecTable.h"

namespace YAKSys_Hybrid_CPU::Arch
{

	const std::vector<std::shared_ptr<VmxOpcodeSpec>> VmxSpecTable::Vmx8Opcodes = { std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMXON, L"VMXON", 1, InstructionFlags::Privileged, VmxOperandForm::ReservedIgnoredRegister, L"VmxOn", L"VmxOn", VmxRegisterRoleMask::None, L"VMX8 preserves one encoded operand as reserved/ignored; VMX-v2 may reinterpret rs1 only behind a future capability."), std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMXOFF, L"VMXOFF", 0, InstructionFlags::Privileged, VmxOperandForm::None, L"VmxOff", L"VmxOff", VmxRegisterRoleMask::None, L"VMX8 control transition; guest-context VMXOFF retires as a VM-exit through runtime state."), std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMLAUNCH, L"VMLAUNCH", 0, InstructionFlags::Privileged, VmxOperandForm::None, L"VmLaunch", L"VmLaunch", VmxRegisterRoleMask::None, L"VM-entry legality remains runtime-owned and active-VMCS dependent."), std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMRESUME, L"VMRESUME", 0, InstructionFlags::Privileged, VmxOperandForm::None, L"VmResume", L"VmResume", VmxRegisterRoleMask::None, L"VM-entry resume legality remains runtime-owned and launched-VMCS dependent."), std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMREAD, L"VMREAD", 2, InstructionFlags::Privileged, VmxOperandForm::FieldSelectorToRegister, L"VmRead", L"VmRead", VmxRegisterRoleMask::ReadsRs1 | VmxRegisterRoleMask::WritesRd, L"Rs1 is the VMCS field selector register; VMX8 does not publish an immediate selector ABI."), std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMWRITE, L"VMWRITE", 2, InstructionFlags::Privileged, VmxOperandForm::FieldSelectorAndValueRegisters, L"VmWrite", L"VmWrite", VmxRegisterRoleMask::ReadsRs1 | VmxRegisterRoleMask::ReadsRs2, L"Rs1 is the VMCS field selector register and Rs2 is the value register."), std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMCLEAR, L"VMCLEAR", 1, InstructionFlags::Privileged, VmxOperandForm::VmcsPointerRegister, L"VmClear", L"VmClear", VmxRegisterRoleMask::ReadsRs1, L"Rs1 carries a VMCS descriptor pointer key; VMX8 does not publish scalar memory side effects."), std::make_shared<VmxOpcodeSpec>(Processor::CPU_Core::IsaOpcodeValues::VMPTRLD, L"VMPTRLD", 1, InstructionFlags::Privileged, VmxOperandForm::VmcsPointerRegister, L"VmPtrLd", L"VmPtrLd", VmxRegisterRoleMask::ReadsRs1, L"Rs1 carries a VMCS descriptor pointer key; descriptor materialization is runtime-owned.") };

	const std::vector<std::shared_ptr<VmcsFieldSpec>> VmxSpecTable::Vmx8VmcsFields = { std::make_shared<VmcsFieldSpec>(L"GuestPc", 0, VmcsFieldGroup::GuestState, VmcsVisibilityPolicy::GuestArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"GuestSp", 1, VmcsFieldGroup::GuestState, VmcsVisibilityPolicy::GuestArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"GuestFlags", 2, VmcsFieldGroup::GuestState, VmcsVisibilityPolicy::GuestArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"GuestCr0", 3, VmcsFieldGroup::GuestState, VmcsVisibilityPolicy::GuestArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"GuestCr3", 4, VmcsFieldGroup::GuestState, VmcsVisibilityPolicy::GuestArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"GuestCr4", 5, VmcsFieldGroup::GuestState, VmcsVisibilityPolicy::GuestArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"HostPc", 32, VmcsFieldGroup::HostState, VmcsVisibilityPolicy::HostArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"HostSp", 33, VmcsFieldGroup::HostState, VmcsVisibilityPolicy::HostArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"HostFlags", 34, VmcsFieldGroup::HostState, VmcsVisibilityPolicy::HostArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"HostCr0", 35, VmcsFieldGroup::HostState, VmcsVisibilityPolicy::HostArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"HostCr3", 36, VmcsFieldGroup::HostState, VmcsVisibilityPolicy::HostArchitecturalState, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"PinBasedControls", 64, VmcsFieldGroup::Controls, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"ProcBasedControls", 65, VmcsFieldGroup::Controls, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"ExitControls", 66, VmcsFieldGroup::Controls, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"EntryControls", 67, VmcsFieldGroup::Controls, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"EptPointer", 80, VmcsFieldGroup::NestedTranslation, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias; not a complete NPT contract in Phase 00."), std::make_shared<VmcsFieldSpec>(L"Vpid", 81, VmcsFieldGroup::NestedTranslation, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias; not a complete VPID contract in Phase 00."), std::make_shared<VmcsFieldSpec>(L"SecondaryProcControls", 82, VmcsFieldGroup::NestedTranslation, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"Cr3TargetCount", 83, VmcsFieldGroup::NestedTranslation, VmcsVisibilityPolicy::RootOwnedControl, L"VMX8 compatibility alias."), std::make_shared<VmcsFieldSpec>(L"ExitReason", 96, VmcsFieldGroup::ExitInfo, VmcsVisibilityPolicy::GuestVisibleByPolicy, L"VMX8 compatibility alias for legacy exit projection."), std::make_shared<VmcsFieldSpec>(L"ExitQualification", 97, VmcsFieldGroup::ExitInfo, VmcsVisibilityPolicy::GuestVisibleByPolicy, L"VMX8 compatibility alias for legacy exit projection."), std::make_shared<VmcsFieldSpec>(L"GuestPhysicalAddress", 112, VmcsFieldGroup::NestedTranslationExitInfo, VmcsVisibilityPolicy::GuestVisibleByPolicy, L"VMX8 compatibility alias; Phase 03 wires typed NPT qualification."), std::make_shared<VmcsFieldSpec>(L"EptViolationQualification", 113, VmcsFieldGroup::NestedTranslationExitInfo, VmcsVisibilityPolicy::GuestVisibleByPolicy, L"VMX8 compatibility alias; Phase 03 wires typed NPT qualification.") };

	const std::vector<std::shared_ptr<VmxCsrSpec>> VmxSpecTable::Vmx8Csrs = { std::make_shared<VmxCsrSpec>(L"VmxEnable", 0x820, VmxCsrAccessPolicy::MachineReadWrite, L"Legacy VMX enable alias."), std::make_shared<VmxCsrSpec>(L"VmxCaps", 0x821, VmxCsrAccessPolicy::ReadOnly, L"Legacy VMX capability alias."), std::make_shared<VmxCsrSpec>(L"VmxControl", 0x822, VmxCsrAccessPolicy::MachineReadWrite, L"Legacy VMX control alias."), std::make_shared<VmxCsrSpec>(L"VmxExitReason", 0x823, VmxCsrAccessPolicy::MachineReadWrite, L"Legacy VMX exit/failure reason projection alias."), std::make_shared<VmxCsrSpec>(L"VmxExitQual", 0x824, VmxCsrAccessPolicy::MachineReadWrite, L"Legacy VMX exit qualification projection alias.") };

	const std::vector<std::shared_ptr<VmExitReasonSpec>> VmxSpecTable::Vmx8ExitReasons = { std::make_shared<VmExitReasonSpec>(L"None", 0), std::make_shared<VmExitReasonSpec>(L"ExternalInterrupt", 1), std::make_shared<VmExitReasonSpec>(L"TripleFault", 2), std::make_shared<VmExitReasonSpec>(L"InitSignal", 3), std::make_shared<VmExitReasonSpec>(L"Hlt", 12), std::make_shared<VmExitReasonSpec>(L"VmCall", 18), std::make_shared<VmExitReasonSpec>(L"VmxOff", 26), std::make_shared<VmExitReasonSpec>(L"InvalidGuestState", 33), std::make_shared<VmExitReasonSpec>(L"EntryFailMsrLoading", 34), std::make_shared<VmExitReasonSpec>(L"EntryFailMachineCheck", 41) };

	const std::vector<std::shared_ptr<VmExitReasonSpec>> VmxSpecTable::VmxV2ExitReasons = { std::make_shared<VmExitReasonSpec>(L"VmFunc", 59), std::make_shared<VmExitReasonSpec>(L"InstructionIntercept", 60), std::make_shared<VmExitReasonSpec>(L"CsrIntercept", 61), std::make_shared<VmExitReasonSpec>(L"MemoryIntercept", 62), std::make_shared<VmExitReasonSpec>(L"VmxOperationIntercept", 63), std::make_shared<VmExitReasonSpec>(L"LaneOperationIntercept", 64), std::make_shared<VmExitReasonSpec>(L"VirtualTimer", 65), std::make_shared<VmExitReasonSpec>(L"VmxPreemptionTimerExpired", 66), std::make_shared<VmExitReasonSpec>(L"VirtualInterrupt", 67), std::make_shared<VmExitReasonSpec>(L"DmaDescriptorFault", 68), std::make_shared<VmExitReasonSpec>(L"IommuFault", 69), std::make_shared<VmExitReasonSpec>(L"DmaPermissionFault", 70), std::make_shared<VmExitReasonSpec>(L"DmaAbort", 71), std::make_shared<VmExitReasonSpec>(L"DmaReplay", 72), std::make_shared<VmExitReasonSpec>(L"VectorException", 73), std::make_shared<VmExitReasonSpec>(L"StreamDescriptorFault", 74), std::make_shared<VmExitReasonSpec>(L"StreamReplayRequired", 75), std::make_shared<VmExitReasonSpec>(L"Lane7TokenFault", 76), std::make_shared<VmExitReasonSpec>(L"Lane7BackendUnavailable", 77), std::make_shared<VmExitReasonSpec>(L"Lane7QuotaExceeded", 78), std::make_shared<VmExitReasonSpec>(L"SecurityPolicyViolation", 79) };

	const std::vector<std::shared_ptr<VmxOpcodeSpec>> VmxSpecTable::getVmx8Opcodes()
	{
		return Vmx8Opcodes;
	}

	const std::vector<std::shared_ptr<VmcsFieldSpec>> VmxSpecTable::getVmx8VmcsFields()
	{
		return Vmx8VmcsFields;
	}

	const std::vector<std::shared_ptr<VmxCsrSpec>> VmxSpecTable::getVmx8Csrs()
	{
		return Vmx8Csrs;
	}

	const std::vector<std::shared_ptr<VmExitReasonSpec>> VmxSpecTable::getVmx8ExitReasons()
	{
		return Vmx8ExitReasons;
	}

	const std::vector<std::shared_ptr<VmExitReasonSpec>> VmxSpecTable::getVmxV2ExitReasons()
	{
		return VmxV2ExitReasons;
	}

	std::shared_ptr<VmxOpcodeSpec> VmxSpecTable::GetVmx8Opcode(unsigned short opcode)
	{
		for (auto spec : getVmx8Opcodes())
		{
			if (spec->Opcode == opcode)
			{
				return spec;
			}
		}

		throw ArgumentOutOfRangeException(L"opcode", opcode, L"Opcode is not a frozen VMX8 opcode.");
	}
}
