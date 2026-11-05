#include "Lane7AcceleratorTokenNamespace.h"
#include "../YAKSys_Hybrid_CPU.Core.Lane7StateBlock.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorTokenNamespace(Lane7StateBlock lane7)
	Lane7AcceleratorTokenNamespace::Lane7AcceleratorTokenNamespace(std::shared_ptr<Lane7StateBlock> lane7) : _lane7((lane7 != nullptr) ? lane7 : throw std::invalid_argument("lane7"))
	{
	}

	bool Lane7AcceleratorTokenNamespace::TryMapHostToken(std::shared_ptr<Lane7VirtualHandle> handle, unsigned short addressSpaceTag, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault)
	{
		return getLane7()->TryMapHostToken(handle, addressSpaceTag, token, virtualToken, fault);
	}

	bool Lane7AcceleratorTokenNamespace::TryResolveHostToken(unsigned long long virtualTokenId, unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, std::shared_ptr<AcceleratorTokenHandle> &hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault)
	{
		return getLane7()->TryResolveHostToken(virtualTokenId, executionDomainTag, addressSpaceTag, ownerVirtualThreadId, hostHandle, virtualToken, fault);
	}

	bool Lane7AcceleratorTokenNamespace::TryResolveVirtualTokenForHost(std::shared_ptr<AcceleratorTokenHandle> hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken)
	{
		return getLane7()->TryResolveVirtualTokenForHost(hostHandle, virtualToken);
	}

	bool Lane7AcceleratorTokenNamespace::Release(unsigned long long virtualTokenId)
	{
		return getLane7()->ReleaseVirtualToken(virtualTokenId);
	}
}
