#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeNamedPositivePath : unsigned char
	{
		GuestCr0Cr4ReadOnlyProjection = 0,
		HypercallBackendOwnerProofOnly = 1,
		CompletionRetirePublicationGate = 2,
		MigrationOutputManifestClassification = 3,
		DebugAttestationVisibility = 4,
		FutureRestrictedRuntimeExecution = 5
	};

	enum class SecureComputeNamedPathVmxZeroAuthorityDecision : unsigned char
	{
		ZeroAuthorityClassified = 0,
		DeniedVmxActivation = 1,
		DeniedVmxCapsAuthority = 2,
		DeniedVmcsStateStore = 3,
		DeniedActiveVmcsPointerIdentity = 4,
		DeniedVmreadSecureStateAuthority = 5,
		DeniedVmwriteSecureStateMutation = 6,
		DeniedVmcsCheckpointAuthority = 7,
		DeniedCompletionPublication = 8,
		DeniedRetirePublication = 9,
		DeniedProjectionWithoutNeutralResult = 10
	};

    struct SecureComputeNamedPathVmxZeroAuthorityRequest final
    {
        SecureComputeNamedPositivePath Path;
        bool HasNeutralRuntimeResult = false, RequestsCompatibilityProjection = false;
        bool AttemptsVmxActivation = false, AttemptsVmxCapsGrant = false, AttemptsVmcsStateStore = false;
        bool AttemptsActiveVmcsPointerIdentity = false, AttemptsVmreadSecureStateAuthority = false;
        bool AttemptsVmwriteSecureStateMutation = false, AttemptsVmcsCheckpointAuthority = false;
        bool AttemptsCompletionPublication = false, AttemptsRetirePublication = false;
    };
    struct SecureComputeNamedPathVmxZeroAuthorityResult final
    {
        SecureComputeNamedPathVmxZeroAuthorityDecision Decision;
        SecureComputeNamedPositivePath Path;
        bool CompatibilityProjectionAllowed, VmxActivationAuthorized, VmxCapsAuthorityAuthorized;
        bool VmcsStateStoreAuthorized, ActiveVmcsPointerIdentityAuthorized, VmreadSecureStateAuthorityAuthorized;
        bool VmwriteSecureStateMutationAuthorized, VmcsCheckpointAuthorityAuthorized;
        bool CompletionPublicationAuthorized, RetirePublicationAuthorized;
        std::wstring Reason;
        bool IsAllowed() const noexcept { return Decision == SecureComputeNamedPathVmxZeroAuthorityDecision::ZeroAuthorityClassified; }
        bool CreatesAnyVmxAuthority() const noexcept
        {
            return VmxActivationAuthorized || VmxCapsAuthorityAuthorized || VmcsStateStoreAuthorized ||
                ActiveVmcsPointerIdentityAuthorized || VmreadSecureStateAuthorityAuthorized ||
                VmwriteSecureStateMutationAuthorized || VmcsCheckpointAuthorityAuthorized;
        }
        static std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> Classified(
            SecureComputeNamedPositivePath path, bool compatibilityProjectionAllowed, std::wstring reason)
        {
            return std::make_shared<SecureComputeNamedPathVmxZeroAuthorityResult>(SecureComputeNamedPathVmxZeroAuthorityResult{
                SecureComputeNamedPathVmxZeroAuthorityDecision::ZeroAuthorityClassified, path, compatibilityProjectionAllowed,
                false, false, false, false, false, false, false, false, false, std::move(reason)});
        }
        static std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> Denied(
            SecureComputeNamedPathVmxZeroAuthorityDecision decision, SecureComputeNamedPositivePath path, std::wstring reason)
        {
            return std::make_shared<SecureComputeNamedPathVmxZeroAuthorityResult>(SecureComputeNamedPathVmxZeroAuthorityResult{
                decision, path, false, false, false, false, false, false, false, false, false, false, std::move(reason)});
        }
    };
	class SecureComputeNamedPathVmxZeroAuthorityPolicy final : public std::enable_shared_from_this<SecureComputeNamedPathVmxZeroAuthorityPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityPolicy> FailClosed = std::make_shared<SecureComputeNamedPathVmxZeroAuthorityPolicy>();

	public:
		static std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityPolicy> getFailClosed();

		std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> Classify(std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityRequest> request);

	private:
		static std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> DenyAuthorityShortcut(std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityRequest> request);

		static std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> Deny(SecureComputeNamedPathVmxZeroAuthorityDecision decision, std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityRequest> request, const std::wstring &reason);
	};

}



