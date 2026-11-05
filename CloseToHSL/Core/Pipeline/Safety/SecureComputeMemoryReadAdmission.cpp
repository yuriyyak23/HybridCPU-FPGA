#include "SecureComputeMemoryReadAdmission.h"

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeMemoryReadMachineResult(ExternalSecureMemoryReadRequest request, SecureComputeMemoryReadDecision decision, CanonicalBundle? canonical = null, System.Nullable<ScheduledOperation> placement = null, System.Nullable<ExternalSecureMemoryReadAttempt> attempt = null, System.Nullable<ExternalSecureMemoryReadConsumptionResult> consumption = null, System.Nullable<ExternalSecureMemoryReadSubmission> submission = null, System.Nullable<ExternalSecureMemoryReadResult> outcome = null, System.Nullable<SecureComputeMemoryReadRetirement> retirement = null, System.Nullable<ulong> register3Value = null)
	SecureComputeMemoryReadMachineResult::SecureComputeMemoryReadMachineResult(std::shared_ptr<ExternalSecureMemoryReadRequest> request, SecureComputeMemoryReadDecision decision, std::shared_ptr<CanonicalBundle> canonical, std::shared_ptr<ScheduledOperation> placement, std::optional<std::shared_ptr<ExternalSecureMemoryReadAttempt>> attempt, std::optional<std::shared_ptr<ExternalSecureMemoryReadConsumptionResult>> consumption, std::optional<std::shared_ptr<ExternalSecureMemoryReadSubmission>> submission, std::optional<std::shared_ptr<ExternalSecureMemoryReadResult>> outcome, std::shared_ptr<SecureComputeMemoryReadRetirement> retirement, std::optional<unsigned long long> register3Value) : Request(std::move(request)), Decision(decision), Canonical(std::move(canonical)), Placement(std::move(placement)), Attempt(std::move(attempt)), Consumption(std::move(consumption)), Submission(std::move(submission)), Outcome(std::move(outcome)), Retirement(std::move(retirement)), Register3Value(register3Value) {}

	std::shared_ptr<ExternalSecureMemoryReadRequest> SecureComputeMemoryReadMachineResult::getRequest() const
	{
		return Request;
	}

	SecureComputeMemoryReadDecision SecureComputeMemoryReadMachineResult::getDecision() const
	{
		return Decision;
	}

	std::shared_ptr<CanonicalBundle> SecureComputeMemoryReadMachineResult::getCanonical() const
	{
		return Canonical;
	}

	std::shared_ptr<ScheduledOperation> SecureComputeMemoryReadMachineResult::getPlacement() const
	{
		return Placement;
	}

	std::optional<std::shared_ptr<ExternalSecureMemoryReadAttempt>> SecureComputeMemoryReadMachineResult::getAttempt() const
	{
		return Attempt;
	}

	std::optional<std::shared_ptr<ExternalSecureMemoryReadConsumptionResult>> SecureComputeMemoryReadMachineResult::getConsumption() const
	{
		return Consumption;
	}

	std::optional<std::shared_ptr<ExternalSecureMemoryReadSubmission>> SecureComputeMemoryReadMachineResult::getSubmission() const
	{
		return Submission;
	}

	std::optional<std::shared_ptr<ExternalSecureMemoryReadResult>> SecureComputeMemoryReadMachineResult::getOutcome() const
	{
		return Outcome;
	}

	std::shared_ptr<SecureComputeMemoryReadRetirement> SecureComputeMemoryReadMachineResult::getRetirement() const
	{
		return Retirement;
	}

	std::optional<unsigned long long> SecureComputeMemoryReadMachineResult::getRegister3Value() const
	{
		return Register3Value;
	}

	SecureComputeMemoryReadRetirement::SecureComputeMemoryReadRetirement(std::shared_ptr<ExternalSecureMemoryReadReceipt> receipt, unsigned long long acceptance, unsigned long long commit, unsigned long long value, std::shared_ptr<ScalarRegisterWriteRetireEffect> effect) : Receipt(std::move(receipt)), AcceptanceSequence(acceptance), CommitSequence(commit), Register3Value(value), Effect(std::move(effect)) {}

	std::shared_ptr<ExternalSecureMemoryReadReceipt> SecureComputeMemoryReadRetirement::getReceipt() const
	{
		return Receipt;
	}

	unsigned long long SecureComputeMemoryReadRetirement::getAcceptanceSequence() const
	{
		return AcceptanceSequence;
	}

	unsigned long long SecureComputeMemoryReadRetirement::getCommitSequence() const
	{
		return CommitSequence;
	}

	unsigned long long SecureComputeMemoryReadRetirement::getRegister3Value() const
	{
		return Register3Value;
	}

	std::shared_ptr<ScalarRegisterWriteRetireEffect> SecureComputeMemoryReadRetirement::getEffect() const
	{
		return Effect;
	}

	int SecureComputeMemoryReadRetirement::getEffectCount() const
	{
		return 1;
	}

	std::shared_ptr<SecureComputeMemoryReadCertificate> SecureComputeMemoryReadIssuePermit::getCertificate() const
	{
		return Certificate;
	}
}
