#pragma once

#include "../../Decoder/CanonicalDecodedContracts.h"
#include "SecureComputePlacedIntent.h"
#include "SecureComputeIssuePermitOwner.h"
#include "../../Runtime/Completion/Records/SecureComputeNoEffectCompletionOwner.h"
#include "../../Runtime/Completion/Records/SecureComputeRegisterLocalCompletionOwner.h"
#include <optional>
#include <memory>
#include <utility>

namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureRegisterLocalRequest; class ExternalSecureExecutionAttempt; class ExternalSecureExecutionConsumptionResult; class ExternalSecureNoEffectSubmission; class ExternalSecureNoEffectExecutionResult; class ExternalSecureRegisterLocalSubmission; class ExternalSecureRegisterLocalExecutionResult; }
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeMachinePermitConsumption;
	class SecureComputeNoEffectRetirement;
	class SecureComputeRegisterLocalRetirement;

	enum class SecureComputeCanonicalAdmissionDecision : unsigned char
	{
		DeniedUnknown = 0,
		DeniedLocalBinding = 1,
		DeniedCanonicalDecode = 2,
		DeniedMachineChainUnavailable = 3,
		DeniedOperationCompleted = 4,
		DeniedBackendCleanupRequired = 5,
		DeniedMachinePlacement = 6,
		DeniedSecurePermitUnavailable = 7,
		DeniedBackendEntitlementUnavailable = 8,
		DeniedExecutionTailUnavailable = 9,
		NoEffectRetired = 10,
		DeniedNoEffectCompletion = 11,
		RegisterLocalRetired = 12,
		DeniedRegisterLocalCompletion = 13
	};

	/// <summary>Denial or historical bounded retirement/result facts; no live token or OS publication.</summary>
	class SecureComputeCanonicalAdmissionResult final : public std::enable_shared_from_this<SecureComputeCanonicalAdmissionResult>
	{
	private:
		const std::shared_ptr<ExternalSecureRegisterLocalRequest> Request;
		const SecureComputeCanonicalAdmissionDecision Decision;
		const std::shared_ptr<CanonicalBundle> Canonical;
		const std::shared_ptr<SecureComputePlacedIntent> PlacedIntent;
		const std::shared_ptr<SecureComputeMachinePermitConsumption> MachinePermitConsumption;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureExecutionAttempt> BackendAttempt;
		std::optional<std::shared_ptr<ExternalSecureExecutionAttempt>> BackendAttempt;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureExecutionConsumptionResult> BackendConsumption;
		std::optional<std::shared_ptr<ExternalSecureExecutionConsumptionResult>> BackendConsumption;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureNoEffectSubmission> NoEffectSubmission;
		std::optional<std::shared_ptr<ExternalSecureNoEffectSubmission>> NoEffectSubmission;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureNoEffectExecutionResult> NoEffectOutcome;
		std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> NoEffectOutcome;
		const std::shared_ptr<SecureComputeNoEffectRetirement> NoEffectRetirement;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureRegisterLocalSubmission> RegisterLocalSubmission;
		std::optional<std::shared_ptr<ExternalSecureRegisterLocalSubmission>> RegisterLocalSubmission;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureRegisterLocalExecutionResult> RegisterLocalOutcome;
		std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> RegisterLocalOutcome;
		const std::shared_ptr<SecureComputeRegisterLocalRetirement> RegisterLocalRetirement;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeCanonicalAdmissionResult(ExternalSecureRegisterLocalRequest request, SecureComputeCanonicalAdmissionDecision decision, CanonicalBundle? canonical, System.Nullable<SecureComputePlacedIntent> placedIntent = null, System.Nullable<SecureComputeMachinePermitConsumption> machinePermitConsumption = null, System.Nullable<ExternalSecureExecutionAttempt> backendAttempt = null, System.Nullable<ExternalSecureExecutionConsumptionResult> backendConsumption = null, System.Nullable<ExternalSecureNoEffectSubmission> noEffectSubmission = null, System.Nullable<ExternalSecureNoEffectExecutionResult> noEffectOutcome = null, System.Nullable<SecureComputeNoEffectRetirement> noEffectRetirement = null, System.Nullable<ExternalSecureRegisterLocalSubmission> registerLocalSubmission = null, System.Nullable<ExternalSecureRegisterLocalExecutionResult> registerLocalOutcome = null, System.Nullable<SecureComputeRegisterLocalRetirement> registerLocalRetirement = null)
		SecureComputeCanonicalAdmissionResult(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, SecureComputeCanonicalAdmissionDecision decision, std::shared_ptr<CanonicalBundle> canonical, std::shared_ptr<SecureComputePlacedIntent> placedIntent = nullptr, std::shared_ptr<SecureComputeMachinePermitConsumption> machinePermitConsumption = nullptr, std::optional<std::shared_ptr<ExternalSecureExecutionAttempt>> backendAttempt = std::nullopt, std::optional<std::shared_ptr<ExternalSecureExecutionConsumptionResult>> backendConsumption = std::nullopt, std::optional<std::shared_ptr<ExternalSecureNoEffectSubmission>> noEffectSubmission = std::nullopt, std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> noEffectOutcome = std::nullopt, std::shared_ptr<SecureComputeNoEffectRetirement> noEffectRetirement = nullptr, std::optional<std::shared_ptr<ExternalSecureRegisterLocalSubmission>> registerLocalSubmission = std::nullopt, std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> registerLocalOutcome = std::nullopt, std::shared_ptr<SecureComputeRegisterLocalRetirement> registerLocalRetirement = nullptr);

		std::shared_ptr<ExternalSecureRegisterLocalRequest> getRequest() const;
		SecureComputeCanonicalAdmissionDecision getDecision() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CanonicalBundle? Canonical {get;}
		std::shared_ptr<CanonicalBundle> getCanonical() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputePlacedIntent? PlacedIntent {get;}
		std::shared_ptr<SecureComputePlacedIntent> getPlacedIntent() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeMachinePermitConsumption? MachinePermitConsumption {get;}
		std::shared_ptr<SecureComputeMachinePermitConsumption> getMachinePermitConsumption() const;
		std::optional<std::shared_ptr<ExternalSecureExecutionAttempt>> getBackendAttempt() const;
		std::optional<std::shared_ptr<ExternalSecureExecutionConsumptionResult>> getBackendConsumption() const;
		std::optional<std::shared_ptr<ExternalSecureNoEffectSubmission>> getNoEffectSubmission() const;
		std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> getNoEffectOutcome() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeNoEffectRetirement? NoEffectRetirement {get;}
		std::shared_ptr<SecureComputeNoEffectRetirement> getNoEffectRetirement() const;
		std::optional<std::shared_ptr<ExternalSecureRegisterLocalSubmission>> getRegisterLocalSubmission() const;
		std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> getRegisterLocalOutcome() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeRegisterLocalRetirement? RegisterLocalRetirement {get;}
		std::shared_ptr<SecureComputeRegisterLocalRetirement> getRegisterLocalRetirement() const;
		bool isAllowed() const;

		/// <summary>Preserves known original facts when independent backend cleanup is unresolved.</summary>
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> RequireBackendCleanup();

		/// <summary>
		/// Denial with an unresolved provider reservation obligation. The original
		/// request is required for cleanup-only reconciliation, never admission retry.
		/// Canonical diagnostics, if present, confer no issue/publication authority.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SecureComputeCanonicalAdmissionResult DeniedBackendCleanupRequired(ExternalSecureRegisterLocalRequest request, CanonicalBundle? canonical = null)
		static std::shared_ptr<SecureComputeCanonicalAdmissionResult> DeniedBackendCleanupRequired(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<CanonicalBundle> canonical = nullptr);

		static std::shared_ptr<SecureComputeCanonicalAdmissionResult> DeniedLocalBinding(std::shared_ptr<ExternalSecureRegisterLocalRequest> request);

		static std::shared_ptr<SecureComputeCanonicalAdmissionResult> DeniedCompletedOperation(std::shared_ptr<ExternalSecureRegisterLocalRequest> request);
	};


}
