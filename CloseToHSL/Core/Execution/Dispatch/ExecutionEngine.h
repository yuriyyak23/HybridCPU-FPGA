#pragma once

#include "../IExecutionUnit.h"
#include "../../Pipeline/MicroOps/Internal/InternalOp.h"
#include "ExecutionDispatcherV4.h"
#include "../../State/ICpuState.h"
#include <unordered_map>
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Execution
{
	// ─────────────────────────────────────────────────────────────────────────
	// ExecutionEngine — V5 Phase 3
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// ISA-agnostic execution engine.
	///
	/// <para>
	/// Dispatches <see cref="InternalOp"/> instances to typed
	/// <see cref="IExecutionUnit"/> implementations keyed by
	/// <see cref="InternalOpKind"/>. The engine has no knowledge of
	/// <c>IsaV4Opcode</c>, <c>InstructionsEnum</c>, or any other ISA-specific
	/// concept — all ISA translation happens upstream in
	/// <c>IInternalOpBuilder</c> (Phase 2).
	/// </para>
	///
	/// <para>
	/// <b>Construction:</b> pass a fully populated registry. If any
	/// <see cref="InternalOpKind"/> value is submitted that lacks a registered
	/// unit the engine throws <see cref="InvalidInternalOpException"/>, which
	/// surfaces pipeline configuration errors at test time.
	/// </para>
	///
	/// <para>
	/// <b>Thread safety:</b> the registry dictionary is read-only after
	/// construction. The engine itself is thread-safe provided each VT's
	/// <see cref="ICanonicalCpuState"/> is not shared across threads.
	/// </para>
	/// </summary>
	class ExecutionEngine final : public std::enable_shared_from_this<ExecutionEngine>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyDictionary<InternalOpKind, IExecutionUnit> _units;
		const std::unordered_map<InternalOpKind, std::shared_ptr<IExecutionUnit>> _units;

		/// <summary>
		/// Constructs the engine with the given unit registry.
		/// </summary>
		/// <param name="units">
		/// Mapping from <see cref="InternalOpKind"/> to the execution unit
		/// responsible for that kind. Must not be <see langword="null"/>.
		/// </param>
		/// <exception cref="ArgumentNullException">
		/// Thrown when <paramref name="units"/> is <see langword="null"/>.
		/// </exception>
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public ExecutionEngine(IReadOnlyDictionary<InternalOpKind, IExecutionUnit> units)
		ExecutionEngine(const std::unordered_map<InternalOpKind, std::shared_ptr<IExecutionUnit>> &units);

		/// <summary>
		/// Executes <paramref name="op"/> by dispatching to the registered
		/// <see cref="IExecutionUnit"/> for <c>op.Kind</c>.
		/// </summary>
		/// <param name="op">The resolved micro-operation to execute.</param>
		/// <param name="state">VT-scoped CPU state.</param>
		/// <returns>The execution outcome.</returns>
		/// <exception cref="InvalidInternalOpException">
		/// Thrown when <c>op.Kind</c> has no registered execution unit.
		/// </exception>
		ExecutionResult Execute(std::shared_ptr<InternalOp> op, std::shared_ptr<ICanonicalCpuState> state);
	};
}

