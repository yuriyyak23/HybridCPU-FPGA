#pragma once
namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeIssuePermitOwner;
	class SecureComputeNoEffectCompletionOwner;
	class SecureComputeMemoryReadCompletionOwner;
	class SecureComputeMemoryReadMachineResult;
	class SecureComputeRegisterLocalCompletionOwner;
}
#include "ConditionalWeakTable.h"
#include "../../../../Guid.h"
#include "../../../../ExternalRuntime/Contracts/ExternalOperationIdentity.h"
#include "../../../../ExternalRuntime/Contracts/ExternalSecureExecutionConsumptionProvider.h"
#include "../../../../ExternalRuntime/Contracts/ExternalSecureMemoryReadProvider.h"

#include "VirtualizationAdmissionCertificate.h"
#include "VirtualizationOperationAdmissionCertificate.h"
#include "SafetyVerifier.Types.h"
#include "../Certificates/YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"
namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "../Certificates/BundleResourceCertificate.h"
#include "../MicroOps/Types/MicroOpMemoryRange.h"
#include "../Certificates/BundleResourceCertificate4Way.h"
#include "../Certificates/SafetyMask128.h"
#include "../Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "../Certificates/ReplayPhaseSubstrate.h"
#include "../../../Memory/MMU/MemoryDomain.h"
#include "../Certificates/BundleCertificate.h"
#include "../../Runtime/Events/Hypercalls/ResearchVirtualizationRuntimeProbe.h"
namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }
#include "../../Decoder/Rf06ExecutionContracts.h"
#include "SecureComputeIssuePermitOwner.h"
#include "YAKSys_Hybrid_CPU.Core.SecureComputeIssuePermitOwner.h"
#include "../../Runtime/Completion/Records/SecureComputeNoEffectCompletionOwner.h"
#include "SafetyVerifier.SecureComputeAdmission.h"
#include "SecureComputePlacedIntent.h"
#include "SecureComputeLiveCoreTarget.h"
#include "../../Runtime/Completion/Records/SecureComputeRegisterLocalCompletionOwner.h"
#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Scheduling/SlotLegality/SlotClassCapacity.h"
#include "../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "SafetyVerifier.VirtualizationAdmission.h"
#include "SafetyVerifier.VirtualizationOperationAdmission.h"
#include "../../Runtime/Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "../../Runtime/Services/YAKSys_Hybrid_CPU.Core.RuntimeBoundaryAdmissionService.h"
#include "../../Runtime/Migration/Checkpoint/DomainHypercallLifecycleGate.h"
#include "../../Runtime/Events/Hypercalls/DisabledNeutralVirtualizationOperationOwner.h"
#include "../../Runtime/Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "SecureComputeMemoryReadAdmission.h"
#include "../../Runtime/Completion/Records/SecureComputeMemoryReadCompletionOwner.h"
#include <string>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <any>
#include <functional>
#include <optional>
#include <mutex>
#include <format>
#include <memory>
#include "../../../../tangible_stringbuilder.h"

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class VirtualizationAdmissionCertificate; }

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { struct DmaStreamComputeOwnerBinding; struct DmaStreamComputeOwnerGuardContext; }
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;


using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;



using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;


using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;





using namespace YAKSys_Hybrid_CPU::Core::Registers;


