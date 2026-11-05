#include "PipelineFsmGuard.h"

using namespace YAKSys_Hybrid_CPU::Core::Pipeline;

namespace YAKSys_Hybrid_CPU::Core
{

	PipelineState IllegalFsmTransitionException::getFromState() const
	{
		return FromState;
	}

	PipelineTransitionTrigger IllegalFsmTransitionException::getTrigger() const
	{
		return Trigger;
	}

	IllegalFsmTransitionException::IllegalFsmTransitionException(PipelineState fromState, PipelineTransitionTrigger trigger)
		: std::runtime_error(std::format("Illegal FSM transition: {} + {} has no valid target state. Deterministic execution contract violated.", static_cast<int>(fromState), static_cast<int>(trigger))),
		  FromState(fromState), Trigger(trigger)
	{
	}

	PipelineState PipelineFsmGuard::Transition(PipelineState current, PipelineTransitionTrigger trigger)
	{
		auto TransitionTable_iterator = TransitionTable.find(std::make_pair(current, trigger));
		if (TransitionTable_iterator != TransitionTable.end())
		{
			return TransitionTable_iterator->second;
		}

		throw IllegalFsmTransitionException(current, trigger);
	}

	bool PipelineFsmGuard::IsLegalTransition(PipelineState current, PipelineTransitionTrigger trigger)
	{
		return TransitionTable.contains(std::make_pair(current, trigger));
	}

	PipelineState PipelineFsmGuard::Advance(PipelineState current, std::shared_ptr<PipelineEvent> evt)
	{
		if (evt == nullptr)
		{
			throw std::invalid_argument("evt");
		}

		std::optional<PipelineTransitionTrigger> trigger = TryMapEventToTrigger(evt);
		if (!trigger)
		{
			return current;
		}

		return Transition(current, trigger.value());
	}

	std::optional<PipelineTransitionTrigger> PipelineFsmGuard::TryMapEventToTrigger(std::shared_ptr<PipelineEvent> evt)
	{
		if (std::dynamic_pointer_cast<ResetEvent>(evt)) return PipelineTransitionTrigger::Init;
		if (std::dynamic_pointer_cast<HaltEvent>(evt) || std::dynamic_pointer_cast<WfiEvent>(evt)) return PipelineTransitionTrigger::HaltAll;
		if (std::dynamic_pointer_cast<TrapEntryEvent>(evt)) return PipelineTransitionTrigger::TrapEnter;
		if (std::dynamic_pointer_cast<MretEvent>(evt) || std::dynamic_pointer_cast<SretEvent>(evt)) return PipelineTransitionTrigger::TrapReturn;
		if (std::dynamic_pointer_cast<WfeEvent>(evt)) return PipelineTransitionTrigger::EnterWaitForEvent;
		if (std::dynamic_pointer_cast<SevEvent>(evt)) return PipelineTransitionTrigger::ExitWaitForEvent;
		if (std::dynamic_pointer_cast<ClusterSyncEnterEvent>(evt)) return PipelineTransitionTrigger::EnterClusterSync;
		if (std::dynamic_pointer_cast<ClusterSyncExitEvent>(evt)) return PipelineTransitionTrigger::ExitClusterSync;
		if (std::dynamic_pointer_cast<PtwWalkStartEvent>(evt)) return PipelineTransitionTrigger::PtwStart;
		if (std::dynamic_pointer_cast<PtwWalkCompleteEvent>(evt)) return PipelineTransitionTrigger::PtwComplete;
		if (std::dynamic_pointer_cast<PtwWalkFaultEvent>(evt)) return PipelineTransitionTrigger::PtwFault;
		if (std::dynamic_pointer_cast<ClockGatedDonorEnterEvent>(evt)) return PipelineTransitionTrigger::EnterClockGate;
		if (std::dynamic_pointer_cast<ClockGatedDonorExitEvent>(evt)) return PipelineTransitionTrigger::ExitClockGate;
		return std::nullopt;
	}
}
