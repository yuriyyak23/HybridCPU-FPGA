#pragma once

#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include <string>
#include <unordered_set>
#include <any>
#include <mutex>
#include <memory>

#if defined(TESTING)

namespace YAKSys_Hybrid_CPU::Core
{

	enum class ResearchVirtualizationProbeOperation : unsigned char
	{
		None = 0,
		StateMinimalLiveness = 1
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct ResearchVirtualizationProbeIdentity(int VirtualThreadId, int OwnerContextId, ulong DomainTag, ulong AddressSpaceTag, ulong CarrierAttemptId, ulong ReplayEpoch, ulong CapabilityGeneration, ulong EvidenceGeneration, ulong RestoreGeneration)
//	{
//		internal bool IsComplete
//		{
//			get
//			{
//				return VirtualThreadId >= 0 && OwnerContextId >= 0 && DomainTag != 0 && AddressSpaceTag != 0 && CarrierAttemptId != 0 && ReplayEpoch != 0 && CapabilityGeneration != 0 && EvidenceGeneration != 0 && RestoreGeneration != 0;
//			}
//		}
//	}

	enum class ResearchVirtualizationProbeAdmissionDecision : unsigned char
	{
		IssuedForResearchExecution = 0,
		DeniedUnsupportedOperation = 1,
		DeniedIncompleteIdentity = 2,
		DeniedE1Carrier = 3,
		DeniedCarrierIdentityMismatch = 4,
		DeniedRuntimeIdentitySnapshot = 5
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct ResearchVirtualizationProbeAdmissionResult(ResearchVirtualizationProbeAdmissionDecision Decision, SafetyVerifier.ResearchVirtualizationOperationAdmissionCertificate? Certificate, string Reason)
//	internal readonly record struct ResearchVirtualizationProbeAdmissionResult(ResearchVirtualizationProbeAdmissionDecision Decision, SafetyVerifier.ResearchVirtualizationOperationAdmissionCertificate Certificate, string Reason)
//	{
//		internal bool IsIssued
//		{
//			get
//			{
//				return Decision == ResearchVirtualizationProbeAdmissionDecision.IssuedForResearchExecution && Certificate != nullptr;
//			}
//		}
//	}

	enum class ResearchVirtualizationProbeExecutionDecision : unsigned char
	{
		Executed = 0,
		DeniedForeignOwner = 1,
		DeniedStalePolicyGeneration = 2,
		DeniedDuplicateAttempt = 3,
		DeniedStaleRuntimeContext = 4,
		DeniedStaleAdmission = 5
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct ResearchVirtualizationProbeExecutionResult(ResearchVirtualizationProbeExecutionDecision Decision, ResearchVirtualizationRuntimeOwner.ExecutionReceipt? Receipt, string Reason)
//	internal readonly record struct ResearchVirtualizationProbeExecutionResult(ResearchVirtualizationProbeExecutionDecision Decision, ResearchVirtualizationRuntimeOwner.ExecutionReceipt Receipt, string Reason)
//	{
//		internal bool Succeeded
//		{
//			get
//			{
//				return Decision == ResearchVirtualizationProbeExecutionDecision.Executed && Receipt != nullptr;
//			}
//		}
//	}

	/// <summary>
	/// TESTING-only operation context owner. It materializes one opaque identity snapshot
	/// so capability/evidence/restore generations cannot be supplied as loose booleans.
	/// </summary>
	class ResearchVirtualizationOperationContext final : public std::enable_shared_from_this<ResearchVirtualizationOperationContext>
	{
	private:
		const int VirtualThreadId;
		const int OwnerContextId;
		const unsigned long long DomainTag;
		const unsigned long long AddressSpaceTag;
		unsigned long long CapabilityGeneration = 0;
		unsigned long long EvidenceGeneration = 0;
		unsigned long long RestoreGeneration = 0;

		std::mutex _contextSeal;
		unsigned long long _contextGeneration = 1;

	public:
		ResearchVirtualizationOperationContext(int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long addressSpaceTag, unsigned long long capabilityGeneration, unsigned long long evidenceGeneration, unsigned long long restoreGeneration);

	public:
		class IdentitySnapshot final : public std::enable_shared_from_this<IdentitySnapshot>
		{
		private:
			const std::any ContextSeal;
			const unsigned long long ContextGeneration;
			const std::shared_ptr<ResearchVirtualizationProbeIdentity> Identity;

		public:
			IdentitySnapshot(std::any contextSeal, unsigned long long contextGeneration, std::shared_ptr<ResearchVirtualizationProbeIdentity> identity);

			std::any getContextSeal() const;
			unsigned long long getContextGeneration() const;
			std::shared_ptr<ResearchVirtualizationProbeIdentity> getIdentity() const;
		};

