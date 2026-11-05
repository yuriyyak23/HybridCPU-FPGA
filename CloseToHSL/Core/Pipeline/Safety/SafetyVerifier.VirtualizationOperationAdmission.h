#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../Runtime/Events/Hypercalls/VirtualizationOperationDecisionTypes.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationOperationAdmissionCertificate;

	// Historical Phase-34 request remains fail-closed. In particular its mandatory
	// address-space/evidence booleans are not the PROBE_NO_STATE_V1 contract.
	enum class VirtualizationOperationAdmissionDecision : unsigned char
	{
		DeniedD2DecisionArtifact = 0,
		DeniedOwnerInterfaceDisabled = 1,
		DeniedCanonicalLeafCaptureMissing = 2,
		DeniedRuntimeIdentitiesMissing = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VirtualizationOperationAdmissionRequest(VirtualizationOperationDecisionManifest DecisionManifest, VirtualizationDecisionAttributionEvidence Attribution, bool CanonicalRuntimeLeafCaptured, bool CapabilityGrantIdentityPresent, bool EvidencePolicyIdentityPresent, bool AddressSpaceIdentityPresent, bool RestoreGenerationPresent);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct VirtualizationOperationAdmissionResult(VirtualizationOperationAdmissionDecision Decision, SafetyVerifier.VirtualizationOperationAdmissionCertificate? Certificate, string Reason)
//	internal readonly record struct VirtualizationOperationAdmissionResult(VirtualizationOperationAdmissionDecision Decision, SafetyVerifier.VirtualizationOperationAdmissionCertificate Certificate, string Reason)
//	{
//		internal bool IsIssued
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool BackendExecutionAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool CompletionPublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RetirePublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

    struct VirtualizationOperationAdmissionRequest final {
        VirtualizationOperationDecisionManifest DecisionManifest;
        VirtualizationDecisionAttributionEvidence Attribution;
        bool CanonicalRuntimeLeafCaptured = false, CapabilityGrantIdentityPresent = false;
        bool EvidencePolicyIdentityPresent = false, AddressSpaceIdentityPresent = false, RestoreGenerationPresent = false;
        bool operator==(const VirtualizationOperationAdmissionRequest&) const = default;
    };
    struct VirtualizationOperationAdmissionResult final {
        VirtualizationOperationAdmissionDecision Decision;
        std::shared_ptr<VirtualizationOperationAdmissionCertificate> Certificate;
        std::wstring Reason;
        bool IsIssued() const noexcept { return false; }
        bool BackendExecutionAuthorized() const noexcept { return false; }
        bool CompletionPublicationAuthorized() const noexcept { return false; }
        bool RetirePublicationAuthorized() const noexcept { return false; }
    };
	enum class VirtualizationE2State : unsigned char
	{
		Issued = 0,
		ConsumedByExecutor = 1,
		Revoked = 2
	};

	enum class VirtualizationE2Decision : unsigned char
	{
		Issued = 0,
		MissingInput = 1,
		InvalidD2OrOwnerPolicy = 2,
		InvalidE1 = 3,
		InvalidOperand = 4,
		DomainIdentityMismatch = 5,
		CapabilityLeaseNotLive = 6,
		CapabilityPolicyMismatch = 7,
		RootAuthorityMismatch = 8,
		CommonRuntimeAdmissionDenied = 9,
		RestoreGenerationMismatch = 10,
		DuplicateAttempt = 11,
		CertificateIssuerMismatch = 12,
		CertificateNotLive = 13,
		CertificateBindingMismatch = 14,
		CertificateDigestMismatch = 15,
		Revoked = 16,
		LifecycleGateDenied = 17
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct VirtualizationE2IssueRequest(ReplayPhaseContext ReplayPhase, SmtBundleMetadata4Way BundleMetadata, VmxMicroOp? Carrier, int SourceSlotId, int WorkingSlotId, System.Nullable<SafetyVerifier.VirtualizationAdmissionCertificate> E1, System.Nullable<VirtualizationOperationOwnerSnapshot> OwnerPolicy, System.Nullable<VirtualizationOperandSnapshot> Operand, System.Nullable<DomainRuntimeContext> DomainContext, System.Nullable<RootAuthorityDescriptor> RootAuthority, System.Nullable<RuntimeCapabilityGrantOwner> CapabilityOwner, System.Nullable<RuntimeCapabilityGrantLease> CapabilityLease, System.Nullable<VirtualizationRestoreGenerationOwner> RestoreOwner, System.Nullable<DomainHypercallLifecycleGate> LifecycleGate);
//	internal readonly record struct VirtualizationE2IssueRequest(ReplayPhaseContext ReplayPhase, SmtBundleMetadata4Way BundleMetadata, VmxMicroOp Carrier, int SourceSlotId, int WorkingSlotId, SafetyVerifier.VirtualizationAdmissionCertificate E1, VirtualizationOperationOwnerSnapshot OwnerPolicy, VirtualizationOperandSnapshot Operand, DomainRuntimeContext DomainContext, RootAuthorityDescriptor RootAuthority, RuntimeCapabilityGrantOwner CapabilityOwner, RuntimeCapabilityGrantLease CapabilityLease, VirtualizationRestoreGenerationOwner RestoreOwner, DomainHypercallLifecycleGate LifecycleGate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct VirtualizationE2Result(VirtualizationE2Decision Decision, SafetyVerifier.VirtualizationOperationAdmissionCertificate? Certificate, string Reason)
//	internal readonly record struct VirtualizationE2Result(VirtualizationE2Decision Decision, SafetyVerifier.VirtualizationOperationAdmissionCertificate Certificate, string Reason)
//	{
//		internal bool IsIssued
//		{
//			get
//			{
//				return Decision == VirtualizationE2Decision.Issued && Certificate != nullptr;
//			}
//		}
//		internal bool IsLive
//		{
//			get
//			{
//				return IsIssued;
//			}
//		}
//		internal bool BackendExecutionAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool CompletionPublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RetirePublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

    struct VirtualizationE2Result final {
        VirtualizationE2Decision Decision;
        std::shared_ptr<VirtualizationOperationAdmissionCertificate> Certificate;
        std::wstring Reason;
        bool IsIssued() const noexcept { return Decision == VirtualizationE2Decision::Issued && Certificate != nullptr; }
        bool IsLive() const noexcept { return IsIssued(); }
        bool BackendExecutionAuthorized() const noexcept { return false; }
        bool CompletionPublicationAuthorized() const noexcept { return false; }
        bool RetirePublicationAuthorized() const noexcept { return false; }
    };
	enum class VirtualizationE2ConsumptionDecision : unsigned char
	{
		Consumed = 0,
		InvalidConsumer = 1,
		InvalidCertificate = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VirtualizationE2ConsumptionResult(VirtualizationE2ConsumptionDecision Decision, string Reason)
//	{
//		internal bool IsConsumed
//		{
//			get
//			{
//				return Decision == VirtualizationE2ConsumptionDecision.Consumed;
//			}
//		}
//	}


    struct VirtualizationE2ConsumptionResult final {
        VirtualizationE2ConsumptionDecision Decision;
        std::wstring Reason;
        bool IsConsumed() const noexcept { return Decision == VirtualizationE2ConsumptionDecision::Consumed; }
    };
	class VirtualizationE2Digest final
	{
	private:
		static inline const std::vector<unsigned char> Envelope = {'H', 'C', 'P', 'U', 'E', '2', 'V', '2', 0};

	public:
		static std::wstring Compute(unsigned long long sequence, unsigned long long attemptId, unsigned long long e1Generation, int vt, int ownerContext, unsigned long long domain, unsigned long long bundle, unsigned long long replay, const std::wstring &decisionId, const std::wstring &specDigest, const std::wstring &acceptanceDigest, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring &operationNamespace, const std::wstring &operationId, unsigned short leaf, const std::wstring &ownerPolicyDigest, const std::wstring &operandDigest, unsigned long long grantIdentity, unsigned long long grantGeneration, unsigned long long rootEpoch, unsigned long long restoreGeneration);


	};

}
