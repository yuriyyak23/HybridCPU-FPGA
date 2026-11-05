#include "IPipelineEventQueue.h"

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{
	const std::shared_ptr<NullPipelineEventQueue> NullPipelineEventQueue::Instance(new NullPipelineEventQueue());

	NullPipelineEventQueue::NullPipelineEventQueue()
	{
	}

	void NullPipelineEventQueue::Enqueue(std::shared_ptr<PipelineEvent> evt)
	{
	}
}