		/// <summary>
		/// Opaque TESTING-only lease captured before canonical issue. It binds the
		/// runtime-owned identity and authority generations without guessing the E1
		/// attempt/replay identity that will be materialized at the issue boundary.
		/// </summary>
	public:
		class MaterializationLease final : public std::enable_shared_from_this<MaterializationLease>
		{
		private:
			const std::any ContextSeal;
			const unsigned long long ContextGeneration;
			const int VirtualThreadId;
			const int OwnerContextId;
			const unsigned long long DomainTag;
			const unsigned long long AddressSpaceTag;
			const unsigned long long CapabilityGeneration;
			const unsigned long long EvidenceGeneration;
			const unsigned long long RestoreGeneration;

		public:
			MaterializationLease(std::any contextSeal, unsigned long long contextGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long addressSpaceTag, unsigned long long capabilityGeneration, unsigned long long evidenceGeneration, unsigned long long restoreGeneration);

			std::any getContextSeal() const;
			unsigned long long getContextGeneration() const;
			int getVirtualThreadId() const;
			int getOwnerContextId() const;
			unsigned long long getDomainTag() const;
			unsigned long long getAddressSpaceTag() const;
			unsigned long long getCapabilityGeneration() const;
			unsigned long long getEvidenceGeneration() const;
			unsigned long long getRestoreGeneration() const;
		};

	private:
		int getVirtualThreadId() const;
		int getOwnerContextId() const;
		unsigned long long getDomainTag() const;
		unsigned long long getAddressSpaceTag() const;
		unsigned long long getCapabilityGeneration() const;
		void setCapabilityGeneration(unsigned long long value);
		unsigned long long getEvidenceGeneration() const;
		void setEvidenceGeneration(unsigned long long value);
		unsigned long long getRestoreGeneration() const;
		void setRestoreGeneration(unsigned long long value);

	public:
		std::shared_ptr<MaterializationLease> CaptureMaterializationLease();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal IdentitySnapshot? Materialize(MaterializationLease lease, ulong carrierAttemptId, ulong replayEpoch)
		std::shared_ptr<IdentitySnapshot> Materialize(std::shared_ptr<MaterializationLease> lease, unsigned long long carrierAttemptId, unsigned long long replayEpoch);

		std::shared_ptr<IdentitySnapshot> Capture(unsigned long long carrierAttemptId, unsigned long long replayEpoch);

		bool IsLive(std::shared_ptr<IdentitySnapshot> snapshot);

		bool IsLive(std::any contextSeal, unsigned long long contextGeneration);

		void Invalidate();

		void AdvanceCapabilityGeneration();

		void AdvanceEvidenceGeneration();

		void AdvanceRestoreGeneration();
	};

	/// <summary>
	/// TESTING-only neutral research owner for a state-minimal virtualization probe.
	/// It owns policy and deterministic execution, but SafetyVerifier alone issues admission.
	/// </summary>
	class ResearchVirtualizationRuntimeOwner final : public std::enable_shared_from_this<ResearchVirtualizationRuntimeOwner>
	{
	private:
		const std::any _ownerSeal = std::any();
		std::mutex _stateGate;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _consumedAttempts = new();
		std::unordered_set<unsigned long long> _consumedAttempts = std::unordered_set<unsigned long long>();
		unsigned long long _policyGeneration = 1;

	public:
		class PolicySnapshot final : public std::enable_shared_from_this<PolicySnapshot>
		{
		private:
			const std::any OwnerSeal;
			const unsigned long long PolicyGeneration;
			const ResearchVirtualizationProbeOperation Operation;

		public:
			PolicySnapshot(std::any ownerSeal, unsigned long long policyGeneration, ResearchVirtualizationProbeOperation operation);

			std::any getOwnerSeal() const;
			unsigned long long getPolicyGeneration() const;
			ResearchVirtualizationProbeOperation getOperation() const;
		};

	public:
		class ExecutionReceipt final : public std::enable_shared_from_this<ExecutionReceipt>
		{
		private:
			const unsigned long long PolicyGeneration;
			const unsigned long long VerifierGeneration;
			const ResearchVirtualizationProbeOperation Operation;
			const std::shared_ptr<ResearchVirtualizationProbeIdentity> Identity;

		public:
			ExecutionReceipt(unsigned long long policyGeneration, unsigned long long verifierGeneration, ResearchVirtualizationProbeOperation operation, std::shared_ptr<ResearchVirtualizationProbeIdentity> identity);

			unsigned long long getPolicyGeneration() const;
			unsigned long long getVerifierGeneration() const;
			ResearchVirtualizationProbeOperation getOperation() const;
			std::shared_ptr<ResearchVirtualizationProbeIdentity> getIdentity() const;
			int getPayloadLength() const;
			int getStateMutationCount() const;
			bool getCompletionPublicationAuthorized() const;
			bool getRetirePublicationAuthorized() const;
		};

	public:
		std::shared_ptr<PolicySnapshot> CapturePolicy();

		std::shared_ptr<ResearchVirtualizationProbeExecutionResult> Execute(std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate> certificate, std::shared_ptr<ResearchVirtualizationOperationContext> context);

		void InvalidatePolicy();

	private:
		static std::shared_ptr<ResearchVirtualizationProbeExecutionResult> Deny(ResearchVirtualizationProbeExecutionDecision decision, const std::wstring &reason);
	};

}

#endif
