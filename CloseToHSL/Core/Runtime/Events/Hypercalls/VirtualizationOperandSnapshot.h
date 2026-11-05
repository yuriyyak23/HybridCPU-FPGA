#pragma once

#include "../../../Pipeline/Safety/VirtualizationAdmissionCertificate.h"
#include "Governance/VirtualizationOperationOwnerSnapshot.h"

#include <string>
#include <vector>
#include <mutex>
#include <set>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
    class VmxMicroOp;

	enum class VirtualizationOperandCaptureDecision : unsigned char
	{
		Captured = 0,
		MissingInput = 1,
		InvalidE1 = 2,
		NotExactVmCall = 3,
		InvalidRegisterShape = 4,
		InvalidLeafValue = 5,
		LeafHighBitsSet = 6,
		MissingDomainIdentity = 7,
		InvalidSlotIdentity = 8,
		MissingRestoreGeneration = 9,
		DuplicateAttempt = 10,
		OwnerPolicyMismatch = 11
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct VirtualizationOperandCaptureResult(VirtualizationOperandCaptureDecision Decision, VirtualizationOperandSnapshot? Snapshot, string Reason)
//	internal readonly record struct VirtualizationOperandCaptureResult(VirtualizationOperandCaptureDecision Decision, VirtualizationOperandSnapshot Snapshot, string Reason)
//	{
//		internal bool IsCaptured
//		{
//			get
//			{
//				return Decision == VirtualizationOperandCaptureDecision.Captured && Snapshot != nullptr;
//			}
//		}
//	}

	enum class VirtualizationOperandValidationDecision : unsigned char
	{
		ValidForE2Input = 0,
		MissingInput = 1,
		CarrierOrE1Mismatch = 2,
		OwnerPolicyMismatch = 3,
		RestoreGenerationMismatch = 4,
		IdentityMismatch = 5,
		DigestMismatch = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VirtualizationOperandValidationResult(VirtualizationOperandValidationDecision Decision, string Reason)
//	{
//		internal bool IsValidForE2Input
//		{
//			get
//			{
//				return Decision == VirtualizationOperandValidationDecision.ValidForE2Input;
//			}
//		}
//	}

    class VirtualizationOperandSnapshot;

    struct VirtualizationOperandCaptureResult final
    {
        const VirtualizationOperandCaptureDecision Decision;
        const std::shared_ptr<VirtualizationOperandSnapshot> Snapshot;
        const std::wstring Reason;

        VirtualizationOperandCaptureResult(VirtualizationOperandCaptureDecision decision,
            std::shared_ptr<VirtualizationOperandSnapshot> snapshot, std::wstring reason)
            : Decision(decision), Snapshot(std::move(snapshot)), Reason(std::move(reason)) {}
        bool isCaptured() const noexcept
        {
            return Decision == VirtualizationOperandCaptureDecision::Captured && Snapshot != nullptr;
        }
    };

    struct VirtualizationOperandValidationResult final
    {
        const VirtualizationOperandValidationDecision Decision;
        const std::wstring Reason;

        VirtualizationOperandValidationResult(VirtualizationOperandValidationDecision decision, std::wstring reason)
            : Decision(decision), Reason(std::move(reason)) {}
        bool isValidForE2Input() const noexcept
        {
            return Decision == VirtualizationOperandValidationDecision::ValidForE2Input;
        }
    };
	/// <summary>
	/// Immutable one-attempt operand identity captured after live E1 validation.
	/// It is not serializable authority and cannot authorize execution or publication.
	/// </summary>
	class VirtualizationOperandSnapshot final : public std::enable_shared_from_this<VirtualizationOperandSnapshot>
	{
	private:
		const unsigned long long AttemptId;
		const unsigned long long E1IssuerGeneration;
		const int VirtualThreadId;
		const int OwnerContextId;
		const unsigned long long DomainTag;
		const unsigned char Rs1Selector;
		const unsigned long long Rs1Value;
		const unsigned char Rs2Selector;
		const unsigned long long Rs2Value;
		const unsigned char RdSelector;
		const int SourceSlotId;
		const int WorkingSlotId;
		const unsigned long long BundleIdentity;
		const unsigned long long ReplayEpoch;
		const unsigned long long CarrierIdentityDigest;
		const unsigned long long RestoreGeneration;
		const unsigned long long CaptureSequence;
		const std::wstring OwnerPolicyDigest;
		const std::wstring OperandDigest;

		VirtualizationOperandSnapshot(unsigned long long attemptId, unsigned long long e1IssuerGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned char rs1Selector, unsigned long long rs1Value, unsigned char rs2Selector, unsigned long long rs2Value, unsigned char rdSelector, int sourceSlotId, int workingSlotId, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long carrierIdentityDigest, unsigned long long restoreGeneration, unsigned long long captureSequence, const std::wstring &ownerPolicyDigest, const std::wstring &operandDigest);

	public:
		unsigned int getSchemaVersion() const;
		unsigned long long getAttemptId() const;
		unsigned long long getE1IssuerGeneration() const;
		int getVirtualThreadId() const;
		int getOwnerContextId() const;
		unsigned long long getDomainTag() const;
		unsigned char getRs1Selector() const;
		unsigned long long getRs1Value() const;
		unsigned char getRs2Selector() const;
		unsigned long long getRs2Value() const;
		unsigned char getRdSelector() const;
		int getSourceSlotId() const;
		int getWorkingSlotId() const;
		unsigned long long getBundleIdentity() const;
		unsigned long long getReplayEpoch() const;
		unsigned long long getCarrierIdentityDigest() const;
		unsigned long long getRestoreGeneration() const;
		unsigned long long getCaptureSequence() const;
		std::wstring getOwnerPolicyDigest() const;
		std::wstring getOperandDigest() const;

		bool getBackendExecutionAuthorized() const;
		bool getCompletionPublicationAuthorized() const;
		bool getRetirePublicationAuthorized() const;

		static std::shared_ptr<VirtualizationOperandSnapshot> CreateValidated(std::shared_ptr<VirtualizationAdmissionCertificate> e1, unsigned char rs1Selector, unsigned long long rs1Value, unsigned long long restoreGeneration, unsigned long long captureSequence, std::shared_ptr<VirtualizationOperationOwnerSnapshot> ownerPolicy);
	};

	class VirtualizationOperandSnapshotDigest final
	{
	private:
		static inline const std::vector<unsigned char> Envelope = {'H', 'C', 'P', 'U', 'O', 'P', '1', 0};

	public:
		static std::wstring Compute(unsigned long long attemptId, unsigned long long issuerGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned char rs1Selector, unsigned long long rs1Value, unsigned char rs2Selector, unsigned long long rs2Value, unsigned char rdSelector, int sourceSlotId, int workingSlotId, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long carrierIdentityDigest, unsigned long long restoreGeneration, unsigned long long captureSequence, const std::wstring &ownerPolicyDigest);

	private:
		static void WriteUInt32(std::vector<unsigned char> &stream, unsigned int value);

		static void WriteUInt64(std::vector<unsigned char> &stream, unsigned long long value);
	};

	class VirtualizationOperandSnapshotMaterializer final : public std::enable_shared_from_this<VirtualizationOperandSnapshotMaterializer>
	{
	private:
		std::set<std::weak_ptr<VirtualizationAdmissionCertificate>, std::owner_less<std::weak_ptr<VirtualizationAdmissionCertificate>>> _captured; mutable std::mutex _captureGate;
		unsigned long long _nextCaptureSequence = 1;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationOperandCaptureResult CaptureAfterValidatedE1(VmxMicroOp? carrier, System.Nullable<SafetyVerifier.VirtualizationAdmissionCertificate> e1, ulong rs1Value, ulong restoreGeneration, System.Nullable<VirtualizationOperationOwnerSnapshot> ownerPolicy)
		std::shared_ptr<VirtualizationOperandCaptureResult> CaptureAfterValidatedE1(std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1, unsigned long long rs1Value, unsigned long long restoreGeneration, std::shared_ptr<VirtualizationOperationOwnerSnapshot> ownerPolicy);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static VirtualizationOperandValidationResult ValidateForE2Input(VirtualizationOperandSnapshot? snapshot, System.Nullable<VmxMicroOp> carrier, System.Nullable<SafetyVerifier.VirtualizationAdmissionCertificate> e1, System.Nullable<VirtualizationOperationOwnerSnapshot> ownerPolicy, ulong currentRestoreGeneration)
		static std::shared_ptr<VirtualizationOperandValidationResult> ValidateForE2Input(std::shared_ptr<VirtualizationOperandSnapshot> snapshot, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1, std::shared_ptr<VirtualizationOperationOwnerSnapshot> ownerPolicy, unsigned long long currentRestoreGeneration);

	private:
		unsigned long long AllocateCaptureSequence();

		static std::shared_ptr<VirtualizationOperandCaptureResult> Deny(VirtualizationOperandCaptureDecision decision, const std::wstring &reason);

		static std::shared_ptr<VirtualizationOperandValidationResult> Invalid(VirtualizationOperandValidationDecision decision, const std::wstring &reason);
	};

	class VirtualizationRestoreGenerationOwner final : public std::enable_shared_from_this<VirtualizationRestoreGenerationOwner>
	{
	private:
		mutable std::mutex _gate;
		unsigned long long _currentGeneration = 1;

	public:
		unsigned long long getCurrentGeneration() const;

		void AdvanceAfterRestore();
	};

}
