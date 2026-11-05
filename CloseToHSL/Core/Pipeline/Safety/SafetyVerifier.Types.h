#pragma once

#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Certificates/BundleResourceCertificate4Way.h"
namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "../Certificates/BundleResourceCertificate.h"
#include "../Certificates/ReplayPhaseSubstrate.h"
#include "../Certificates/SafetyMask128.h"
#include "../Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "../Certificates/YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"
#include "../../../Memory/MMU/MemoryDomain.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class LegalityDecision; class PhaseCertificateTemplate4Way; }

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Combined reject classification for typed-slot world.
	/// Provides a complete picture: what kind of reject, where it happened, why.
	/// <para>
	/// HLS design note: pure combinational decode of existing reject codes.
	/// 3 comparators, no flip-flops, no timing impact.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct TypedSlotRejectClassification
	class TypedSlotRejectClassification
	{
	private:
		TypedSlotRejectReason AdmissionReject;
		CertificateRejectDetail CertificateDetail;
		SlotClass CandidateClass;
		SlotPinningKind PinningKind;
		bool IsPinnedConflict;
		bool IsClassCapacityIssue;
		bool IsStaticOvercommit;
		bool IsDynamicExhaustion;
		bool IsDynamicStateIssue;

		/// <summary>Admission-stage reject reason from the two-stage pipeline.</summary>
	public:
		TypedSlotRejectReason getAdmissionReject() const;

		/// <summary>Certificate-level detail (shared vs register group).</summary>
		CertificateRejectDetail getCertificateDetail() const;

		/// <summary>Slot class of the rejected candidate.</summary>
		SlotClass getCandidateClass() const;

		/// <summary>Pinning kind of the rejected candidate.</summary>
		SlotPinningKind getPinningKind() const;

		/// <summary>True if reject was due to hard-pinned lane already occupied.</summary>
		bool isPinnedConflict() const;

		/// <summary>True if reject was due to all class lanes being occupied.</summary>
		bool isClassCapacityIssue() const;

		/// <summary>True if reject was due to static overcommit (compiler fault).</summary>
		bool isStaticOvercommit() const;

		/// <summary>True if reject was due to dynamic typed-slot densification exhaustion during normal runtime class-capacity pressure.</summary>
		bool isDynamicExhaustion() const;

		/// <summary>True if reject was due to dynamic runtime state (scoreboard, bank, budget).</summary>
		bool isDynamicStateIssue() const;

		TypedSlotRejectClassification(TypedSlotRejectReason admissionReject, CertificateRejectDetail certificateDetail, SlotClass candidateClass, SlotPinningKind pinningKind, bool isPinnedConflict, bool isClassCapacityIssue, bool isStaticOvercommit, bool isDynamicExhaustion, bool isDynamicStateIssue);

		TypedSlotRejectClassification() : AdmissionReject(TypedSlotRejectReason::None), CertificateDetail(CertificateRejectDetail::None), CandidateClass(SlotClass::Unclassified), PinningKind(SlotPinningKind::ClassFlexible), IsPinnedConflict(false), IsClassCapacityIssue(false), IsStaticOvercommit(false), IsDynamicExhaustion(false), IsDynamicStateIssue(false) {}
	};

	/// <summary>
	/// Explicit classification for domain-isolation stress probes.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DomainIsolationProbeResult
	class DomainIsolationProbeResult
	{
	private:
		const bool IsAllowed;
		const bool IsCrossDomainBlock;
		const bool IsKernelToUserBlock;

	public:
		DomainIsolationProbeResult(bool isAllowed, bool isCrossDomainBlock, bool isKernelToUserBlock);

		bool isAllowed() const;

		bool isCrossDomainBlock() const;

		bool isKernelToUserBlock() const;

	};

	/// <summary>
	/// Explicit checker-owned guard decision for inter-core domain filtering.
	/// Keeps telemetry classification available without letting scheduler-side callers
	/// treat the raw probe result as legality authority.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct InterCoreDomainGuardDecision


	/// <summary>
	/// Which structural authority produced the current legality decision.
	/// </summary>
	enum class LegalityAuthoritySource : unsigned char
	{
		StructuralCertificate = 0,
		ReplayPhaseCertificate = 1,
		GuardPlane = 2,
		DetailedCompatibilityCheck = 3,
		AdmissionMetadataStructuralCheck = 4
	};

	/// <summary>
	/// Explicit legality reject classification for runtime checker decisions.
	/// </summary>
	enum class RejectKind : unsigned char
	{
		None = 0,
		Boundary,
		OwnerMismatch,
		DomainMismatch,
		EpochMismatch,
		ClassCapacity,
		LaneUnavailable,
		CrossLaneConflict,
		RareHazard,
		Ordering,
		CertificateStale
	};

	/// <summary>
	/// Explicit checker-owned legality decision for scheduler hot-path consumers.
	/// Keeps structural certificate detail local while exposing a stable authority seam.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct LegalityDecision
	class LegalityDecision
	{
	private:
		const bool IsAllowed;
		const YAKSys_Hybrid_CPU::Core::RejectKind RejectKind;
		const CertificateRejectDetail CertificateDetail;
		const LegalityAuthoritySource AuthoritySource;
		const bool AttemptedReplayCertificateReuse;

	public:
		LegalityDecision(bool isAllowed, YAKSys_Hybrid_CPU::Core::RejectKind rejectKind, CertificateRejectDetail certificateDetail, LegalityAuthoritySource authoritySource, bool attemptedReplayCertificateReuse);

		bool isAllowed() const;

		YAKSys_Hybrid_CPU::Core::RejectKind getRejectKind() const;

		CertificateRejectDetail getCertificateDetail() const;

		LegalityAuthoritySource getAuthoritySource() const;

		bool getAttemptedReplayCertificateReuse() const;

		bool getReusedReplayCertificate() const;

		static LegalityDecision Allow(LegalityAuthoritySource authoritySource, bool attemptedReplayCertificateReuse);

		static LegalityDecision Reject(YAKSys_Hybrid_CPU::Core::RejectKind rejectKind, CertificateRejectDetail certificateDetail, LegalityAuthoritySource authoritySource, bool attemptedReplayCertificateReuse);

	};

	class InterCoreDomainGuardDecision
	{
	private:
		const YAKSys_Hybrid_CPU::Core::LegalityDecision LegalityDecision;
		const DomainIsolationProbeResult ProbeResult;

	public:
		InterCoreDomainGuardDecision(YAKSys_Hybrid_CPU::Core::LegalityDecision legalityDecision, DomainIsolationProbeResult probeResult);

		YAKSys_Hybrid_CPU::Core::LegalityDecision getLegalityDecision() const;

		DomainIsolationProbeResult getProbeResult() const;

		bool isAllowed() const;

	};

	/// <summary>
	/// Staged normative status for compiler-emitted typed-slot facts.
	/// This is an architectural vocabulary surface; only
	/// <see cref="ValidationOnly"/> is active in the current mainline.
	/// </summary>
	enum class TypedSlotFactMode : unsigned char
	{
		/// <summary>
		/// Facts are validated and compared when present, but canonical runtime
		/// execution remains correct and fail-closed without them.
		/// </summary>
		ValidationOnly = 0,

		/// <summary>
		/// Future mode: missing facts would surface an explicit warning or
		/// diagnostic, but admission would still remain runtime-owned.
		/// </summary>
		WarnOnMissing = 1,

		/// <summary>
		/// Future stricter mode: missing or invalid facts would fail before the
		/// typed-slot admission path is allowed to proceed.
		/// </summary>
		RequiredForAdmission = 2
	};

	/// <summary>
	/// Canonical repository-facing staging surface for typed-slot facts.
	/// Current code remains in <see cref="TypedSlotFactMode.ValidationOnly"/>;
	/// the stronger modes are declared here so docs and tests can describe the
	/// staged path without pretending it already landed.
	/// </summary>
	class TypedSlotFactStaging final
	{
	public:
		static TypedSlotFactMode getCurrentMode();

		static bool getAllowsCanonicalExecutionWithoutFacts();
	};

	/// <summary>
	/// Runtime-local legality checker seam for scheduler and replay/certificate hot paths.
	/// This is intentionally separate from <see cref="YAKSys_Hybrid_CPU.Core.Legality.ILegalityChecker"/>,
	/// which serves the older decoder/instruction legality layer.
	/// </summary>
	class ILegalityChecker
	{
	public:
		virtual bool isKernelDomainIsolationEnabled() const = 0;

		virtual InterCoreDomainGuardDecision EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag) = 0;

		virtual TypedSlotRejectClassification ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind) = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: LegalityDecision EvaluateInterCoreLegality(IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default);
		virtual LegalityDecision EvaluateInterCoreLegality(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) = 0;

		virtual LegalityDecision EvaluateSmtBoundaryGuard(PhaseCertificateTemplateKey4Way liveTemplateKey) = 0;

		virtual LegalityDecision EvaluateSmtLegality(BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, PhaseCertificateTemplate4Way phaseTemplate, std::shared_ptr<MicroOp> candidate) = 0;
	};

	/// <summary>
	/// Internal compatibility-only runtime checker provisioning.
	/// Keeps checker instantiation behind runtime legality service wiring instead of
	/// exposing it as a public scheduler/runtime seam.
	/// </summary>
	class RuntimeLegalityCheckerFactory final
	{
	public:
		static std::shared_ptr<ILegalityChecker> CreateCompatibilityDefault();
	};

	/// <summary>
	/// Verification condition type for formal proofs (Phase 5)
	/// </summary>
	enum class VerificationConditionType
	{
		RegisterIsolation,
		MemoryDomainSeparation,
		FSPNonInterference,
		NoDataRaces,
		PredicateSafety
	};

	/// <summary>
	/// Verification condition for formal proofs (Phase 5)
	/// </summary>
	class VerificationCondition
	{
	public:
		VerificationConditionType Type = static_cast<VerificationConditionType>(0);
		std::wstring Description;
		bool IsValid = false;
		std::wstring CounterExample; // If !IsValid
		std::wstring FormalSpec; // Z3/SMT formula (optional)

		VerificationCondition();
	};

	/// <summary>
	/// Formal verification context (Phase 5)
	/// </summary>
	class VerificationContext
	{
	public:
		std::unordered_map<int, MemoryDomain> ThreadMemoryDomains;
		std::vector<VerificationCondition> GeneratedConditions;
		bool EnableFormalChecks = false;

		VerificationContext();
	};

	/// <summary>
	/// Security context for bundle verification
	/// Defines resource boundaries and access policies for a thread/process
	/// (Phase 2: Formal Resource Proofs)
	/// </summary>
	class SecurityContext
	{
		/// <summary>
		/// Minimum accessible memory address
		/// </summary>
	public:
		unsigned long long MinAddr = 0;

		/// <summary>
		/// Maximum accessible memory address
		/// </summary>
		unsigned long long MaxAddr = 0;

		/// <summary>
		/// Bitmask of active hardware threads
		/// </summary>
		unsigned int ActiveThreads = 0;

		/// <summary>
		/// Thread ID that owns this security context
		/// </summary>
		int OwnerThreadId = 0;

		SecurityContext();
	};

	/// <summary>
	/// Safety Verifier for Formally Safe Packing (FSP).
	///
	/// Purpose: Verify that injecting a micro-operation from one thread into another thread's
	/// VLIW bundle does not violate non-interference constraints.
	///
	/// Non-interference contract:
	/// 1. State isolation: Registers/CSR/PC are logically separate per thread
	/// 2. Memory domain separation: IOMMU enforces memory access boundaries
	/// 3. Per-thread in-order commit: Injection doesn't break commit order within a thread
	/// 4. No cross-thread hazards: No RAW/WAW/WAR conflicts between threads in same bundle
	/// 5. Control/system ops non-stealable: Privileged operations stay in original thread
	///
	/// Verification algorithm:
	/// - Check register dependencies (RAW/WAW/WAR)
	/// - Check memory range conflicts (overlap detection)
	/// - Check operation type restrictions (control flow, CSR, etc.)
	/// - Check capability domains (IOMMU/protection domains)
	///
	/// Phase 5 additions:
	/// - Formal verification condition generation
	/// - SMT-LIB2 proof export
	/// - Memory domain verification
	///
	/// Phase 2 additions:
	/// - simulated proof-fixture signature generation, not a hardware root-of-trust claim
	/// - bundle resource proof certificates
	/// - Singularity/SIP-style isolation verification
}
