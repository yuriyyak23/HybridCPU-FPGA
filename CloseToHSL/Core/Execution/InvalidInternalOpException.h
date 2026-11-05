#pragma once

#include "../Pipeline/MicroOps/Internal/InternalOp.h"
#include "ExecutionFaultContract.h"
#include <stdexcept>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Execution
{
	// ─────────────────────────────────────────────────────────────────────────
	// InvalidInternalOpException — V5 Phase 3
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Thrown by <see cref="ExecutionEngine"/> when an <see cref="InternalOp"/>
	/// with an <see cref="InternalOpKind"/> that has no registered
	/// <see cref="IExecutionUnit"/> reaches the engine.
	///
	/// <para>
	/// This exception indicates a pipeline configuration error — either a new
	/// <see cref="InternalOpKind"/> was added to the taxonomy without a
	/// corresponding unit, or the engine was constructed with an incomplete
	/// unit registry.
	/// </para>
	/// </summary>
	class InvalidInternalOpException final : public std::runtime_error, public ExecutionFaultMetadata
	{
	private:
		const InternalOpKind Kind;

		/// <summary>The unhandled operation kind.</summary>
	public:
		InternalOpKind getKind() const;
		ExecutionFaultCategory getCategory() const;

		/// <summary>
		/// Initializes the exception for the given unregistered
		/// <paramref name="kind"/>.
		/// </summary>
		InvalidInternalOpException(InternalOpKind kind);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public InvalidInternalOpException(InternalOpKind kind, Exception? innerException) : base(ExecutionFaultContract.FormatMessage(ExecutionFaultCategory.InvalidInternalOp, string.Format("No execution unit registered for InternalOpKind.{0}. ", kind) + string.Format("Ensure every InternalOpKind value has a corresponding IExecutionUnit ") + string.Format("in the ExecutionEngine registry.")), innerException)
		InvalidInternalOpException(InternalOpKind kind, std::exception_ptr innerException);
	};
}
