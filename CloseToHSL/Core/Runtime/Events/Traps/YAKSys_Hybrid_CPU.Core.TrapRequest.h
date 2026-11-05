#pragma once
#include "TrapRequest.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
enum class VmxOperationKind : unsigned char;
struct TrapRequest {
    const TrapTargetKind TargetKind; const unsigned long long Target, Auxiliary; const TrapAccessType AccessType;
    const unsigned char VtId; const unsigned short ExecutionDomainTag, AddressSpaceTag;
    TrapRequest(TrapTargetKind kind = TrapTargetKind::None, unsigned long long target = 0, unsigned long long auxiliary = 0, TrapAccessType access = TrapAccessType::None, unsigned char vt = 0, unsigned short domain = 0, unsigned short space = 0) : TargetKind(kind), Target(target), Auxiliary(auxiliary), AccessType(access), VtId(vt), ExecutionDomainTag(domain), AddressSpaceTag(space) {}
    static std::shared_ptr<TrapRequest> ForInstruction(unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::InstructionOpcode, opcode, 0, TrapAccessType::Execute, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForCsr(unsigned short csr, TrapAccessType access, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::CsrAddress, csr, 0, access, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForCompatibilityOperation(unsigned char operation, unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::CompatibilityOperation, operation, opcode, TrapAccessType::CompatibilityOperation, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForVmxOperation(VmxOperationKind operation, unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return ForCompatibilityOperation(static_cast<unsigned char>(operation), opcode, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForMemory(unsigned long long address, unsigned long long length, TrapAccessType access, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::MemoryRange, address, length, access, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForLaneOperation(unsigned char lane, unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::LaneOperation, lane, opcode, TrapAccessType::LaneOperation, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForPreemptionTimer(unsigned char vt, unsigned short domain, unsigned short space, unsigned long long deadline) { return std::make_shared<TrapRequest>(TrapTargetKind::PreemptionTimer, deadline, 0, TrapAccessType::TimerExpiry, vt, domain, space); }
};
}