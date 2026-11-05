#pragma once
#include "../Lane7StateBlock.h"


#include "../../../../Execution/ExternalAccelerators/Tokens/AcceleratorToken.h"
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core { class Lane7StateBlock; class LaneCompletionDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

	class Lane7AcceleratorTokenNamespace final : public std::enable_shared_from_this<Lane7AcceleratorTokenNamespace>
	{
	private:
		const std::weak_ptr<Lane7StateBlock> _lane7;
		std::shared_ptr<Lane7StateBlock> getLane7() const { auto lane = _lane7.lock(); if (!lane) throw std::runtime_error("Lane7 state no longer exists"); return lane; }

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorTokenNamespace(Lane7StateBlock lane7)
		Lane7AcceleratorTokenNamespace(std::shared_ptr<Lane7StateBlock> lane7);

		bool TryMapHostToken(std::shared_ptr<Lane7VirtualHandle> handle, unsigned short addressSpaceTag, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault);

		bool TryResolveHostToken(unsigned long long virtualTokenId, unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, std::shared_ptr<AcceleratorTokenHandle> &hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault);

		bool TryResolveVirtualTokenForHost(std::shared_ptr<AcceleratorTokenHandle> hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken);

		bool Release(unsigned long long virtualTokenId);
	};

}
