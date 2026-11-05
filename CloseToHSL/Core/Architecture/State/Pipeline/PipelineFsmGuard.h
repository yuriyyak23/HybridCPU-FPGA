#pragma once

#include "PipelineState.h"
#include "PipelineTransitionTrigger.h"
#include "../../../Pipeline/PipelineEvent.h"
#include <unordered_map>
#include <map>
#include <utility>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Pipeline;

namespace YAKSys_Hybrid_CPU::Core
{
	using namespace Pipeline;
	/// <summary>
	/// Thrown when a pipeline FSM transition is attempted that has no legal target state.
	/// This indicates a deterministic execution contract violation: the pipeline must
	/// never be placed in an architecturally undefined state.
	/// </summary>
	class IllegalFsmTransitionException final : public std::runtime_error, public std::enable_shared_from_this<IllegalFsmTransitionException>
	{
	private:
		const PipelineState FromState;
		const PipelineTransitionTrigger Trigger;

		/// <summary>The pipeline state from which the illegal transition was attempted.</summary>
	public:
		PipelineState getFromState() const;

		/// <summary>The trigger that caused the illegal transition attempt.</summary>
		PipelineTransitionTrigger getTrigger() const;

		IllegalFsmTransitionException(PipelineState fromState, PipelineTransitionTrigger trigger);
	};

	/// <summary>
	/// Validates pipeline FSM state transitions and enforces the HybridCPU
	/// deterministic execution contract.
	///
	/// All transitions not present in the transition table are illegal and
	/// will throw <see cref="IllegalFsmTransitionException"/>.
	///
	/// The transition table is the single authoritative source of truth for
	/// legal pipeline state transitions.
	/// Non-VMX runtime reaches it through the <see cref="PipelineEvent"/> plane.
	/// VMX runtime reaches it through typed retire triggers rather than a
	/// compatibility event mirror.
	/// </summary>
	class PipelineFsmGuard final
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyDictionary<(PipelineState, PipelineTransitionTrigger), PipelineState> TransitionTable = new Dictionary<(PipelineState, PipelineTransitionTrigger), PipelineState> {{ (PipelineState.Reset, PipelineTransitionTrigger.Init), PipelineState.Task }, { (PipelineState.Task, PipelineTransitionTrigger.VmLaunch), PipelineState.VmEntry }, { (PipelineState.Task, PipelineTransitionTrigger.VmResume), PipelineState.VmEntry }, { (PipelineState.Task, PipelineTransitionTrigger.HaltAll), PipelineState.Halted }, { (PipelineState.VmEntry, PipelineTransitionTrigger.EntryOk), PipelineState.GuestExecution }, { (PipelineState.VmEntry, PipelineTransitionTrigger.EntryFail), PipelineState.Task }, { (PipelineState.GuestExecution, PipelineTransitionTrigger.VmExitCond), PipelineState.VmExit }, { (PipelineState.GuestExecution, PipelineTransitionTrigger.VmxOff), PipelineState.VmExit }, { (PipelineState.VmExit, PipelineTransitionTrigger.ExitComplete), PipelineState.Task }, { (PipelineState.Task, PipelineTransitionTrigger.Interrupt), PipelineState.Task }, { (PipelineState.Halted, PipelineTransitionTrigger.Interrupt), PipelineState.Task }, { (PipelineState.Task, PipelineTransitionTrigger.EnterClusterSync), PipelineState.WaitForClusterSync }, { (PipelineState.WaitForClusterSync, PipelineTransitionTrigger.ExitClusterSync), PipelineState.Task }, { (PipelineState.WaitForClusterSync, PipelineTransitionTrigger.Interrupt), PipelineState.Task }, { (PipelineState.Task, PipelineTransitionTrigger.PtwStart), PipelineState.PtwStall }, { (PipelineState.PtwStall, PipelineTransitionTrigger.PtwComplete), PipelineState.Task }, { (PipelineState.PtwStall, PipelineTransitionTrigger.PtwFault), PipelineState.Task }, { (PipelineState.PtwStall, PipelineTransitionTrigger.Interrupt), PipelineState.Task }, { (PipelineState.Task, PipelineTransitionTrigger.EnterClockGate), PipelineState.ClockGatedDonor }, { (PipelineState.ClockGatedDonor, PipelineTransitionTrigger.ExitClockGate), PipelineState.Task }, { (PipelineState.ClockGatedDonor, PipelineTransitionTrigger.Interrupt), PipelineState.Task }, { (PipelineState.Task, PipelineTransitionTrigger.TrapEnter), PipelineState.TrapPending }, { (PipelineState.TrapPending, PipelineTransitionTrigger.TrapReturn), PipelineState.Task }, { (PipelineState.TrapPending, PipelineTransitionTrigger.Interrupt), PipelineState.Task }, { (PipelineState.Task, PipelineTransitionTrigger.EnterWaitForEvent), PipelineState.WaitForEvent }, { (PipelineState.WaitForEvent, PipelineTransitionTrigger.ExitWaitForEvent), PipelineState.Task }, { (PipelineState.WaitForEvent, PipelineTransitionTrigger.Interrupt), PipelineState.Task }};
		static inline const std::map<std::pair<PipelineState, PipelineTransitionTrigger>, PipelineState> TransitionTable = std::map<std::pair<PipelineState, PipelineTransitionTrigger>, PipelineState>
		{
			{{PipelineState::Reset, PipelineTransitionTrigger::Init}, PipelineState::Task},
			{{PipelineState::Task, PipelineTransitionTrigger::VmLaunch}, PipelineState::VmEntry},
			{{PipelineState::Task, PipelineTransitionTrigger::VmResume}, PipelineState::VmEntry},
			{{PipelineState::Task, PipelineTransitionTrigger::HaltAll}, PipelineState::Halted},
			{{PipelineState::VmEntry, PipelineTransitionTrigger::EntryOk}, PipelineState::GuestExecution},
			{{PipelineState::VmEntry, PipelineTransitionTrigger::EntryFail}, PipelineState::Task},
			{{PipelineState::GuestExecution, PipelineTransitionTrigger::VmExitCond}, PipelineState::VmExit},
			{{PipelineState::GuestExecution, PipelineTransitionTrigger::VmxOff}, PipelineState::VmExit},
			{{PipelineState::VmExit, PipelineTransitionTrigger::ExitComplete}, PipelineState::Task},
			{{PipelineState::Task, PipelineTransitionTrigger::Interrupt}, PipelineState::Task},
			{{PipelineState::Halted, PipelineTransitionTrigger::Interrupt}, PipelineState::Task},
			{{PipelineState::Task, PipelineTransitionTrigger::EnterClusterSync}, PipelineState::WaitForClusterSync},
			{{PipelineState::WaitForClusterSync, PipelineTransitionTrigger::ExitClusterSync}, PipelineState::Task},
			{{PipelineState::WaitForClusterSync, PipelineTransitionTrigger::Interrupt}, PipelineState::Task},
			{{PipelineState::Task, PipelineTransitionTrigger::PtwStart}, PipelineState::PtwStall},
			{{PipelineState::PtwStall, PipelineTransitionTrigger::PtwComplete}, PipelineState::Task},
			{{PipelineState::PtwStall, PipelineTransitionTrigger::PtwFault}, PipelineState::Task},
			{{PipelineState::PtwStall, PipelineTransitionTrigger::Interrupt}, PipelineState::Task},
			{{PipelineState::Task, PipelineTransitionTrigger::EnterClockGate}, PipelineState::ClockGatedDonor},
			{{PipelineState::ClockGatedDonor, PipelineTransitionTrigger::ExitClockGate}, PipelineState::Task},
			{{PipelineState::ClockGatedDonor, PipelineTransitionTrigger::Interrupt}, PipelineState::Task},
			{{PipelineState::Task, PipelineTransitionTrigger::TrapEnter}, PipelineState::TrapPending},
			{{PipelineState::TrapPending, PipelineTransitionTrigger::TrapReturn}, PipelineState::Task},
			{{PipelineState::TrapPending, PipelineTransitionTrigger::Interrupt}, PipelineState::Task},
			{{PipelineState::Task, PipelineTransitionTrigger::EnterWaitForEvent}, PipelineState::WaitForEvent},
			{{PipelineState::WaitForEvent, PipelineTransitionTrigger::ExitWaitForEvent}, PipelineState::Task},
			{{PipelineState::WaitForEvent, PipelineTransitionTrigger::Interrupt}, PipelineState::Task}
		};

