#include "Rf07ExecutionOutcomeContracts.h"

using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionDiagnostic(ExecutionOutcomeKind disposition, ExecutionDiagnosticCode code, string reason, System.Nullable<ulong> faultAddress, System.Nullable<bool> faultIsWrite, System.Nullable<ExecutionFaultCategory> legacyFaultCategory, string? exceptionType)
    ExecutionDiagnostic::ExecutionDiagnostic(ExecutionOutcomeKind disposition, ExecutionDiagnosticCode code, const std::wstring& reason, std::optional<unsigned long long> faultAddress, std::optional<bool> faultIsWrite, std::optional<ExecutionFaultCategory> legacyFaultCategory, const std::wstring& exceptionType)
        : Disposition(disposition), Code(code), Reason(RequireReason(reason)), FaultAddress(faultAddress), FaultIsWrite(faultIsWrite), LegacyFaultCategory(legacyFaultCategory), ExceptionType(exceptionType) {}
	ExecutionOutcomeKind ExecutionDiagnostic::getDisposition() const
	{
		return Disposition;
	}

	ExecutionDiagnosticCode ExecutionDiagnostic::getCode() const
	{
		return Code;
	}

	std::wstring ExecutionDiagnostic::getReason() const
	{
		return Reason;
	}

	std::optional<unsigned long long> ExecutionDiagnostic::getFaultAddress() const
	{
		return FaultAddress;
	}

	std::optional<bool> ExecutionDiagnostic::getFaultIsWrite() const
	{
		return FaultIsWrite;
	}

	std::optional<ExecutionFaultCategory> ExecutionDiagnostic::getLegacyFaultCategory() const
	{
		return LegacyFaultCategory;
	}

	std::wstring ExecutionDiagnostic::getExceptionType() const
	{
		return ExceptionType;
	}

	std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::PageFault(std::shared_ptr<PageFaultException> exception)
	{
		ArgumentNullException::ThrowIfNull(exception);
		return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::ArchitecturalFault, ExecutionDiagnosticCode::PageFault, std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(exception->what()), exception->getFaultAddress(), exception->isWrite(), std::nullopt, L"PageFaultException"));
	}

	std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::AlignmentFault(std::shared_ptr<MemoryAlignmentException> exception, bool isWrite)
	{
		ArgumentNullException::ThrowIfNull(exception);
		return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::ArchitecturalFault, ExecutionDiagnosticCode::AlignmentFault, std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(exception->what()), exception->getAddress(), isWrite, std::nullopt, L"MemoryAlignmentException"));
	}

	std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::Retryable(const std::wstring &reason)
	{
		std::optional<unsigned long long> faultAddress;
		std::optional<bool> faultIsWrite;
		std::optional<ExecutionFaultCategory> legacyFaultCategory;
		const std::wstring exceptionType;
		return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::Retryable, ExecutionDiagnosticCode::ResourceWait, reason, faultAddress, faultIsWrite, legacyFaultCategory, exceptionType));
	}

	std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::StructuralBlocked(const std::wstring &reason)
	{
		std::optional<unsigned long long> faultAddress;
		std::optional<bool> faultIsWrite;
		std::optional<ExecutionFaultCategory> legacyFaultCategory;
		const std::wstring exceptionType;
		return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::StructuralBlocked, ExecutionDiagnosticCode::StructuralHazard, reason, faultAddress, faultIsWrite, legacyFaultCategory, exceptionType));
	}

	std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::SpeculativeFaultSuppressed(const std::wstring &reason)
	{
		std::optional<unsigned long long> faultAddress;
		std::optional<bool> faultIsWrite;
		std::optional<ExecutionFaultCategory> legacyFaultCategory;
		const std::wstring exceptionType;
		return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::StructuralBlocked, ExecutionDiagnosticCode::SpeculativeFaultSuppressed, reason, faultAddress, faultIsWrite, legacyFaultCategory, exceptionType));
	}

	std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::BackendUnavailable(const std::wstring &reason)
	{
		std::optional<unsigned long long> faultAddress;
		std::optional<bool> faultIsWrite;
		std::optional<ExecutionFaultCategory> legacyFaultCategory;
		const std::wstring exceptionType;
		return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::BackendUnavailable, ExecutionDiagnosticCode::RuntimeBackendUnavailable, reason, faultAddress, faultIsWrite, legacyFaultCategory, exceptionType));
	}

    std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::Fatal(const std::exception& exception)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        const auto rawType = typeid(exception).name();
        const std::wstring exceptionType(rawType, rawType + std::char_traits<char>::length(rawType));
        std::wstring reason;
        try { reason = converter.from_bytes(exception.what()); }
        catch (const std::range_error&) { reason = L"Exception message is not valid UTF-8."; }
        if (StringHelper::isEmptyOrWhiteSpace(reason)) reason = std::format(L"Unhandled exception of type {}.", exceptionType);
        ExecutionFaultCategory legacyCategory;
        const bool hasLegacyCategory = ExecutionFaultContract::TryGetCategory(exception, legacyCategory);
        std::optional<ExecutionFaultCategory> legacy = hasLegacyCategory ? std::optional<ExecutionFaultCategory>(legacyCategory) : std::nullopt;
        std::optional<unsigned long long> address;
        std::optional<bool> isWrite;
        return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::FatalInvariantViolation, hasLegacyCategory ? ExecutionDiagnosticCode::ExistingExecutionFault : ExecutionDiagnosticCode::UnknownException, reason, address, isWrite, legacy, exceptionType));
    }
	std::shared_ptr<ExecutionDiagnostic> ExecutionDiagnostic::ContractViolation(const std::wstring &reason)
	{
		std::optional<unsigned long long> faultAddress;
		std::optional<bool> faultIsWrite;
		std::optional<ExecutionFaultCategory> legacyFaultCategory = ExecutionFaultCategory::InvalidInternalOp;
		const std::wstring exceptionType = L"YAKSys_Hybrid_CPU.Core.Execution.ExecutionOutcomeContractViolationException";
		return std::shared_ptr<ExecutionDiagnostic>(new ExecutionDiagnostic(ExecutionOutcomeKind::FatalInvariantViolation, ExecutionDiagnosticCode::OutcomeContractViolation, reason, faultAddress, faultIsWrite, legacyFaultCategory, exceptionType));
	}

	std::wstring ExecutionDiagnostic::RequireReason(const std::wstring &reason)
	{
		if (StringHelper::isEmptyOrWhiteSpace(reason)) throw std::invalid_argument("reason must not be empty");
		return StringHelper::trim(reason);
	}

	ExecutionResultContract::ExecutionResultContract(bool hasScalarResult, unsigned long long scalarResult, int architecturalEffectCount) : HasScalarResult(hasScalarResult), ScalarResult(scalarResult), ArchitecturalEffectCount(architecturalEffectCount)
	{
		if (architecturalEffectCount < 0)
		{
			throw ArgumentOutOfRangeException(L"architecturalEffectCount");
		}


	}

	bool ExecutionResultContract::getHasScalarResult() const
	{
		return HasScalarResult;
	}

	unsigned long long ExecutionResultContract::getScalarResult() const
	{
		return ScalarResult;
	}

	int ExecutionResultContract::getArchitecturalEffectCount() const
	{
		return ArchitecturalEffectCount;
	}

	bool ExecutionResultContract::getHasArchitecturalEffects() const
	{
		return getArchitecturalEffectCount() != 0;
	}

	std::shared_ptr<ExecutionResultContract> ExecutionResultContract::WithoutScalarResult(int architecturalEffectCount)
	{
		return std::shared_ptr<ExecutionResultContract>(new ExecutionResultContract(false, 0, architecturalEffectCount));
	}

	std::shared_ptr<ExecutionResultContract> ExecutionResultContract::Scalar(unsigned long long value, int architecturalEffectCount)
	{
		return std::shared_ptr<ExecutionResultContract>(new ExecutionResultContract(true, value, architecturalEffectCount));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionOutcome(ExecutionOutcomeKind kind, ExecutionResultContract? result, System.Nullable<ExecutionDiagnostic> diagnostic)
	ExecutionOutcome::ExecutionOutcome(ExecutionOutcomeKind kind, std::shared_ptr<ExecutionResultContract> result, std::shared_ptr<ExecutionDiagnostic> diagnostic) : Kind(kind), Result(std::move(result)), Diagnostic(std::move(diagnostic)) {}

	ExecutionOutcomeKind ExecutionOutcome::getKind() const
	{
		return Kind;
	}

	std::shared_ptr<ExecutionResultContract> ExecutionOutcome::getResult() const
	{
		return Result;
	}

	std::shared_ptr<ExecutionDiagnostic> ExecutionOutcome::getDiagnostic() const
	{
		return Diagnostic;
	}

	bool ExecutionOutcome::getHasArchitecturalEffects() const
	{
		return getResult() && getResult()->getHasArchitecturalEffects();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ExecutionOutcome Create(ExecutionOutcomeKind kind, ExecutionResultContract? result = null, System.Nullable<ExecutionDiagnostic> diagnostic = null)
	std::shared_ptr<ExecutionOutcome> ExecutionOutcome::Create(ExecutionOutcomeKind kind, std::shared_ptr<ExecutionResultContract> result, std::shared_ptr<ExecutionDiagnostic> diagnostic)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		const auto kindValue = static_cast<unsigned char>(kind);
		if (kindValue < static_cast<unsigned char>(ExecutionOutcomeKind::Completed) ||
			kindValue > static_cast<unsigned char>(ExecutionOutcomeKind::FatalInvariantViolation))
		{
			throw *ContractViolation(std::format(L"Unknown execution outcome kind value {}.", static_cast<unsigned int>(kind)));
		}

		if (kind == ExecutionOutcomeKind::Completed)
		{
			if (result == nullptr)
			{
				throw *ContractViolation(L"Completed requires an explicit ExecutionResultContract.");
			}

			if (diagnostic != nullptr)
			{
				throw *ContractViolation(L"Completed cannot carry a fault/block/retry diagnostic.");
			}

			return std::shared_ptr<ExecutionOutcome>(new ExecutionOutcome(kind, result, nullptr));
		}

		if (result != nullptr)
		{
			std::wstring effectDetail = result->getHasArchitecturalEffects() ? L" with architectural effects" : L"";
			throw *ContractViolation(std::format(L"Outcome {} cannot carry a result contract{}.", static_cast<unsigned int>(kind), effectDetail));
		}

		if (diagnostic == nullptr)
		{
			throw *ContractViolation(std::format(L"Outcome {} requires a typed ExecutionDiagnostic.", static_cast<unsigned int>(kind)));
		}

		if (diagnostic->getDisposition() != kind)
		{
			throw *ContractViolation(std::format(L"Diagnostic {} has disposition {} and cannot be used as {}.", static_cast<unsigned int>(diagnostic->getCode()), static_cast<unsigned int>(diagnostic->getDisposition()), static_cast<unsigned int>(kind)));
		}

		return std::shared_ptr<ExecutionOutcome>(new ExecutionOutcome(kind, nullptr, diagnostic));
	}

	std::shared_ptr<ExecutionOutcome> ExecutionOutcome::Completed(std::shared_ptr<ExecutionResultContract> result)
	{
		return Create(ExecutionOutcomeKind::Completed, result);
	}

	std::shared_ptr<ExecutionOutcome> ExecutionOutcome::ArchitecturalFault(std::shared_ptr<ExecutionDiagnostic> diagnostic)
	{
		return Create(ExecutionOutcomeKind::ArchitecturalFault, nullptr, diagnostic);
	}

	std::shared_ptr<ExecutionOutcome> ExecutionOutcome::Retryable(std::shared_ptr<ExecutionDiagnostic> diagnostic)
	{
		return Create(ExecutionOutcomeKind::Retryable, nullptr, diagnostic);
	}

	std::shared_ptr<ExecutionOutcome> ExecutionOutcome::StructuralBlocked(std::shared_ptr<ExecutionDiagnostic> diagnostic)
	{
		return Create(ExecutionOutcomeKind::StructuralBlocked, nullptr, diagnostic);
	}

	std::shared_ptr<ExecutionOutcome> ExecutionOutcome::BackendUnavailable(std::shared_ptr<ExecutionDiagnostic> diagnostic)
	{
		return Create(ExecutionOutcomeKind::BackendUnavailable, nullptr, diagnostic);
	}

	std::shared_ptr<ExecutionOutcome> ExecutionOutcome::FatalInvariantViolation(std::shared_ptr<ExecutionDiagnostic> diagnostic)
	{
		return Create(ExecutionOutcomeKind::FatalInvariantViolation, nullptr, diagnostic);
	}

	std::shared_ptr<ExecutionOutcomeContractViolationException> ExecutionOutcome::ContractViolation(const std::wstring &message)
	{
		return std::make_shared<ExecutionOutcomeContractViolationException>(message);
	}

    ExecutionTransition::ExecutionTransition(std::shared_ptr<VliwOperationId> operationId, std::shared_ptr<GeneratedStaticBinding> generatedBinding, std::shared_ptr<ExecutionOutcome> outcome)
        : OperationId(std::move(operationId)), GeneratedBinding(std::move(generatedBinding)), Outcome(std::move(outcome)) {
        ArgumentNullException::ThrowIfNull(GeneratedBinding);
        ArgumentNullException::ThrowIfNull(Outcome);
    }
	std::shared_ptr<VliwOperationId> ExecutionTransition::getOperationId() const
	{
		return OperationId;
	}

	std::shared_ptr<GeneratedStaticBinding> ExecutionTransition::getGeneratedBinding() const
	{
		return GeneratedBinding;
	}

	std::shared_ptr<ExecutionOutcome> ExecutionTransition::getOutcome() const
	{
		return Outcome;
	}

    ExecutionRecord::ExecutionRecord(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation)
        : ScheduledOperation(scheduledOperation), OperationId(scheduledOperation->getOperationId()), GeneratedBinding(scheduledOperation->getAdmission()->Contract->getGeneratedBinding()), _state(ExecutionRecordState::Issued) {}
	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> ExecutionRecord::getScheduledOperation() const
	{
		return ScheduledOperation;
	}

	std::shared_ptr<VliwOperationId> ExecutionRecord::getOperationId() const
	{
		return OperationId;
	}

	std::shared_ptr<GeneratedStaticBinding> ExecutionRecord::getGeneratedBinding() const
	{
		return GeneratedBinding;
	}

	ExecutionRecordState ExecutionRecord::getState() const
	{
		{
		std::scoped_lock<std::mutex> lock(_transitionGate);
			return _state;
		}
	}

	std::shared_ptr<ExecutionOutcome> ExecutionRecord::getOutcome() const
	{
		{
		std::scoped_lock<std::mutex> lock(_transitionGate);
			return _outcome;
		}
	}

	std::shared_ptr<ExecutionRecord> ExecutionRecord::Create(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation)
	{
		ArgumentNullException::ThrowIfNull(scheduledOperation);
		return std::shared_ptr<ExecutionRecord>(new ExecutionRecord(scheduledOperation));
	}

	std::shared_ptr<ExecutionTransition> ExecutionRecord::CreateTerminalTransition(std::shared_ptr<ExecutionOutcome> outcome)
	{
		ArgumentNullException::ThrowIfNull(outcome);
		return std::make_shared<ExecutionTransition>(getOperationId(), getGeneratedBinding(), outcome);
	}

	void ExecutionRecord::ApplyTerminalTransition(std::shared_ptr<ExecutionTransition> transition)
	{
		ArgumentNullException::ThrowIfNull(transition);
		{
			std::scoped_lock<std::mutex> lock(_transitionGate);
			if (_state == ExecutionRecordState::Terminal)
			{
				throw ExecutionOutcomeContractViolationException(L"ExecutionRecord already has a terminal outcome; duplicate or post-terminal transition rejected.");
			}

			if (transition->getOperationId() != getOperationId())
			{
				throw ExecutionOutcomeContractViolationException(L"Execution transition operation identity does not match the issued attempt.");
			}

			if (transition->getGeneratedBinding() != getGeneratedBinding())
			{
				throw ExecutionOutcomeContractViolationException(L"Execution transition does not carry the exact frozen GeneratedStaticBinding instance from ScheduledOperation.");
			}

			_outcome = transition->getOutcome();
			_state = ExecutionRecordState::Terminal;
		}
	}

	std::shared_ptr<ExecutionOutcome> Rf07LegacyOutcomeProjection::ProjectSuccessfulExecution(bool legacySuccess, std::shared_ptr<ExecutionResultContract> completedResult)
	{
		ArgumentNullException::ThrowIfNull(completedResult);
		if (!legacySuccess)
		{
			throw ExecutionOutcomeContractViolationException(L"A false legacy observation requires contour-specific classification; it cannot be projected as generic success/retry.");
		}

		return ExecutionOutcome::Completed(completedResult);
	}

	std::shared_ptr<ExecutionOutcome> Rf07LegacyOutcomeProjection::ProjectKnownRetry(bool legacySuccess, const std::wstring &retryReason)
	{
		if (legacySuccess)
		{
			throw ExecutionOutcomeContractViolationException(L"A completed legacy observation cannot be projected as Retryable.");
		}

		return ExecutionOutcome::Retryable(ExecutionDiagnostic::Retryable(retryReason));
	}

	std::shared_ptr<ExecutionOutcome> Rf07LegacyOutcomeProjection::ProjectKnownBackendUnavailable(bool legacySuccess, const std::wstring &denialReason)
	{
		if (legacySuccess)
		{
			throw ExecutionOutcomeContractViolationException(L"A completed legacy observation cannot be projected as BackendUnavailable.");
		}

		return ExecutionOutcome::BackendUnavailable(ExecutionDiagnostic::BackendUnavailable(denialReason));
	}

    std::shared_ptr<ExecutionOutcome> Rf07LegacyOutcomeProjection::ProjectException(const std::exception& exception, bool alignmentIsWrite)
    {
        if (const auto* pageFault = dynamic_cast<const PageFaultException*>(&exception))
            return ExecutionOutcome::ArchitecturalFault(ExecutionDiagnostic::PageFault(std::make_shared<PageFaultException>(*pageFault)));
        if (const auto* alignmentFault = dynamic_cast<const MemoryAlignmentException*>(&exception))
            return ExecutionOutcome::ArchitecturalFault(ExecutionDiagnostic::AlignmentFault(std::make_shared<MemoryAlignmentException>(*alignmentFault), alignmentIsWrite));
        return ExecutionOutcome::FatalInvariantViolation(ExecutionDiagnostic::Fatal(exception));
    }

#ifdef FormatMessage
#undef FormatMessage
#endif
	ExecutionOutcomeContractViolationException::ExecutionOutcomeContractViolationException(const std::wstring &message) : InvalidOperationException(ExecutionFaultContract::FormatMessage(ExecutionFaultCategory::InvalidInternalOp, message))
	{
        ExecutionFaultContract::Stamp(*this, ExecutionFaultCategory::InvalidInternalOp);
	}
}
