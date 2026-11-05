#pragma once
#include "../Lane7StateBlock.h"


#include "../../../../Execution/ExternalAccelerators/Tokens/AcceleratorToken.h"
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core { class Lane7StateBlock; class LaneCompletionDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

	class Lane7AcceleratorCompletionRouter final : public std::enable_shared_from_this<Lane7AcceleratorCompletionRouter>
	{
	private:
		const std::weak_ptr<Lane7StateBlock> _lane7;
		std::shared_ptr<Lane7StateBlock> getLane7() const { auto lane = _lane7.lock(); if (!lane) throw std::runtime_error("Lane7 state no longer exists"); return lane; }

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorCompletionRouter(Lane7StateBlock lane7)
		Lane7AcceleratorCompletionRouter(std::shared_ptr<Lane7StateBlock> lane7);

		bool TryBuildCompletion(std::shared_ptr<AcceleratorToken> token, unsigned short sourceOpcode, unsigned long long runtimeQueueSequence, std::shared_ptr<LaneCompletionDescriptor> &completion, std::shared_ptr<Lane7Fault> &fault);
	};

}
