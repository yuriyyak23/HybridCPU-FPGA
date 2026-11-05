#pragma once

#include <string>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeCompilerEmissionPath : unsigned char
	{
		NoCompilerChange = 0,
		SecureBackendHelper = 1,
		SecureHypercallHelper = 2,
		SecureSidebandMetadata = 3,
		FutureControlledEmission = 4
	};

	enum class SecureComputeControlledEmissionDecision : unsigned char
	{
		NoEmissionPreserved = 0,
		DeniedMissingPositiveRuntimeOwner = 1,
		DeniedMissingControlledEmissionRfc = 2,
		DeniedMissingReleaseApproval = 3,
		DeniedBackendExecutionClosed = 4,
		DeniedNewInstructionEncoding = 5,
		DeniedNewOperandFormat = 6,
		DeniedCapabilityAwareLoadStoreFetch = 7,
		DeniedVmxSecureModeEmission = 8,
		DeniedCompilerEmissionFutureGated = 9
	};

struct SecureComputeControlledEmissionRequest final {
    SecureComputeCompilerEmissionPath Path = SecureComputeCompilerEmissionPath::NoCompilerChange;
    bool RequestsCompilerEmission = false;
    bool HasPositiveNeutralRuntimeOwner = false;
    bool HasControlledEmissionRfc = false;
    bool HasReleaseApproval = false;
    bool BackendExecutionAuthorized = false;
    bool EmitsNewInstructionEncoding = false;
    bool EmitsNewOperandFormat = false;
    bool EmitsCapabilityAwareLoadStoreFetch = false;
    bool EmitsVmxSecureMode = false;
};

struct SecureComputeControlledEmissionResult final {
    SecureComputeControlledEmissionDecision Decision;
    SecureComputeCompilerEmissionPath Path;
    bool CompilerEmissionAuthorized;
    bool BackendExecutionAuthorized;
    bool NewInstructionEncodingAuthorized;
    bool NewOperandFormatAuthorized;
    bool CapabilityAwareLoadStoreFetchAuthorized;
    bool VmxSecureModeEmissionAuthorized;
    std::wstring Reason;
    bool IsAllowed() const noexcept { return Decision == SecureComputeControlledEmissionDecision::NoEmissionPreserved; }
    bool CreatesAnyEmissionAuthority() const noexcept {
        return CompilerEmissionAuthorized || BackendExecutionAuthorized || NewInstructionEncodingAuthorized ||
            NewOperandFormatAuthorized || CapabilityAwareLoadStoreFetchAuthorized || VmxSecureModeEmissionAuthorized;
    }

    SecureComputeControlledEmissionResult(SecureComputeControlledEmissionDecision decision,
        SecureComputeCompilerEmissionPath path, bool compilerAuthorized, bool backendAuthorized,
        bool instructionAuthorized, bool operandAuthorized, bool loadStoreAuthorized,
        bool vmxAuthorized, std::wstring reason)
        : Decision(decision), Path(path), CompilerEmissionAuthorized(compilerAuthorized),
          BackendExecutionAuthorized(backendAuthorized), NewInstructionEncodingAuthorized(instructionAuthorized),
          NewOperandFormatAuthorized(operandAuthorized), CapabilityAwareLoadStoreFetchAuthorized(loadStoreAuthorized),
          VmxSecureModeEmissionAuthorized(vmxAuthorized), Reason(std::move(reason)) {}

    static std::shared_ptr<SecureComputeControlledEmissionResult> NoEmissionPreserved(SecureComputeCompilerEmissionPath path, const std::wstring& reason) {
        return std::make_shared<SecureComputeControlledEmissionResult>(SecureComputeControlledEmissionDecision::NoEmissionPreserved, path, false, false, false, false, false, false, reason);
    }
    static std::shared_ptr<SecureComputeControlledEmissionResult> Denied(SecureComputeControlledEmissionDecision decision, SecureComputeCompilerEmissionPath path, const std::wstring& reason) {
        return std::make_shared<SecureComputeControlledEmissionResult>(decision, path, false, false, false, false, false, false, reason);
    }
};

	class SecureComputeControlledEmissionGatePolicy final : public std::enable_shared_from_this<SecureComputeControlledEmissionGatePolicy>
	{
	private:
		static inline const std::shared_ptr<SecureComputeControlledEmissionGatePolicy> FailClosed = std::make_shared<SecureComputeControlledEmissionGatePolicy>();

	public:
		static std::shared_ptr<SecureComputeControlledEmissionGatePolicy> getFailClosed();

		std::shared_ptr<SecureComputeControlledEmissionResult> Classify(std::shared_ptr<SecureComputeControlledEmissionRequest> request);

	private:
		static std::shared_ptr<SecureComputeControlledEmissionResult> DenyNoEmissionViolation(std::shared_ptr<SecureComputeControlledEmissionRequest> request);

		static std::shared_ptr<SecureComputeControlledEmissionResult> Deny(SecureComputeControlledEmissionDecision decision, std::shared_ptr<SecureComputeControlledEmissionRequest> request, const std::wstring &reason);
	};

}
