#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Decoder/Rf06ExecutionContracts.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Execute-stage and execution-local pipeline latches. This container does not
	/// own micro-op semantics, hazard decisions, fault selection or publication.
	/// </summary>
	class ExecutionState final : public std::enable_shared_from_this<ExecutionState>
	{
	public:
		Processor::CPU_Core::ExecuteStage Execute;
		Processor::CPU_Core::PipelineControl Control;
		Processor::CPU_Core::ForwardingPath ExecuteForwarding;
		Processor::CPU_Core::ForwardingPath MemoryForwarding;
		Processor::CPU_Core::ForwardingPath WriteBackForwarding;
		const std::shared_ptr<Decoder::OperationAttemptIssuer> OperationAttemptIssuer = std::make_shared<Decoder::OperationAttemptIssuer>();
	};

}
