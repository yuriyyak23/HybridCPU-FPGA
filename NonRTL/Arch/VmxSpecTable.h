#pragma once

#include "OpcodeInfo.Types.h"
#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Arch
{
	enum class VmxOperandForm : unsigned char
	{
		None = 0,
		ReservedIgnoredRegister = 1,
		FieldSelectorToRegister = 2,
		FieldSelectorAndValueRegisters = 3,
		VmcsPointerRegister = 4
	};

	enum class VmxRegisterRoleMask : unsigned char
	{
		None = 0,
		ReadsRs1 = 1 << 0,
		ReadsRs2 = 1 << 1,
		WritesRd = 1 << 2
	};

	constexpr VmxRegisterRoleMask operator|(VmxRegisterRoleMask left, VmxRegisterRoleMask right) noexcept
	{
		return static_cast<VmxRegisterRoleMask>(static_cast<unsigned char>(left) | static_cast<unsigned char>(right));
	}

	enum class VmcsFieldGroup : unsigned char
	{
		GuestState = 0,
		HostState = 1,
		Controls = 2,
		NestedTranslation = 3,
		ExitInfo = 4,
		NestedTranslationExitInfo = 5
	};

	enum class VmcsVisibilityPolicy : unsigned char
	{
		GuestArchitecturalState = 0,
		HostArchitecturalState = 1,
		RootOwnedControl = 2,
		GuestVisibleByPolicy = 3,
		HostOwnedRuntimeEvidence = 4
	};

	enum class VmxCsrAccessPolicy : unsigned char
	{
		ReadOnly = 0,
		MachineReadWrite = 1
	};

	struct VmxOpcodeSpec final
	{
		unsigned short Opcode;
		std::wstring Mnemonic;
		unsigned char OperandCount;
		InstructionFlags Flags;
		VmxOperandForm OperandForm;
		std::wstring OperationKindName;
		std::wstring InternalOpKindName;
		VmxRegisterRoleMask RegisterRoleMask;
		std::wstring CompatibilityNote;
		VmxOpcodeSpec(unsigned short opcode, std::wstring mnemonic, unsigned char operandCount, InstructionFlags flags, VmxOperandForm operandForm, std::wstring operationKindName, std::wstring internalOpKindName, VmxRegisterRoleMask registerRoleMask, std::wstring compatibilityNote)
			: Opcode(opcode), Mnemonic(std::move(mnemonic)), OperandCount(operandCount), Flags(flags), OperandForm(operandForm), OperationKindName(std::move(operationKindName)), InternalOpKindName(std::move(internalOpKindName)), RegisterRoleMask(registerRoleMask), CompatibilityNote(std::move(compatibilityNote)) {}
	};
	struct VmcsFieldSpec final
	{
		std::wstring Name; unsigned short Id; VmcsFieldGroup Group; VmcsVisibilityPolicy VisibilityPolicy; std::wstring CompatibilityNote;
		VmcsFieldSpec(std::wstring name, unsigned short id, VmcsFieldGroup group, VmcsVisibilityPolicy visibilityPolicy, std::wstring note)
			: Name(std::move(name)), Id(id), Group(group), VisibilityPolicy(visibilityPolicy), CompatibilityNote(std::move(note)) {}
	};
	struct VmxCsrSpec final
	{
		std::wstring Name; unsigned short Address; VmxCsrAccessPolicy AccessPolicy; std::wstring CompatibilityNote;
		VmxCsrSpec(std::wstring name, unsigned short address, VmxCsrAccessPolicy accessPolicy, std::wstring note)
			: Name(std::move(name)), Address(address), AccessPolicy(accessPolicy), CompatibilityNote(std::move(note)) {}
	};
	struct VmExitReasonSpec final
	{
		std::wstring Name; unsigned int Value;
		VmExitReasonSpec(std::wstring name, unsigned int value) : Name(std::move(name)), Value(value) {}
	};

	/// <summary>
	/// Machine-readable VMX architecture seed for Phase 00.
	/// VMX8 entries are the frozen compatibility baseline; VMX-v2 drafts remain
	/// unfrozen until a future VMCSv2 revision is explicitly published here.
	/// </summary>
	class VmxSpecTable final
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<VmxOpcodeSpec> Vmx8Opcodes = [new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMXON, Mnemonic: "VMXON", OperandCount: 1, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.ReservedIgnoredRegister, OperationKindName: "VmxOn", InternalOpKindName: "VmxOn", RegisterRoleMask: VmxRegisterRoleMask.None, CompatibilityNote: "VMX8 preserves one encoded operand as reserved/ignored; VMX-v2 may reinterpret rs1 only behind a future capability."), new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMXOFF, Mnemonic: "VMXOFF", OperandCount: 0, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.None, OperationKindName: "VmxOff", InternalOpKindName: "VmxOff", RegisterRoleMask: VmxRegisterRoleMask.None, CompatibilityNote: "VMX8 control transition; guest-context VMXOFF retires as a VM-exit through runtime state."), new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMLAUNCH, Mnemonic: "VMLAUNCH", OperandCount: 0, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.None, OperationKindName: "VmLaunch", InternalOpKindName: "VmLaunch", RegisterRoleMask: VmxRegisterRoleMask.None, CompatibilityNote: "VM-entry legality remains runtime-owned and active-VMCS dependent."), new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMRESUME, Mnemonic: "VMRESUME", OperandCount: 0, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.None, OperationKindName: "VmResume", InternalOpKindName: "VmResume", RegisterRoleMask: VmxRegisterRoleMask.None, CompatibilityNote: "VM-entry resume legality remains runtime-owned and launched-VMCS dependent."), new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMREAD, Mnemonic: "VMREAD", OperandCount: 2, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.FieldSelectorToRegister, OperationKindName: "VmRead", InternalOpKindName: "VmRead", RegisterRoleMask: VmxRegisterRoleMask.ReadsRs1 | VmxRegisterRoleMask.WritesRd, CompatibilityNote: "Rs1 is the VMCS field selector register; VMX8 does not publish an immediate selector ABI."), new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMWRITE, Mnemonic: "VMWRITE", OperandCount: 2, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.FieldSelectorAndValueRegisters, OperationKindName: "VmWrite", InternalOpKindName: "VmWrite", RegisterRoleMask: VmxRegisterRoleMask.ReadsRs1 | VmxRegisterRoleMask.ReadsRs2, CompatibilityNote: "Rs1 is the VMCS field selector register and Rs2 is the value register."), new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMCLEAR, Mnemonic: "VMCLEAR", OperandCount: 1, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.VmcsPointerRegister, OperationKindName: "VmClear", InternalOpKindName: "VmClear", RegisterRoleMask: VmxRegisterRoleMask.ReadsRs1, CompatibilityNote: "Rs1 carries a VMCS descriptor pointer key; VMX8 does not publish scalar memory side effects."), new(Opcode: Processor.CPU_Core.IsaOpcodeValues.VMPTRLD, Mnemonic: "VMPTRLD", OperandCount: 1, Flags: InstructionFlags.Privileged, OperandForm: VmxOperandForm.VmcsPointerRegister, OperationKindName: "VmPtrLd", InternalOpKindName: "VmPtrLd", RegisterRoleMask: VmxRegisterRoleMask.ReadsRs1, CompatibilityNote: "Rs1 carries a VMCS descriptor pointer key; descriptor materialization is runtime-owned.")];
		static const std::vector<std::shared_ptr<VmxOpcodeSpec>> Vmx8Opcodes;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<VmcsFieldSpec> Vmx8VmcsFields = [new("GuestPc", 0, VmcsFieldGroup.GuestState, VmcsVisibilityPolicy.GuestArchitecturalState, "VMX8 compatibility alias."), new("GuestSp", 1, VmcsFieldGroup.GuestState, VmcsVisibilityPolicy.GuestArchitecturalState, "VMX8 compatibility alias."), new("GuestFlags", 2, VmcsFieldGroup.GuestState, VmcsVisibilityPolicy.GuestArchitecturalState, "VMX8 compatibility alias."), new("GuestCr0", 3, VmcsFieldGroup.GuestState, VmcsVisibilityPolicy.GuestArchitecturalState, "VMX8 compatibility alias."), new("GuestCr3", 4, VmcsFieldGroup.GuestState, VmcsVisibilityPolicy.GuestArchitecturalState, "VMX8 compatibility alias."), new("GuestCr4", 5, VmcsFieldGroup.GuestState, VmcsVisibilityPolicy.GuestArchitecturalState, "VMX8 compatibility alias."), new("HostPc", 32, VmcsFieldGroup.HostState, VmcsVisibilityPolicy.HostArchitecturalState, "VMX8 compatibility alias."), new("HostSp", 33, VmcsFieldGroup.HostState, VmcsVisibilityPolicy.HostArchitecturalState, "VMX8 compatibility alias."), new("HostFlags", 34, VmcsFieldGroup.HostState, VmcsVisibilityPolicy.HostArchitecturalState, "VMX8 compatibility alias."), new("HostCr0", 35, VmcsFieldGroup.HostState, VmcsVisibilityPolicy.HostArchitecturalState, "VMX8 compatibility alias."), new("HostCr3", 36, VmcsFieldGroup.HostState, VmcsVisibilityPolicy.HostArchitecturalState, "VMX8 compatibility alias."), new("PinBasedControls", 64, VmcsFieldGroup.Controls, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias."), new("ProcBasedControls", 65, VmcsFieldGroup.Controls, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias."), new("ExitControls", 66, VmcsFieldGroup.Controls, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias."), new("EntryControls", 67, VmcsFieldGroup.Controls, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias."), new("EptPointer", 80, VmcsFieldGroup.NestedTranslation, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias; not a complete NPT contract in Phase 00."), new("Vpid", 81, VmcsFieldGroup.NestedTranslation, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias; not a complete VPID contract in Phase 00."), new("SecondaryProcControls", 82, VmcsFieldGroup.NestedTranslation, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias."), new("Cr3TargetCount", 83, VmcsFieldGroup.NestedTranslation, VmcsVisibilityPolicy.RootOwnedControl, "VMX8 compatibility alias."), new("ExitReason", 96, VmcsFieldGroup.ExitInfo, VmcsVisibilityPolicy.GuestVisibleByPolicy, "VMX8 compatibility alias for legacy exit projection."), new("ExitQualification", 97, VmcsFieldGroup.ExitInfo, VmcsVisibilityPolicy.GuestVisibleByPolicy, "VMX8 compatibility alias for legacy exit projection."), new("GuestPhysicalAddress", 112, VmcsFieldGroup.NestedTranslationExitInfo, VmcsVisibilityPolicy.GuestVisibleByPolicy, "VMX8 compatibility alias; Phase 03 wires typed NPT qualification."), new("EptViolationQualification", 113, VmcsFieldGroup.NestedTranslationExitInfo, VmcsVisibilityPolicy.GuestVisibleByPolicy, "VMX8 compatibility alias; Phase 03 wires typed NPT qualification.")];
		static const std::vector<std::shared_ptr<VmcsFieldSpec>> Vmx8VmcsFields;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<VmxCsrSpec> Vmx8Csrs = [new("VmxEnable", 0x820, VmxCsrAccessPolicy.MachineReadWrite, "Legacy VMX enable alias."), new("VmxCaps", 0x821, VmxCsrAccessPolicy.ReadOnly, "Legacy VMX capability alias."), new("VmxControl", 0x822, VmxCsrAccessPolicy.MachineReadWrite, "Legacy VMX control alias."), new("VmxExitReason", 0x823, VmxCsrAccessPolicy.MachineReadWrite, "Legacy VMX exit/failure reason projection alias."), new("VmxExitQual", 0x824, VmxCsrAccessPolicy.MachineReadWrite, "Legacy VMX exit qualification projection alias.")];
		static const std::vector<std::shared_ptr<VmxCsrSpec>> Vmx8Csrs;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<VmExitReasonSpec> Vmx8ExitReasons = [new("None", 0), new("ExternalInterrupt", 1), new("TripleFault", 2), new("InitSignal", 3), new("Hlt", 12), new("VmCall", 18), new("VmxOff", 26), new("InvalidGuestState", 33), new("EntryFailMsrLoading", 34), new("EntryFailMachineCheck", 41)];
		static const std::vector<std::shared_ptr<VmExitReasonSpec>> Vmx8ExitReasons;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<VmExitReasonSpec> VmxV2ExitReasons = [new("VmFunc", 59), new("InstructionIntercept", 60), new("CsrIntercept", 61), new("MemoryIntercept", 62), new("VmxOperationIntercept", 63), new("LaneOperationIntercept", 64), new("VirtualTimer", 65), new("VmxPreemptionTimerExpired", 66), new("VirtualInterrupt", 67), new("DmaDescriptorFault", 68), new("IommuFault", 69), new("DmaPermissionFault", 70), new("DmaAbort", 71), new("DmaReplay", 72), new("VectorException", 73), new("StreamDescriptorFault", 74), new("StreamReplayRequired", 75), new("Lane7TokenFault", 76), new("Lane7BackendUnavailable", 77), new("Lane7QuotaExceeded", 78), new("SecurityPolicyViolation", 79)];
		static const std::vector<std::shared_ptr<VmExitReasonSpec>> VmxV2ExitReasons;

	public:
		static inline const std::wstring FrozenCompatibilityBaseline = L"VMX8";
		static constexpr bool VmxV2DraftsAreCompatibilityTargets = false;

		static const std::vector<std::shared_ptr<VmxOpcodeSpec>> getVmx8Opcodes();

		static const std::vector<std::shared_ptr<VmcsFieldSpec>> getVmx8VmcsFields();

		static const std::vector<std::shared_ptr<VmxCsrSpec>> getVmx8Csrs();

		static const std::vector<std::shared_ptr<VmExitReasonSpec>> getVmx8ExitReasons();

		static const std::vector<std::shared_ptr<VmExitReasonSpec>> getVmxV2ExitReasons();

		static std::shared_ptr<VmxOpcodeSpec> GetVmx8Opcode(unsigned short opcode);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxOpcodeSpec(ushort Opcode, string Mnemonic, byte OperandCount, InstructionFlags Flags, VmxOperandForm OperandForm, string OperationKindName, string InternalOpKindName, VmxRegisterRoleMask RegisterRoleMask, string CompatibilityNote)
//	{
//		public InstructionClass InstructionClass
//		{
//			get
//			{
//				return InstructionClass.Vmx;
//			}
//		}
//		public SerializationClass SerializationClass
//		{
//			get
//			{
//				return SerializationClass.VmxSerial;
//			}
//		}
//		public OpcodeCategory Category
//		{
//			get
//			{
//				return OpcodeCategory.Privileged;
//			}
//		}
//		public byte ExecutionLatency
//		{
//			get
//			{
//				return 1;
//			}
//		}
//		public byte MemoryBandwidth
//		{
//			get
//			{
//				return 0;
//			}
//		}
//		public SlotClassSpec SlotClass
//		{
//			get
//			{
//				return SlotClassSpec.SystemSingleton;
//			}
//		}
//		public byte PinnedLaneId
//		{
//			get
//			{
//				return 7;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmcsFieldSpec(string Name, ushort Id, VmcsFieldGroup Group, VmcsVisibilityPolicy VisibilityPolicy, string CompatibilityNote);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxCsrSpec(string Name, ushort Address, VmxCsrAccessPolicy AccessPolicy, string CompatibilityNote);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmExitReasonSpec(string Name, uint Value);

	enum class SlotClassSpec : unsigned char
	{
		SystemSingleton = 7
	};

}