using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationOperationOwnerSnapshot;
	class VirtualizationOperandSnapshot;
	class DomainRuntimeContext;
	class RootAuthorityDescriptor;
	class RuntimeCapabilityGrantOwner;
	class RuntimeCapabilityGrantLease;
	class VirtualizationRestoreGenerationOwner;
	class DomainHypercallLifecycleGate;

	struct VirtualizationE2IssueRequest final
	{
		ReplayPhaseContext ReplayPhase;
		SmtBundleMetadata4Way BundleMetadata;
		std::shared_ptr<VmxMicroOp> Carrier;
		int SourceSlotId;
		int WorkingSlotId;
		std::shared_ptr<VirtualizationAdmissionCertificate> E1;
		std::shared_ptr<VirtualizationOperationOwnerSnapshot> OwnerPolicy;
		std::shared_ptr<VirtualizationOperandSnapshot> Operand;
		std::shared_ptr<DomainRuntimeContext> DomainContext;
		std::shared_ptr<RootAuthorityDescriptor> RootAuthority;
		std::shared_ptr<RuntimeCapabilityGrantOwner> CapabilityOwner;
		std::shared_ptr<RuntimeCapabilityGrantLease> CapabilityLease;
		std::shared_ptr<VirtualizationRestoreGenerationOwner> RestoreOwner;
		std::shared_ptr<DomainHypercallLifecycleGate> LifecycleGate;
	};
}

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>Denial or historical bounded retirement/result facts; no live token or OS publication.</summary>


		/// <summary>Preserves known original facts when independent backend cleanup is unresolved.</summary>

		/// <summary>
		/// Denial with an unresolved provider reservation obligation. The original
		/// request is required for cleanup-only reconciliation, never admission retry.
		/// Canonical diagnostics, if present, confer no issue/publication authority.
		/// </summary>



	// Historical Phase-34 request remains fail-closed. In particular its mandatory
	// address-space/evidence booleans are not the PROBE_NO_STATE_V1 contract.









	class SafetyVerifier : public ILegalityChecker, public std::enable_shared_from_this<SafetyVerifier>
	{
	private:
		VerificationContext FormalContext;
		bool KernelDomainIsolation = true;
		const Guid SecureRegisterLocalScope = Guid::NewGuid();

		static LegalityDecision EvaluateSmtCertificate(BundleResourceCertificate4Way certificate, std::shared_ptr<MicroOp> candidate, LegalityAuthoritySource authoritySource, bool attemptedReplayReuse);

		static LegalityDecision EvaluateInterCoreCertificate(std::shared_ptr<BundleResourceCertificate> certificate, std::shared_ptr<MicroOp> candidate, LegalityAuthoritySource authoritySource, bool attemptedReplayReuse);

		static RejectKind MapCertificateReject(CertificateRejectDetail detail);


		/// <summary>
		/// Fast path verification using producer-side admission metadata.
		/// Checks shared structural overlap separately from register hazard legality so the
		/// compatibility helper no longer treats the raw safety mask as the correctness authority.
		/// </summary>
		/// <param name="bundle">Current VLIW bundle (8 slots)</param>
		/// <param name="candidate">Candidate micro-operation to inject</param>
		/// <param name="globalHardwareMask">Global hardware state (e.g., MSHR/Memory channels load)</param>
		/// <returns>True if injection is safe (no mask conflicts), false otherwise</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool VerifyInjectionFast128(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
		bool VerifyInjectionFast128(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128());

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private LegalityDecision EvaluateInterCoreCompatibility(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwnerThreadId, SafetyMask128 globalHardwareMask)
		LegalityDecision EvaluateInterCoreCompatibility(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, SafetyMask128 globalHardwareMask);

		static SafetyMask128 GetStructuralAdmissionMask(std::shared_ptr<MicroOp> microOp);

		static bool HasStructuralAdmissionMask(std::shared_ptr<MicroOp> microOp);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool BundleHasAllStructuralAdmissionMasks(IReadOnlyList<MicroOp?> bundle)
		static bool BundleHasAllStructuralAdmissionMasks(const std::vector<std::shared_ptr<MicroOp>> &bundle);

		static bool HasRegisterConflict(unsigned int bundleRegisterMask, unsigned int candidateRegisterMask);

		/// <summary>
		/// Check for register dependencies (RAW/WAW/WAR hazards) between candidate and bundle operations.
		///
		/// While threads have logically separate register files in the ISA, when operations from
		/// different threads are packed into the same VLIW bundle, they may share physical register
		/// resources during execution. FSP verification must check for register conflicts across
		/// ALL operations in the bundle, regardless of thread ownership.
		///
		/// This ensures:
		/// - No RAW/WAW/WAR hazards between any operations executing in the same cycle
		/// - Safe sharing of physical register file during VLIW bundle execution
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool CheckRegisterDependencies(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwnerThreadId, int candidateOwnerThreadId)
		bool CheckRegisterDependencies(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, int candidateOwnerThreadId);

		/// <summary>
		/// Check for memory dependencies (memory range conflicts) between candidate and bundle operations.
		///
		/// Memory conflicts occur when:
		/// - Two operations access overlapping memory ranges
		/// - At least one is a write
		///
		/// Memory domains (IOMMU) provide isolation between threads, but we still need to check
		/// for conflicts within operations that might be accessing shared memory regions.
		///
		/// Conservative approach: If any overlap exists with a write, reject injection.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool CheckMemoryDependencies(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwnerThreadId, int candidateOwnerThreadId)
		bool CheckMemoryDependencies(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, int candidateOwnerThreadId);

		/// <summary>
		/// Check if two sets of memory ranges overlap.
		/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool HasRangeOverlap(IReadOnlyList<(ulong Address, ulong Length)> ranges1, IReadOnlyList<(ulong Address, ulong Length)> ranges2)
		bool HasRangeOverlap(const std::vector<MicroOpMemoryRange> &ranges1, const std::vector<MicroOpMemoryRange> &ranges2);


		// Phase 5: Formal verification context
	public:
		VerificationContext getFormalContext() const;
		void setFormalContext(VerificationContext value);

		SafetyVerifier();


		/// <summary>
		/// Phase 04 guard plane: when true, kernel-mode operations (DomainTag==0)
		/// are prohibited from being admitted into user-domain bundles through
		/// scheduler, assist, or replay-aware densification paths. This prevents
		/// timing side-channels where a privileged micro-op could probe user-domain
		/// cache/TLB state.
		/// ISA contract: in user mode (podDomainCert != 0), DomainTag=0 is treated as deny-all.
		/// HLS: single AND gate on the domain certificate comparator.
		/// Runtime legality hot paths consume the effective value through
		/// <see cref="IRuntimeLegalityService"/> rather than by holding a concrete verifier.
		/// </summary>
		bool getKernelDomainIsolation() const;
		void setKernelDomainIsolation(bool value);
		bool isKernelDomainIsolationEnabled() const override;

		/// <summary>
		/// Verify domain certificate before scheduler admission, replay reuse, or certificate acceptance.
		/// Hardware gate: checks that the admitted operation belongs to a compatible
		/// memory protection domain by ANDing the operation's
		/// <see cref="MicroOp.Placement"/>.<see cref="SlotPlacementMetadata.DomainTag"/>
		/// with the Pod's domain certificate. If the result is zero, the domains are incompatible
		/// and the operation is rejected before any dependency checking occurs.
		///
		/// HLS-compatible: single-cycle AND + compare-to-zero in hardware.
		/// </summary>
		bool VerifyDomainCertificate(std::shared_ptr<MicroOp> injectedOp, unsigned long long podDomainCert);

		/// <summary>
		/// Public diagnostics/model helper for explicit domain-isolation probing.
		/// Production scheduler/runtime legality consumes checker-owned decisions through
		/// <see cref="IRuntimeLegalityService"/> instead of treating this probe result as
		/// runtime authority.
		/// </summary>
		DomainIsolationProbeResult EvaluateDomainIsolationProbe(std::shared_ptr<MicroOp> injectedOp, unsigned long long podDomainCert);

	private:
		DomainIsolationProbeResult EvaluateDomainIsolationProbe(unsigned long long domainTag, unsigned long long podDomainCert);

		/// <summary>
		/// Evaluate the descriptor-backed lane6 owner/domain guard before descriptor
		/// acceptance, replay reuse, certificate acceptance, or execution. The descriptor
		/// structural read may locate owner fields, but this guard decision is the first
		/// authority allowed to turn those fields into an admissible descriptor.
		/// </summary>
	public:
		std::shared_ptr<DmaStreamComputeOwnerGuardDecision> EvaluateDmaStreamComputeOwnerGuard(std::shared_ptr<DmaStreamComputeOwnerBinding> descriptorOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardContext> runtimeOwnerContext);

	private:
		static std::shared_ptr<DmaStreamComputeOwnerGuardDecision> RejectDmaStreamComputeOwnerGuard(std::shared_ptr<DmaStreamComputeOwnerBinding> descriptorOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardContext> runtimeOwnerContext, RejectKind rejectKind, const std::wstring &message);

		/// <summary>
		/// Evaluate inter-core nomination/domain filtering through an explicit checker-owned
		/// decision seam while preserving domain probe classification for telemetry.
		/// </summary>
	public:
		InterCoreDomainGuardDecision EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag) override;

		InterCoreDomainGuardDecision ILegalityChecker_EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag);

		LegalityDecision ILegalityChecker_EvaluateSmtBoundaryGuard(PhaseCertificateTemplateKey4Way liveTemplateKey);

	private:
		LegalityDecision CreateGuardReject(RejectKind rejectKind);

		std::optional<LegalityDecision> TryResolveInterCoreGuardDecision(int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate);

		std::optional<LegalityDecision> TryRejectSmtOwnerDomainGuard(SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate);

		/// <summary>
		/// Evaluate SMT legality through an explicit checker-owned decision seam.
		/// The scheduler consumes the returned <see cref="LegalityDecision"/> instead of
		/// treating the certificate or mask shape as the top-level legality authority.
		/// </summary>
	public:
		LegalityDecision EvaluateSmtBoundaryGuard(PhaseCertificateTemplateKey4Way liveTemplateKey) override;


		static bool TryClassifyMemoryFootprintInvalidation(std::shared_ptr<MicroOp> writer, std::shared_ptr<MicroOp> replayEvidence, ReplayPhaseInvalidationReason &reason);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static IReadOnlyList<(ulong Address, ulong Length)> GetSafetyVisibleReadRanges(MicroOp microOp)
