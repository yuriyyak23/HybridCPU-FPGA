#include "SafetyVerifier.SecureComputeAdmission.h"

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeCanonicalAdmissionResult(ExternalSecureRegisterLocalRequest request, SecureComputeCanonicalAdmissionDecision decision, CanonicalBundle? canonical, System.Nullable<SecureComputePlacedIntent> placedIntent = null, System.Nullable<SecureComputeMachinePermitConsumption> machinePermitConsumption = null, System.Nullable<ExternalSecureExecutionAttempt> backendAttempt = null, System.Nullable<ExternalSecureExecutionConsumptionResult> backendConsumption = null, System.Nullable<ExternalSecureNoEffectSubmission> noEffectSubmission = null, System.Nullable<ExternalSecureNoEffectExecutionResult> noEffectOutcome = null, System.Nullable<SecureComputeNoEffectRetirement> noEffectRetirement = null, System.Nullable<ExternalSecureRegisterLocalSubmission> registerLocalSubmission = null, System.Nullable<ExternalSecureRegisterLocalExecutionResult> registerLocalOutcome = null, System.Nullable<SecureComputeRegisterLocalRetirement> registerLocalRetirement = null)
	SecureComputeCanonicalAdmissionResult::SecureComputeCanonicalAdmissionResult(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, SecureComputeCanonicalAdmissionDecision decision, std::shared_ptr<CanonicalBundle> canonical, std::shared_ptr<SecureComputePlacedIntent> placedIntent, std::shared_ptr<SecureComputeMachinePermitConsumption> machinePermitConsumption, std::optional<std::shared_ptr<ExternalSecureExecutionAttempt>> backendAttempt, std::optional<std::shared_ptr<ExternalSecureExecutionConsumptionResult>> backendConsumption, std::optional<std::shared_ptr<ExternalSecureNoEffectSubmission>> noEffectSubmission, std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> noEffectOutcome, std::shared_ptr<SecureComputeNoEffectRetirement> noEffectRetirement, std::optional<std::shared_ptr<ExternalSecureRegisterLocalSubmission>> registerLocalSubmission, std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> registerLocalOutcome, std::shared_ptr<SecureComputeRegisterLocalRetirement> registerLocalRetirement)
        : Request(std::move(request)), Decision(decision), Canonical(std::move(canonical)),
          PlacedIntent(std::move(placedIntent)), MachinePermitConsumption(std::move(machinePermitConsumption)),
          BackendAttempt(std::move(backendAttempt)), BackendConsumption(std::move(backendConsumption)),
          NoEffectSubmission(std::move(noEffectSubmission)), NoEffectOutcome(std::move(noEffectOutcome)),
          NoEffectRetirement(std::move(noEffectRetirement)), RegisterLocalSubmission(std::move(registerLocalSubmission)),
          RegisterLocalOutcome(std::move(registerLocalOutcome)), RegisterLocalRetirement(std::move(registerLocalRetirement))
    {}

	std::shared_ptr<ExternalSecureRegisterLocalRequest> SecureComputeCanonicalAdmissionResult::getRequest() const
	{
		return Request;
	}

	SecureComputeCanonicalAdmissionDecision SecureComputeCanonicalAdmissionResult::getDecision() const
	{
		return Decision;
	}

	std::shared_ptr<CanonicalBundle> SecureComputeCanonicalAdmissionResult::getCanonical() const
	{
		return Canonical;
	}

	std::shared_ptr<SecureComputePlacedIntent> SecureComputeCanonicalAdmissionResult::getPlacedIntent() const
	{
		return PlacedIntent;
	}

	std::shared_ptr<SecureComputeMachinePermitConsumption> SecureComputeCanonicalAdmissionResult::getMachinePermitConsumption() const
	{
		return MachinePermitConsumption;
	}

	std::optional<std::shared_ptr<ExternalSecureExecutionAttempt>> SecureComputeCanonicalAdmissionResult::getBackendAttempt() const
	{
		return BackendAttempt;
	}

	std::optional<std::shared_ptr<ExternalSecureExecutionConsumptionResult>> SecureComputeCanonicalAdmissionResult::getBackendConsumption() const
	{
		return BackendConsumption;
	}

	std::optional<std::shared_ptr<ExternalSecureNoEffectSubmission>> SecureComputeCanonicalAdmissionResult::getNoEffectSubmission() const
	{
		return NoEffectSubmission;
	}

	std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> SecureComputeCanonicalAdmissionResult::getNoEffectOutcome() const
	{
		return NoEffectOutcome;
	}

	std::shared_ptr<SecureComputeNoEffectRetirement> SecureComputeCanonicalAdmissionResult::getNoEffectRetirement() const
	{
		return NoEffectRetirement;
	}

	std::optional<std::shared_ptr<ExternalSecureRegisterLocalSubmission>> SecureComputeCanonicalAdmissionResult::getRegisterLocalSubmission() const
	{
		return RegisterLocalSubmission;
	}

	std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> SecureComputeCanonicalAdmissionResult::getRegisterLocalOutcome() const
	{
		return RegisterLocalOutcome;
	}

	std::shared_ptr<SecureComputeRegisterLocalRetirement> SecureComputeCanonicalAdmissionResult::getRegisterLocalRetirement() const
	{
		return RegisterLocalRetirement;
	}

	bool SecureComputeCanonicalAdmissionResult::isAllowed() const
	{
		return getDecision() == SecureComputeCanonicalAdmissionDecision::RegisterLocalRetired && getRegisterLocalRetirement() != nullptr;
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SecureComputeCanonicalAdmissionResult::RequireBackendCleanup()
	{
		return std::make_shared<SecureComputeCanonicalAdmissionResult>(getRequest(), SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, getCanonical(), getPlacedIntent(), getMachinePermitConsumption(), getBackendAttempt(), getBackendConsumption(), getNoEffectSubmission(), getNoEffectOutcome(), getNoEffectRetirement(), getRegisterLocalSubmission(), getRegisterLocalOutcome(), getRegisterLocalRetirement());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SecureComputeCanonicalAdmissionResult DeniedBackendCleanupRequired(ExternalSecureRegisterLocalRequest request, CanonicalBundle? canonical = null)
	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SecureComputeCanonicalAdmissionResult::DeniedBackendCleanupRequired(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<CanonicalBundle> canonical)
	{
		ArgumentNullException::ThrowIfNull(request);
		return std::make_shared<SecureComputeCanonicalAdmissionResult>(std::move(request), SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, std::move(canonical));
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SecureComputeCanonicalAdmissionResult::DeniedLocalBinding(std::shared_ptr<ExternalSecureRegisterLocalRequest> request)
	{
		ArgumentNullException::ThrowIfNull(request);
		return std::make_shared<SecureComputeCanonicalAdmissionResult>(std::move(request), SecureComputeCanonicalAdmissionDecision::DeniedLocalBinding, nullptr);
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SecureComputeCanonicalAdmissionResult::DeniedCompletedOperation(std::shared_ptr<ExternalSecureRegisterLocalRequest> request)
	{
		ArgumentNullException::ThrowIfNull(request);
		return std::make_shared<SecureComputeCanonicalAdmissionResult>(std::move(request), SecureComputeCanonicalAdmissionDecision::DeniedOperationCompleted, nullptr);
	}
}