		/// <summary>
		/// Validates and returns the next pipeline state for a given (current state, trigger) pair.
		/// </summary>
		/// <param name="current">The current pipeline state.</param>
		/// <param name="trigger">The event triggering the transition.</param>
		/// <returns>The legal target state.</returns>
		/// <exception cref="IllegalFsmTransitionException">
		/// Thrown when no legal transition exists for the given (state, trigger) pair.
		/// </exception>
	public:
		static PipelineState Transition(PipelineState current, PipelineTransitionTrigger trigger);

		/// <summary>
		/// Returns true if the given (current state, trigger) pair has a legal transition.
		/// Does not perform the transition.
		/// </summary>
		static bool IsLegalTransition(PipelineState current, PipelineTransitionTrigger trigger);

		/// <summary>
		/// Advances the pipeline FSM in response to a <see cref="PipelineEvent"/>.
		/// This is the canonical event-to-transition entry point for the live pipeline.
		/// </summary>
		/// <param name="current">The current pipeline FSM state.</param>
		/// <param name="evt">The pipeline event that was observed.</param>
		/// <returns>
		/// The new pipeline FSM state after consuming <paramref name="evt"/>,
		/// or <paramref name="current"/> if the event has no FSM mapping.
		/// </returns>
		/// <exception cref="IllegalFsmTransitionException">
		/// Thrown when the event maps to a trigger that has no legal transition
		/// from <paramref name="current"/>.
		/// </exception>
		/// <exception cref="ArgumentNullException">
		/// Thrown when <paramref name="evt"/> is <see langword="null"/>.
		/// </exception>
		static PipelineState Advance(PipelineState current, std::shared_ptr<PipelineEvent> evt);

		/// <summary>
		/// Maps a <see cref="PipelineEvent"/> subtype to its
		/// <see cref="PipelineTransitionTrigger"/>, or returns
		/// <see langword="null"/> if the event does not drive an FSM transition.
		/// </summary>
	private:
		static std::optional<PipelineTransitionTrigger> TryMapEventToTrigger(std::shared_ptr<PipelineEvent> evt);
	};
}