//		{
//			if (microOp == nullptr)
//			{
//				return Array.Empty<(ulong Address, ulong Length)>();
//			}
//
//			IReadOnlyList<(ulong Address, ulong Length)> normalizedReadRanges = microOp.AdmissionMetadata.NormalizedReadMemoryRanges;
//			if (normalizedReadRanges != nullptr && normalizedReadRanges.Count != 0)
//			{
//				return normalizedReadRanges;
//			}
//
//			return microOp.ReadMemoryRanges ?? Array.Empty<(ulong Address, ulong Length)>();
//		}

	private:
		static std::vector<MicroOpMemoryRange> GetSafetyVisibleReadRanges(std::shared_ptr<MicroOp> microOp);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool HasAnyMemoryRangeOverlap(IReadOnlyList<(ulong Address, ulong Length)> leftRanges, IReadOnlyList<(ulong Address, ulong Length)> rightRanges)
		static bool HasAnyMemoryRangeOverlap(const std::vector<MicroOpMemoryRange> &leftRanges, const std::vector<MicroOpMemoryRange> &rightRanges);

		static unsigned long long SaturatingRangeEnd(unsigned long long address, unsigned long long length);


		/// <summary>
		/// Generate register isolation verification condition (Phase 5)
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private VerificationCondition GenerateRegisterIsolationCondition(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwner, int candidateOwner)
		VerificationCondition GenerateRegisterIsolationCondition(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwner, int candidateOwner);

		/// <summary>
		/// Generate memory domain separation verification condition (Phase 5)
		/// </summary>
		VerificationCondition GenerateMemoryDomainCondition(std::shared_ptr<MicroOp> candidate, int bundleOwner, int candidateOwner);

		/// <summary>
		/// Generate FSP non-interference verification condition (Phase 5)
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private VerificationCondition GenerateFSPNonInterferenceCondition(IReadOnlyList<MicroOp?> bundle, int targetSlot, MicroOp candidate)
		VerificationCondition GenerateFSPNonInterferenceCondition(const std::vector<std::shared_ptr<MicroOp>> &bundle, int targetSlot, std::shared_ptr<MicroOp> candidate);

		/// <summary>
		/// Generate SMT-LIB2 specification for register isolation (Phase 5)
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private string GenerateSMTRegisterIsolationSpec(HashSet<int> candidateWrites, IReadOnlyList<MicroOp?> bundle)
		std::wstring GenerateSMTRegisterIsolationSpec(std::unordered_set<int> &candidateWrites, const std::vector<std::shared_ptr<MicroOp>> &bundle);

		/// <summary>
		/// Generate SMT-LIB2 specification for memory domain separation (Phase 5)
		/// </summary>
		std::wstring GenerateSMTMemoryDomainSpec(MemoryDomain candidateDomain, MemoryDomain bundleDomain, std::vector<MicroOpMemoryRange> &accessRanges);

		/// <summary>
		/// Log verification conditions (Phase 5)
		/// </summary>
		void LogVerificationConditions(std::vector<VerificationCondition> &conditions);


		// Phase 2: Current cycle counter for proof generation
		long long _currentCycle = 0;

		/// <summary>
		/// Set current cycle for proof generation (Phase 2)
		/// </summary>
	public:
		void SetCurrentCycle(long long cycle);

		/// <summary>
		/// Generate a bundle resource proof certificate.
		/// This proof demonstrates that the bundle respects all safety invariants
		/// and can be verified by external monitors (OS, hypervisor, HRoT).
		/// (Phase 2: Formal Resource Proofs - Verification Readiness)
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public BundleResourceProof GenerateProof(IReadOnlyList<MicroOp?> bundle, SecurityContext context)
		BundleResourceProof GenerateProof(const std::vector<std::shared_ptr<MicroOp>> &bundle, SecurityContext context);

		/// <summary>
		/// Verify memory isolation for bundle operations.
		/// Ensures all memory accesses are within allowed bounds.
		/// (Phase 2: Internal verification helper)
		/// </summary>
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void VerifyMemoryIsolation(IReadOnlyList<MicroOp?> bundle, SecurityContext context)
		void VerifyMemoryIsolation(const std::vector<std::shared_ptr<MicroOp>> &bundle, SecurityContext context);


		/// <summary>
		/// Verify injection with formal proof generation (Phase 5)
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool VerifyInjectionWithProof(IReadOnlyList<MicroOp?> bundle, int targetSlot, MicroOp candidate, int bundleOwnerThreadId, int candidateOwnerThreadId, out VerificationCondition[] conditions)
		bool VerifyInjectionWithProof(const std::vector<std::shared_ptr<MicroOp>> &bundle, int targetSlot, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, int candidateOwnerThreadId, std::vector<VerificationCondition> &conditions);


