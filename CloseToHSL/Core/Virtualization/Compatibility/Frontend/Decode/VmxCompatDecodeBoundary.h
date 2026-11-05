#pragma once

#include "VmxInstructionPayload.h"
#include <memory>
#include <string>
#include <utility>

// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCompatDecodeDecision : unsigned char
	{
		Allowed = 0,
		UnknownOpcode = 1,
		DescriptorValidationDenied = 2,
		CapabilityValidationDenied = 3,
		SchedulingValidationDenied = 4,
		NoEmissionValidationDenied = 5
	};

struct VmxCompatDecodeRequest {
    const unsigned short Opcode; const unsigned char Rd, Rs1, Rs2; const bool DescriptorValidated, CapabilityValidated, SchedulingValidated, NoEmissionValidated;
    VmxCompatDecodeRequest(unsigned short opcode, unsigned char rd, unsigned char rs1, unsigned char rs2, bool descriptor, bool capability, bool scheduling, bool noEmission) : Opcode(opcode), Rd(rd), Rs1(rs1), Rs2(rs2), DescriptorValidated(descriptor), CapabilityValidated(capability), SchedulingValidated(scheduling), NoEmissionValidated(noEmission) {}
};
struct VmxCompatDecodeResult {
    const VmxCompatDecodeDecision Decision; const std::shared_ptr<VmxInstructionPayload> Payload; const std::wstring Reason; const bool IsAllowed;
    VmxCompatDecodeResult(VmxCompatDecodeDecision decision, std::shared_ptr<VmxInstructionPayload> payload, std::wstring reason) : Decision(decision), Payload(std::move(payload)), Reason(std::move(reason)), IsAllowed(decision == VmxCompatDecodeDecision::Allowed) {}
    static std::shared_ptr<VmxCompatDecodeResult> Allowed(std::shared_ptr<VmxInstructionPayload> payload) { return std::make_shared<VmxCompatDecodeResult>(VmxCompatDecodeDecision::Allowed, std::move(payload), L""); }
    static std::shared_ptr<VmxCompatDecodeResult> Denied(VmxCompatDecodeDecision decision, std::wstring reason) { return std::make_shared<VmxCompatDecodeResult>(decision, VmxInstructionPayload::Empty, std::move(reason)); }
};
}