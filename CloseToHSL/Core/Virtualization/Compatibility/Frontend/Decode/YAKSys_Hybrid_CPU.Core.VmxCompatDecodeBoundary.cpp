#include "YAKSys_Hybrid_CPU.Core.VmxCompatDecodeBoundary.h"
#include "VmxCompatDecodeBoundary.h"
#include "VmxFrozenOpcodeValues.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmxCompatDecodeResult> VmxCompatDecodeBoundary::Decode(std::shared_ptr<VmxCompatDecodeRequest> request)
	{
		if (!IsFrozenVmxOpcode(request->Opcode))
		{
			return VmxCompatDecodeResult::Denied(VmxCompatDecodeDecision::UnknownOpcode, L"Opcode is not part of the frozen VMX compatibility frontend.");
		}

		if (!request->DescriptorValidated)
		{
			return VmxCompatDecodeResult::Denied(VmxCompatDecodeDecision::DescriptorValidationDenied, L"VMX compatibility decode requires descriptor validation.");
		}

		if (!request->CapabilityValidated)
		{
			return VmxCompatDecodeResult::Denied(VmxCompatDecodeDecision::CapabilityValidationDenied, L"VMX compatibility decode requires capability validation.");
		}

		if (!request->SchedulingValidated)
		{
			return VmxCompatDecodeResult::Denied(VmxCompatDecodeDecision::SchedulingValidationDenied, L"VMX compatibility decode requires scheduling validation.");
		}

		if (!request->NoEmissionValidated)
		{
			return VmxCompatDecodeResult::Denied(VmxCompatDecodeDecision::NoEmissionValidationDenied, L"VMX compatibility decode requires no-emission validation.");
		}

		return VmxCompatDecodeResult::Allowed(VmxInstructionPayload::FromDecodedRegisters(request->Opcode, request->Rd, request->Rs1, request->Rs2));
	}

	bool VmxCompatDecodeBoundary::IsFrozenVmxOpcode(unsigned short opcode)
	{
		return (opcode == VmxFrozenOpcodeValues::VMXON || opcode == VmxFrozenOpcodeValues::VMXOFF || opcode == VmxFrozenOpcodeValues::VMLAUNCH || opcode == VmxFrozenOpcodeValues::VMRESUME || opcode == VmxFrozenOpcodeValues::VMREAD || opcode == VmxFrozenOpcodeValues::VMWRITE || opcode == VmxFrozenOpcodeValues::VMCLEAR || opcode == VmxFrozenOpcodeValues::VMPTRLD || opcode == VmxFrozenOpcodeValues::VMPTRST || opcode == VmxFrozenOpcodeValues::VMCALL || opcode == VmxFrozenOpcodeValues::INVEPT || opcode == VmxFrozenOpcodeValues::INVVPID || opcode == VmxFrozenOpcodeValues::VMFUNC || opcode == VmxFrozenOpcodeValues::VMSAVEX || opcode == VmxFrozenOpcodeValues::VMRESTX);
	}
}