#if defined(TESTING)
	public:
		class ResearchVirtualizationOperationAdmissionCertificate final : public std::enable_shared_from_this<ResearchVirtualizationOperationAdmissionCertificate>
		{
		private:
			const std::any OwnerSeal;
			const unsigned long long OwnerPolicyGeneration;
			const std::any ContextSeal;
			const unsigned long long ContextGeneration;
			const unsigned long long VerifierGeneration;
			const ResearchVirtualizationProbeOperation Operation;
			const std::shared_ptr<ResearchVirtualizationProbeIdentity> Identity;
			const std::shared_ptr<VirtualizationAdmissionCertificate> E1Carrier;

		public:
			ResearchVirtualizationOperationAdmissionCertificate(std::any verifierSeal, std::any ownerSeal, unsigned long long ownerPolicyGeneration, std::any contextSeal, unsigned long long contextGeneration, unsigned long long verifierGeneration, ResearchVirtualizationProbeOperation operation, std::shared_ptr<ResearchVirtualizationProbeIdentity> identity, std::shared_ptr<VirtualizationAdmissionCertificate> e1Carrier);

		private:
			const std::any _verifierSeal;
		public:
			std::any getOwnerSeal() const;
			unsigned long long getOwnerPolicyGeneration() const;
			std::any getContextSeal() const;
			unsigned long long getContextGeneration() const;
			unsigned long long getVerifierGeneration() const;
			ResearchVirtualizationProbeOperation getOperation() const;
			std::shared_ptr<ResearchVirtualizationProbeIdentity> getIdentity() const;
			std::shared_ptr<VirtualizationAdmissionCertificate> getE1Carrier() const;
			bool getHasNumericLeaf() const;
			bool getCompletionPublicationAuthorized() const;
			bool getRetirePublicationAuthorized() const;

			bool WasIssuedBy(std::any verifierSeal);
		};

	public:
		std::shared_ptr<ResearchVirtualizationProbeAdmissionResult> IssueResearchVirtualizationOperationAdmission(std::shared_ptr<ResearchVirtualizationRuntimeOwner::PolicySnapshot> policy, std::shared_ptr<ResearchVirtualizationOperationContext> context, std::shared_ptr<ResearchVirtualizationOperationContext::IdentitySnapshot> identitySnapshot, ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundle, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int workingSlotId, std::shared_ptr<VirtualizationAdmissionCertificate> e1);

		bool IsResearchVirtualizationOperationAdmissionLive(std::shared_ptr<ResearchVirtualizationOperationAdmissionCertificate> certificate);

	private:
		static std::shared_ptr<ResearchVirtualizationProbeAdmissionResult> DenyResearchVirtualizationAdmission(ResearchVirtualizationProbeAdmissionDecision decision, const std::wstring &reason);
