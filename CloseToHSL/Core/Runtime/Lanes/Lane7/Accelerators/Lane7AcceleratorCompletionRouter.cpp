#include "Lane7AcceleratorCompletionRouter.h"
#include "../YAKSys_Hybrid_CPU.Core.Lane7StateBlock.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorCompletionRouter(Lane7StateBlock lane7)
	Lane7AcceleratorCompletionRouter::Lane7AcceleratorCompletionRouter(std::shared_ptr<Lane7StateBlock> lane7) : _lane7((lane7 != nullptr) ? lane7 : throw std::invalid_argument("lane7"))
	{
	}

	bool Lane7AcceleratorCompletionRouter::TryBuildCompletion(std::shared_ptr<AcceleratorToken> token, unsigned short sourceOpcode, unsigned long long runtimeQueueSequence, std::shared_ptr<LaneCompletionDescriptor> &completion, std::shared_ptr<Lane7Fault> &fault)
	{
		return getLane7()->TryBuildCompletion(token, sourceOpcode, runtimeQueueSequence, completion, fault);
	}
}
