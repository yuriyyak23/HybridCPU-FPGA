#pragma once

#include "ExecutionFaultContract.h"
#include "../Pipeline/MicroOps/Types/MicroOp.Exceptions.h"
#include "Memory/LoadStore/MemoryUnit.h"
#include "../Decoder/Rf06ExecutionContracts.h"
#include <string>
#include <stdexcept>
#include <optional>
#include <mutex>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Decoder { class ScheduledOperation; }
// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core::Execution { class ExecutionOutcomeContractViolationException; }

using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution
{

	/// <summary>
	/// The single RF-07 terminal outcome family for one issued execution attempt.
	/// These values neither select retirement nor publish architectural effects.
	/// </summary>
	enum class ExecutionOutcomeKind : unsigned char
	{
		Completed = 1,
		ArchitecturalFault = 2,
		Retryable = 3,
		StructuralBlocked = 4,
		BackendUnavailable = 5,
		FatalInvariantViolation = 6
	};

	/// <summary>
	/// Typed diagnostic codes carried by non-completed outcomes.  A code has one
	/// fixed disposition and therefore cannot be reclassified as retry by a caller.
	/// </summary>
	enum class ExecutionDiagnosticCode : unsigned char
	{
		PageFault = 1,
		AlignmentFault = 2,
		ResourceWait = 3,
		StructuralHazard = 4,
		RuntimeBackendUnavailable = 5,
		ExistingExecutionFault = 6,
		UnknownException = 7,
		OutcomeContractViolation = 8,
		SpeculativeFaultSuppressed = 9
	};

	/// <summary>
	/// Immutable diagnostic payload. Exception objects are deliberately not retained;
	/// only stable type/category/message evidence crosses this contract.
	/// </summary>
	class ExecutionDiagnostic final : public std::enable_shared_from_this<ExecutionDiagnostic>
	{
	private:
		const ExecutionOutcomeKind Disposition;
		const ExecutionDiagnosticCode Code;
		const std::wstring Reason;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ulong> FaultAddress;
		std::optional<unsigned long long> FaultAddress;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<bool> FaultIsWrite;
		std::optional<bool> FaultIsWrite;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExecutionFaultCategory> LegacyFaultCategory;
		std::optional<ExecutionFaultCategory> LegacyFaultCategory;
		const std::wstring ExceptionType;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionDiagnostic(ExecutionOutcomeKind disposition, ExecutionDiagnosticCode code, string reason, System.Nullable<ulong> faultAddress, System.Nullable<bool> faultIsWrite, System.Nullable<ExecutionFaultCategory> legacyFaultCategory, string? exceptionType)
		ExecutionDiagnostic(ExecutionOutcomeKind disposition, ExecutionDiagnosticCode code, const std::wstring &reason, std::optional<unsigned long long> faultAddress, std::optional<bool> faultIsWrite, std::optional<ExecutionFaultCategory> legacyFaultCategory, const std::wstring &exceptionType);

	public:
		ExecutionOutcomeKind getDisposition() const;
		ExecutionDiagnosticCode getCode() const;
		std::wstring getReason() const;
		std::optional<unsigned long long> getFaultAddress() const;
		std::optional<bool> getFaultIsWrite() const;
		std::optional<ExecutionFaultCategory> getLegacyFaultCategory() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? ExceptionType {get;}
		std::wstring getExceptionType() const;

		static std::shared_ptr<ExecutionDiagnostic> PageFault(std::shared_ptr<PageFaultException> exception);

		static std::shared_ptr<ExecutionDiagnostic> AlignmentFault(std::shared_ptr<MemoryAlignmentException> exception, bool isWrite);

		static std::shared_ptr<ExecutionDiagnostic> Retryable(const std::wstring &reason);

		static std::shared_ptr<ExecutionDiagnostic> StructuralBlocked(const std::wstring &reason);

		static std::shared_ptr<ExecutionDiagnostic> SpeculativeFaultSuppressed(const std::wstring &reason);

		static std::shared_ptr<ExecutionDiagnostic> BackendUnavailable(const std::wstring &reason);

		static std::shared_ptr<ExecutionDiagnostic> Fatal(const std::exception& exception);

		static std::shared_ptr<ExecutionDiagnostic> ContractViolation(const std::wstring &reason);

	private:
		static std::wstring RequireReason(const std::wstring &reason);
	};

	/// <summary>
	/// Immutable result contract required even when a completed instruction has no
	/// scalar result. Architectural-effect count is evidence only: this object does
	/// not own retire payloads or publication authority.
	/// </summary>
	class ExecutionResultContract final : public std::enable_shared_from_this<ExecutionResultContract>
	{
	private:
		const bool HasScalarResult;
		const unsigned long long ScalarResult;
		const int ArchitecturalEffectCount;

		ExecutionResultContract(bool hasScalarResult, unsigned long long scalarResult, int architecturalEffectCount);

	public:
		bool getHasScalarResult() const;
		unsigned long long getScalarResult() const;
		int getArchitecturalEffectCount() const;
		bool getHasArchitecturalEffects() const;

		static std::shared_ptr<ExecutionResultContract> WithoutScalarResult(int architecturalEffectCount = 0);

		static std::shared_ptr<ExecutionResultContract> Scalar(unsigned long long value, int architecturalEffectCount = 0);
	};

	/// <summary>
	/// Immutable, internally coherent terminal outcome payload.
	/// </summary>
	class ExecutionOutcome final : public std::enable_shared_from_this<ExecutionOutcome>
	{
	private:
		const ExecutionOutcomeKind Kind;
		const std::shared_ptr<ExecutionResultContract> Result;
		const std::shared_ptr<ExecutionDiagnostic> Diagnostic;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionOutcome(ExecutionOutcomeKind kind, ExecutionResultContract? result, System.Nullable<ExecutionDiagnostic> diagnostic)
		ExecutionOutcome(ExecutionOutcomeKind kind, std::shared_ptr<ExecutionResultContract> result, std::shared_ptr<ExecutionDiagnostic> diagnostic);

	public:
		ExecutionOutcomeKind getKind() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionResultContract? Result {get;}
		std::shared_ptr<ExecutionResultContract> getResult() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionDiagnostic? Diagnostic {get;}
		std::shared_ptr<ExecutionDiagnostic> getDiagnostic() const;
		bool getHasArchitecturalEffects() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ExecutionOutcome Create(ExecutionOutcomeKind kind, ExecutionResultContract? result = null, System.Nullable<ExecutionDiagnostic> diagnostic = null)
		static std::shared_ptr<ExecutionOutcome> Create(ExecutionOutcomeKind kind, std::shared_ptr<ExecutionResultContract> result = nullptr, std::shared_ptr<ExecutionDiagnostic> diagnostic = nullptr);

		static std::shared_ptr<ExecutionOutcome> Completed(std::shared_ptr<ExecutionResultContract> result);

		static std::shared_ptr<ExecutionOutcome> ArchitecturalFault(std::shared_ptr<ExecutionDiagnostic> diagnostic);

		static std::shared_ptr<ExecutionOutcome> Retryable(std::shared_ptr<ExecutionDiagnostic> diagnostic);

		static std::shared_ptr<ExecutionOutcome> StructuralBlocked(std::shared_ptr<ExecutionDiagnostic> diagnostic);

		static std::shared_ptr<ExecutionOutcome> BackendUnavailable(std::shared_ptr<ExecutionDiagnostic> diagnostic);

		static std::shared_ptr<ExecutionOutcome> FatalInvariantViolation(std::shared_ptr<ExecutionDiagnostic> diagnostic);

	private:
		static std::shared_ptr<ExecutionOutcomeContractViolationException> ContractViolation(const std::wstring &message);
	};

	/// <summary>
	/// Immutable terminal-transition command. It repeats identity only so an
	/// ExecutionRecord can fail closed on cross-attempt or reconstructed-binding use.
	/// </summary>
	class ExecutionTransition final : public std::enable_shared_from_this<ExecutionTransition>
	{
	private:
		const std::shared_ptr<VliwOperationId> OperationId;
		const std::shared_ptr<GeneratedStaticBinding> GeneratedBinding;
		const std::shared_ptr<ExecutionOutcome> Outcome;

	public:
		ExecutionTransition(std::shared_ptr<VliwOperationId> operationId, std::shared_ptr<GeneratedStaticBinding> generatedBinding, std::shared_ptr<ExecutionOutcome> outcome);

		std::shared_ptr<VliwOperationId> getOperationId() const;
		std::shared_ptr<GeneratedStaticBinding> getGeneratedBinding() const;
		std::shared_ptr<ExecutionOutcome> getOutcome() const;
	};

	enum class ExecutionRecordState : unsigned char
	{
		Issued = 1,
		Terminal = 2
	};

	/// <summary>
	/// One issued-attempt execution state owner. This is not a ROB entry and owns no
	/// rename allocation, physical-register lifetime, commit-map/free-list action,
	/// ordering, retirement selection, checkpoint, squash, or recovery authority.
	/// </summary>
	class ExecutionRecord final : public std::enable_shared_from_this<ExecutionRecord>
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> ScheduledOperation;
		const std::shared_ptr<VliwOperationId> OperationId;
		const std::shared_ptr<GeneratedStaticBinding> GeneratedBinding;

		mutable std::mutex _transitionGate;
		ExecutionRecordState _state = static_cast<ExecutionRecordState>(0);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionOutcome? _outcome;
		std::shared_ptr<ExecutionOutcome> _outcome;

		ExecutionRecord(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation);

	public:
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> getScheduledOperation() const;
		std::shared_ptr<VliwOperationId> getOperationId() const;
		std::shared_ptr<GeneratedStaticBinding> getGeneratedBinding() const;

		ExecutionRecordState getState() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionOutcome? Outcome
		std::shared_ptr<ExecutionOutcome> getOutcome() const;

		static std::shared_ptr<ExecutionRecord> Create(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation);

		std::shared_ptr<ExecutionTransition> CreateTerminalTransition(std::shared_ptr<ExecutionOutcome> outcome);

		void ApplyTerminalTransition(std::shared_ptr<ExecutionTransition> transition);
	};

	/// <summary>
	/// Additive RF-07.0 differential projection over legacy observations. Production
	/// execute routing does not call this surface in RF-07.0.
	/// </summary>
	class Rf07LegacyOutcomeProjection final
	{
	public:
		static std::shared_ptr<ExecutionOutcome> ProjectSuccessfulExecution(bool legacySuccess, std::shared_ptr<ExecutionResultContract> completedResult);

		static std::shared_ptr<ExecutionOutcome> ProjectKnownRetry(bool legacySuccess, const std::wstring &retryReason);

		static std::shared_ptr<ExecutionOutcome> ProjectKnownBackendUnavailable(bool legacySuccess, const std::wstring &denialReason);

		static std::shared_ptr<ExecutionOutcome> ProjectException(const std::exception& exception, bool alignmentIsWrite = true);
	};

	/// <summary>
	/// Fail-closed programming/invariant error raised when an outcome or transition
	/// violates the frozen RF-07 contract. It is not an architectural instruction fault.
	/// </summary>
	class ExecutionOutcomeContractViolationException final : public InvalidOperationException
	{
	public:
		ExecutionOutcomeContractViolationException(const std::wstring &message);
	};

}