#endif


		/// <summary>
		/// Create the legacy inter-core resource certificate for the current bundle.
		///
		/// This returns a structural conflict summary over the live bundle. It is not a
		/// standalone proof or external attestation object. Active SMT typed-slot hot paths
		/// instead route through <see cref="BundleResourceCertificate4Way"/>,
		/// <see cref="LegalityDecision"/>, and <see cref="IRuntimeLegalityService"/>.
		/// </summary>
		/// <param name="bundle">Current VLIW bundle (8 slots)</param>
		/// <param name="ownerThreadId">Thread ID that owns this bundle</param>
		/// <param name="cycleNumber">Current cycle number</param>
		/// <returns>Bundle resource certificate</returns>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public BundleResourceCertificate CreateCertificate(IReadOnlyList<MicroOp?> bundle, int ownerThreadId, ulong cycleNumber)
		std::shared_ptr<BundleResourceCertificate> CreateCertificate(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerThreadId, unsigned long long cycleNumber);

		/// <summary>
		/// Compatibility/test-facing certificate helper.
		/// Runtime legality hot paths no longer use this as top-level authority; they route
		/// through checker/cache/service seams and return <see cref="LegalityDecision"/>.
		/// </summary>
		/// <param name="certificate">Pre-computed bundle certificate</param>
		/// <param name="candidate">Candidate micro-operation to inject</param>
		/// <returns>True if injection is safe, false otherwise</returns>
		bool VerifyInjectionWithCertificate(std::shared_ptr<BundleResourceCertificate> certificate, std::shared_ptr<MicroOp> candidate);

		/// <summary>
		/// Evaluate inter-core legality through an explicit checker-owned decision seam.
		/// Replay-template reuse remains local to the checker/cache contract instead of
		/// returning a bare bool to scheduler-side callers.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal LegalityDecision EvaluateInterCoreLegality(IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
		LegalityDecision EvaluateInterCoreLegality(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: LegalityDecision ILegalityChecker.EvaluateInterCoreLegality(IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask)
		LegalityDecision ILegalityChecker_EvaluateInterCoreLegality(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask);


	private:
		std::recursive_mutex secureRegisterLocalGate;
		const std::shared_ptr<OperationAttemptIssuer> secureRegisterLocalAttemptIssuer = std::make_shared<OperationAttemptIssuer>();
		unsigned long long secureRegisterLocalSequence = 0;
		unsigned long long secureCertificateGeneration = 1;
		bool secureCertificateScopeExhausted = false;
	public:
		Guid getSecureRegisterLocalScope() const;
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ExternalSecureDomainLease> revokedSecureCertificateDomains = {};
		std::vector<std::shared_ptr<ExternalSecureDomainLease>> revokedSecureCertificateDomains;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<SecureComputeMachineCertificate, bool> secureCertificates = [];
		std::unordered_map<std::shared_ptr<SecureComputeMachineCertificate>, bool> secureCertificates = std::unordered_map<std::shared_ptr<SecureComputeMachineCertificate>, bool>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<SecureComputeMachineCertificate, ExternalSecureExecutionAttempt> secureBackendAttempts = [];
		std::unordered_map<std::shared_ptr<SecureComputeMachineCertificate>, std::shared_ptr<ExternalSecureExecutionAttempt>> secureBackendAttempts = std::unordered_map<std::shared_ptr<SecureComputeMachineCertificate>, std::shared_ptr<ExternalSecureExecutionAttempt>>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureComputeIssuePermitOwner? secureIssuePermits;
		std::shared_ptr<SecureComputeIssuePermitOwner> secureIssuePermits;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureComputeNoEffectCompletionOwner? secureNoEffectCompletion;
		std::shared_ptr<SecureComputeNoEffectCompletionOwner> secureNoEffectCompletion;

	public:
		std::shared_ptr<SecureComputeNoEffectCompletionOwner> getSecureNoEffectCompletion();
		bool IsSecureNoEffectOwner(std::shared_ptr<SecureComputeNoEffectCompletionOwner> owner);
		bool HasConsumedSecureMachinePermit(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared);

		std::optional<std::shared_ptr<ExternalSecureExecutionAttempt>> CreateSecureBackendAttempt(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared);

		bool IsSecureBackendAttempt(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt);

	private:
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> RunSecureNoEffect(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt, std::shared_ptr<ExternalSecureExecutionConsumptionResult> consumption, std::shared_ptr<SecureComputeMachinePermitConsumption> machineFact, std::shared_ptr<IExternalSecureNoEffectExecutionProvider> backend);

		std::shared_ptr<SecureComputeIssuePermitOwner> getSecureIssuePermits();

		/// <summary>
		/// Joins a current machine admission to original provider-local capacity.
		/// Provider callback runs outside all machine locks; current certificate and
		/// one-shot permit are checked after it returns. An optional zero-effect tail
		/// reaches canonical retire; historical consumption grants no later issue.
		/// </summary>
	public:
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> AdmitSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend);

		std::shared_ptr<SecureComputeCanonicalAdmissionResult> ExecuteSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend);

	private:
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> RunSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend, bool execute);

	public:
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> RunPreparedSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend, bool execute, std::shared_ptr<SecureComputePreparedMachineAdmission> prepared);

		/// <summary>
		/// Consumes the machine-local permit for an exact reservation correlation.
		/// Reservation construction/presence does not prove a live provider hold:
		/// backend entitlement consume remains a mandatory, currently closed boundary.
		/// The returned fact cannot authorize work and all live machine state closes.
		/// </summary>
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> AdmitSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputePreparedMachineAdmission? PrepareSecureRegisterLocalMachineAdmission(ExternalSecureRegisterLocalRequest request, ExternalSecureExecutionReservation reservation)
		std::shared_ptr<SecureComputePreparedMachineAdmission> PrepareSecureRegisterLocalMachineAdmission(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation);

		// Final immutable envelope check after the existing successful Stage-B route.
		// It cannot be called as a standalone certificate mint for caller-supplied DTOs.
	private:
		static bool VerifyFinalSecureRegisterLocalPlacement(std::shared_ptr<SecureComputePlacedIntent> placed);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputePreparedMachineAdmission? PrepareLiveSecureRegisterLocal(SecureComputeCanonicalAdmissionResult result, ExternalSecureExecutionReservation reservation, SecureComputeLiveCoreTarget target)
		std::shared_ptr<SecureComputePreparedMachineAdmission> PrepareLiveSecureRegisterLocal(std::shared_ptr<SecureComputeCanonicalAdmissionResult> result, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<SecureComputeLiveCoreTarget> target);

		bool IsCurrentSecureComputeCertificate(std::shared_ptr<SecureComputeMachineCertificate> certificate);

		bool RegisterSecureComputeCertificatePermit(std::shared_ptr<SecureComputeMachineCertificate> certificate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeMachinePermitConsumption? ConsumeSecureRegisterLocalMachinePermit(SecureComputePreparedMachineAdmission prepared, ExternalSecureRegisterLocalRequest request, SecureComputePlacedIntent placed, ExternalSecureExecutionReservation reservation)
		std::shared_ptr<SecureComputeMachinePermitConsumption> ConsumeSecureRegisterLocalMachinePermit(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<SecureComputePlacedIntent> placed, std::shared_ptr<ExternalSecureExecutionReservation> reservation);

		void CompleteSecureRegisterLocalMachineAdmission(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared);

		/// <summary>Invalidates machine-local admission only; establishes no provider containment.</summary>
		void InvalidateSecureRegisterLocalAdmissions(std::shared_ptr<ExternalSecureDomainLease> domain);

		/// <summary>Terminally invalidates all old local certificates before reset/restore/replay/remap.</summary>
		void InvalidateAllSecureRegisterLocalAdmissions();

		/// <summary>
		/// Canonicalizes one bounded neutral intent. ADD remains an ordinary primitive:
		/// it cannot become secure execution without final machine admission/permit.
		/// Input values remain in the original neutral request, never architectural state.
		/// </summary>
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> AdmitSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request);


	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<SecureComputeMachineCertificate> secureSubmissions = {};
		std::unordered_set<std::shared_ptr<SecureComputeMachineCertificate>> secureSubmissions = {};
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureComputeRegisterLocalCompletionOwner? secureRegisterLocalCompletion;
		std::shared_ptr<SecureComputeRegisterLocalCompletionOwner> secureRegisterLocalCompletion;
	public:
		std::shared_ptr<SecureComputeRegisterLocalCompletionOwner> getSecureRegisterLocalCompletion();
		bool IsSecureRegisterLocalOwner(std::shared_ptr<SecureComputeRegisterLocalCompletionOwner> owner);
		bool TryRegisterSecureSubmission(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared);

	private:
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> RunSecureArithmetic(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt, std::shared_ptr<ExternalSecureExecutionConsumptionResult> consumption, std::shared_ptr<SecureComputeMachinePermitConsumption> machineFact, std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> backend);



		/// <summary>
		/// Evaluate SMT legality through an explicit checker-owned decision seam.
		/// Boundary and owner/domain guard-plane checks run before any certificate reuse
		/// or structural resource conflict evaluation. Active authority order is guard
		/// plane first, then replay-phase certificate reuse when the live template
		/// matches, then current-bundle structural certificate fallback.
		/// </summary>
	public:
		LegalityDecision EvaluateSmtLegality(BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, PhaseCertificateTemplate4Way phaseTemplate, std::shared_ptr<MicroOp> candidate) override;

		LegalityDecision ILegalityChecker_EvaluateSmtLegality(BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, PhaseCertificateTemplate4Way phaseTemplate, std::shared_ptr<MicroOp> candidate);


		/// <summary>
		/// Public diagnostics/model helper for typed-slot rejection classification.
		/// Combines certificate-level and class-level reject information into a
		/// <see cref="TypedSlotRejectClassification"/> that bridges compiler/runtime vocabulary.
		/// Scheduler runtime paths use <see cref="IRuntimeLegalityService"/> instead of
		/// calling this helper on a concrete verifier.
		/// Compiler-only preflight categories such as `TypedSlotFactsInvalid` and
		/// `TypedSlotAliasedLaneConflict` do not have runtime reject twins and remain
		/// outside this classifier.
		/// <para>
		/// HLS: pure combinational - 3 comparators, no flip-flops, no timing impact.
		/// </para>
		/// </summary>
		TypedSlotRejectClassification ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind) override;

		TypedSlotRejectClassification ILegalityChecker_ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind);

		/// <summary>
		/// Validate typed-slot facts emitted by the compiler against a live bundle.
		/// Called at decode time if validation is enabled (diagnostic-only, not on critical path).
		/// Current staged status is <see cref="TypedSlotFactMode.ValidationOnly"/> via
		/// <see cref="TypedSlotFactStaging.CurrentMode"/>: empty/default facts remain
		/// legal, and canonical runtime execution must stay correct without them.
		/// <para>
		/// Check 1: per-slot class matches <see cref="MicroOp.Placement"/>.<see cref="SlotPlacementMetadata.RequiredSlotClass"/> for non-null ops.
		/// Check 2: per-class counts match actual ops in bundle.
		/// Check 3: no duplicate <see cref="MicroOp.Placement"/>.<see cref="SlotPlacementMetadata.PinnedLaneId"/> among hard-pinned ops.
		/// Check 4: class counts within <see cref="SlotClassLaneMap.GetClassCapacity"/> bounds.
		/// Check 5: PinnedOpCount + FlexibleOpCount == total non-null ops.
		/// </para>
		/// </summary>
		/// <param name="facts">Compiler-emitted typed-slot facts.</param>
		/// <param name="bundle">The VLIW bundle (nullable slots).</param>
		/// <param name="bundleWidth">Number of slots to validate (default 8).</param>
		/// <returns><see langword="true"/> if all checks pass.</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool ValidateTypedSlotFacts(TypedSlotBundleFacts facts, MicroOp?[] bundle, int bundleWidth = 8)
		static bool ValidateTypedSlotFacts(TypedSlotBundleFacts facts, std::vector<std::shared_ptr<MicroOp>> &bundle, int bundleWidth = 8);

		/// <summary>
		/// Verify that compiler typed-slot facts are consistent with runtime capacity state.
		/// Pure diagnostic agreement check under the current
		/// <see cref="TypedSlotFactMode.ValidationOnly"/> staging. This does not yet
		/// make facts mandatory for runtime admission.
		/// <para>
		/// HLS: pure combinational - 5 comparators, no flip-flops, no timing impact.
		/// </para>
		/// </summary>
		/// <param name="compilerFacts">Compiler-emitted typed-slot facts.</param>
		/// <param name="runtimeCapacity">Current runtime class-capacity state.</param>
		/// <returns><see langword="true"/> if compiler class counts fit runtime capacity.</returns>
		static bool VerifyTypedSlotAgreement(TypedSlotBundleFacts compilerFacts, SlotClassCapacityState runtimeCapacity);


		/// <summary>
		/// Verify that a full VLIW bundle respects all safety constraints.
		/// This is used for validation/testing purposes.
		/// </summary>
		/// <param name="bundle">VLIW bundle to verify</param>
		/// <returns>True if bundle is safe, false if violations exist</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool VerifyBundle(IReadOnlyList<MicroOp?> bundle)
		bool VerifyBundle(const std::vector<std::shared_ptr<MicroOp>> &bundle);

		/// <summary>
		/// Export all verification conditions as SMT-LIB2 (Phase 5)
		/// </summary>
		std::wstring ExportSMTLib2Proofs();

