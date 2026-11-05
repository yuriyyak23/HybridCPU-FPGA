#pragma once
#include "../../../Execution/ExternalAccelerators/Tokens/AcceleratorTokenHandle.h"
#include "../../../Execution/ExternalAccelerators/Tokens/AcceleratorTokenStatusWord.h"
namespace YAKSys_Hybrid_CPU::Core {
struct Lane7VirtualToken final {
 unsigned short ExecutionDomainTag=0, AddressSpaceTag=0, OwnerVirtualThreadId=0; unsigned long long VirtualHandle=0, VirtualTokenId=0, TokenEpoch=0;
 Execution::ExternalAccelerators::Tokens::AcceleratorTokenStatusWord Status{Execution::ExternalAccelerators::Tokens::AcceleratorTokenState::Created, Execution::ExternalAccelerators::Tokens::AcceleratorTokenFaultCode::None, static_cast<Execution::ExternalAccelerators::Tokens::AcceleratorTokenStatusFlags>(0)};
 unsigned long long CompletionEpoch=0;
 bool IsValid() const noexcept { return ExecutionDomainTag && VirtualHandle && VirtualTokenId && TokenEpoch; }
 bool ExposesHostTokenHandle(const Execution::ExternalAccelerators::Tokens::AcceleratorTokenHandle&) const noexcept { return false; }
};
}