//		#region Phase 2: Formal Resource Proof Fixtures

		/// <summary>
		/// Sign a proof with an ISE-local simulated signature.
		/// This is validation evidence only, not a hardware root-of-trust claim.
		/// </summary>
	private:
		BundleResourceProof SignProof(BundleResourceProof proof);

//		#endregion


		/// <summary>
		/// Opaque E1 admission issued only by one live SafetyVerifier instance after the
		/// existing legality decision and successful Stage-B lane materialization.
		/// It is deliberately incapable of authorizing backend, completion or retire.
		/// </summary>
	public:
        using VirtualizationAdmissionCertificate = YAKSys_Hybrid_CPU::Core::VirtualizationAdmissionCertificate;

	private:
		const std::any _virtualizationAdmissionIssuerSeal = std::make_shared<int>(0);
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private sealed record LiveVirtualizationAdmission(ulong AttemptId, ulong IssuerGeneration);
		struct LiveVirtualizationAdmission final { unsigned long long AttemptId; unsigned long long IssuerGeneration; };

		const std::shared_ptr<ConditionalWeakTable<std::shared_ptr<VirtualizationAdmissionCertificate>, std::shared_ptr<LiveVirtualizationAdmission>>> _liveVirtualizationAdmissions = std::make_shared<ConditionalWeakTable<std::shared_ptr<VirtualizationAdmissionCertificate>, std::shared_ptr<LiveVirtualizationAdmission>>>();
		unsigned long long _virtualizationAdmissionIssuerGeneration = 1;
		unsigned long long _nextVirtualizationAdmissionAttemptId = 1;

	public:
		std::shared_ptr<VirtualizationAdmissionIssueResult> IssueVirtualizationAdmissionAfterStageB(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationAdmissionValidationResult ValidateVirtualizationAdmission(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, MicroOp candidate, int sourceSlotId, int selectedLane, VirtualizationAdmissionCertificate? certificate)
		std::shared_ptr<VirtualizationAdmissionValidationResult> ValidateVirtualizationAdmission(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane, std::shared_ptr<VirtualizationAdmissionCertificate> certificate);

		void InvalidateVirtualizationAdmissions(ReplayPhaseInvalidationReason reason);

	private:
		unsigned long long AllocateVirtualizationAdmissionAttemptId();

		static unsigned long long ComputeBundleIdentity(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata);

		static unsigned long long ComputeCarrierIdentityDigest(std::shared_ptr<VmxMicroOp> vmx);

		static void Mix(unsigned long long &hash, unsigned long long value);

		static std::shared_ptr<VirtualizationAdmissionIssueResult> DenyIssue(VirtualizationAdmissionIssueDecision decision, const std::wstring &reason);

		static std::shared_ptr<VirtualizationAdmissionValidationResult> DenyValidation(VirtualizationAdmissionValidationDecision decision, const std::wstring &reason);


		/// <summary>
		/// Opaque, non-serializable E2 for exactly one accepted no-state probe attempt.
		/// It is admission evidence only and deliberately has no backend consumer in PR-D.
		/// </summary>
	public:
        using VirtualizationOperationAdmissionCertificate = YAKSys_Hybrid_CPU::Core::VirtualizationOperationAdmissionCertificate;

	private:
		class LiveVirtualizationE2 final : public std::enable_shared_from_this<LiveVirtualizationE2>
		{
		private:
			const std::shared_ptr<VirtualizationE2IssueRequest> Request;
			VirtualizationE2State State = VirtualizationE2State::Issued;

		public:
			LiveVirtualizationE2(std::shared_ptr<VirtualizationE2IssueRequest> request);

			std::shared_ptr<VirtualizationE2IssueRequest> getRequest() const;
			VirtualizationE2State getState() const;
			void setState(VirtualizationE2State value);
		};

	private:
		const std::any _virtualizationE2IssuerSeal = std::make_shared<int>(0);
		const std::shared_ptr<ConditionalWeakTable<std::shared_ptr<VirtualizationOperationAdmissionCertificate>, std::shared_ptr<LiveVirtualizationE2>>> _liveVirtualizationE2 = std::make_shared<ConditionalWeakTable<std::shared_ptr<VirtualizationOperationAdmissionCertificate>, std::shared_ptr<LiveVirtualizationE2>>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<VirtualizationOperationAdmissionCertificate> _issuedVirtualizationE2 = new();
		std::unordered_set<std::shared_ptr<VirtualizationOperationAdmissionCertificate>> _issuedVirtualizationE2 = std::unordered_set<std::shared_ptr<VirtualizationOperationAdmissionCertificate>>();
		const std::shared_ptr<WeakIdentitySet<std::shared_ptr<VirtualizationOperandSnapshot>>> _issuedVirtualizationE2ByOperand = std::make_shared<WeakIdentitySet<std::shared_ptr<VirtualizationOperandSnapshot>>>();
		const std::shared_ptr<RuntimeBoundaryAdmissionService> _virtualizationE2BoundaryAdmission = std::make_shared<RuntimeBoundaryAdmissionService>();
		std::mutex _virtualizationE2Sync;
		unsigned long long _nextVirtualizationE2Sequence = 1;

	public:
		std::shared_ptr<VirtualizationE2Result> IssueVirtualizationE2(std::shared_ptr<VirtualizationE2IssueRequest> request);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationE2Result ValidateVirtualizationE2(VirtualizationOperationAdmissionCertificate? certificate, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner)
		std::shared_ptr<VirtualizationE2Result> ValidateVirtualizationE2(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool RevokeVirtualizationE2(VirtualizationOperationAdmissionCertificate? certificate)
		bool RevokeVirtualizationE2(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationE2ConsumptionResult ConsumeVirtualizationE2FromExactExecutor(VirtualizationOperationAdmissionCertificate? certificate, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, DomainHypercallLifecycleGate lifecycleGate, Object consumerSeal)
		std::shared_ptr<VirtualizationE2ConsumptionResult> ConsumeVirtualizationE2FromExactExecutor(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate, std::any consumerSeal);

		VirtualizationE2State GetVirtualizationE2State(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate);

		int CountLiveVirtualizationE2(unsigned long long domainTag);

		int CancelLiveVirtualizationE2ForDrain(unsigned long long domainTag);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationOperationAdmissionResult EvaluateVirtualizationOperationAdmission(VirtualizationOperationAdmissionRequest request, INeutralVirtualizationOperationOwner? owner = null)
		std::shared_ptr<VirtualizationOperationAdmissionResult> EvaluateVirtualizationOperationAdmission(std::shared_ptr<VirtualizationOperationAdmissionRequest> request, std::shared_ptr<INeutralVirtualizationOperationOwner> owner = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static CapabilityGrant? ResolveExactGrant(DomainRuntimeContext context)
		static std::shared_ptr<CapabilityGrant> ResolveExactGrant(std::shared_ptr<DomainRuntimeContext> context);

		unsigned long long AllocateE2Sequence();

		static std::shared_ptr<VirtualizationE2Result> DenyE2(VirtualizationE2Decision decision, const std::wstring &reason);

		static std::shared_ptr<VirtualizationOperationAdmissionResult> DenyLegacy(VirtualizationOperationAdmissionDecision decision, const std::wstring &reason);


// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<SecureComputeMemoryReadCertificate, bool> memoryReadCertificates = [];
		std::unordered_map<std::shared_ptr<SecureComputeMemoryReadCertificate>, bool> memoryReadCertificates = std::unordered_map<std::shared_ptr<SecureComputeMemoryReadCertificate>, bool>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ExternalOperationIdentity, ExternalSecureMemoryReadRequest> memoryReadOperations = [];
		std::map<ExternalOperationIdentity, std::shared_ptr<ExternalSecureMemoryReadRequest>> memoryReadOperations;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureComputeMemoryReadCompletionOwner? memoryReadCompletion;
		std::shared_ptr<SecureComputeMemoryReadCompletionOwner> memoryReadCompletion;

	public:
		bool IsCurrentMemoryReadCertificate(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate);

		bool RegisterMemoryReadPermit(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate);
		bool HasConsumedMemoryReadPermit(std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeMemoryReadMachineResult ExecuteSecureMemoryRead(ExternalSecureMemoryReadRequest request, ExternalSecureMemoryReadReservation reservation, IExternalSecureMemoryReadProvider backend, IExternalSecureMemoryReadOrigin originalSource, System.Func<bool>? ownerCurrent = null)
		std::shared_ptr<SecureComputeMemoryReadMachineResult> ExecuteSecureMemoryRead(std::shared_ptr<ExternalSecureMemoryReadRequest> request, std::shared_ptr<ExternalSecureMemoryReadReservation> reservation, std::shared_ptr<IExternalSecureMemoryReadProvider> backend, std::shared_ptr<IExternalSecureMemoryReadOrigin> originalSource, std::function<bool()> ownerCurrent = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureComputeMemoryReadCertificate? PrepareMemoryRead(ExternalSecureMemoryReadRequest request, ExternalSecureMemoryReadReservation reservation, IExternalSecureMemoryReadOrigin origin, Object sourceGate, Object providerGate, System.Nullable<System.Func<bool>> ownerCurrent)
		std::shared_ptr<SecureComputeMemoryReadCertificate> PrepareMemoryRead(std::shared_ptr<ExternalSecureMemoryReadRequest> request, std::shared_ptr<ExternalSecureMemoryReadReservation> reservation, std::shared_ptr<IExternalSecureMemoryReadOrigin> origin, std::any sourceGate, std::any providerGate, std::function<bool()> ownerCurrent);
	};
}
