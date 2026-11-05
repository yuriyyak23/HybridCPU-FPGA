#include "SafetyVerifier.Types.h"
#include "YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Core
{

	TypedSlotRejectReason TypedSlotRejectClassification::getAdmissionReject() const
	{
		return AdmissionReject;
	}

	CertificateRejectDetail TypedSlotRejectClassification::getCertificateDetail() const
	{
		return CertificateDetail;
	}

	SlotClass TypedSlotRejectClassification::getCandidateClass() const
	{
		return CandidateClass;
	}

	SlotPinningKind TypedSlotRejectClassification::getPinningKind() const
	{
		return PinningKind;
	}

	bool TypedSlotRejectClassification::isPinnedConflict() const
	{
		return IsPinnedConflict;
	}

	bool TypedSlotRejectClassification::isClassCapacityIssue() const
	{
		return IsClassCapacityIssue;
	}

	bool TypedSlotRejectClassification::isStaticOvercommit() const
	{
		return IsStaticOvercommit;
	}

	bool TypedSlotRejectClassification::isDynamicExhaustion() const
	{
		return IsDynamicExhaustion;
	}

	bool TypedSlotRejectClassification::isDynamicStateIssue() const
	{
		return IsDynamicStateIssue;
	}

	TypedSlotRejectClassification::TypedSlotRejectClassification(TypedSlotRejectReason admissionReject, CertificateRejectDetail certificateDetail, SlotClass candidateClass, SlotPinningKind pinningKind, bool isPinnedConflict, bool isClassCapacityIssue, bool isStaticOvercommit, bool isDynamicExhaustion, bool isDynamicStateIssue)
		: AdmissionReject(admissionReject), CertificateDetail(certificateDetail), CandidateClass(candidateClass), PinningKind(pinningKind),
		  IsPinnedConflict(isPinnedConflict), IsClassCapacityIssue(isClassCapacityIssue), IsStaticOvercommit(isStaticOvercommit), IsDynamicExhaustion(isDynamicExhaustion), IsDynamicStateIssue(isDynamicStateIssue)
	{}

	DomainIsolationProbeResult::DomainIsolationProbeResult(bool isAllowed, bool isCrossDomainBlock, bool isKernelToUserBlock)
		: IsAllowed(isAllowed), IsCrossDomainBlock(isCrossDomainBlock), IsKernelToUserBlock(isKernelToUserBlock)
	{}

	bool DomainIsolationProbeResult::isAllowed() const
	{
		return IsAllowed;
	}

	bool DomainIsolationProbeResult::isCrossDomainBlock() const
	{
		return IsCrossDomainBlock;
	}

	bool DomainIsolationProbeResult::isKernelToUserBlock() const
	{
		return IsKernelToUserBlock;
	}

	InterCoreDomainGuardDecision::InterCoreDomainGuardDecision(YAKSys_Hybrid_CPU::Core::LegalityDecision legalityDecision, DomainIsolationProbeResult probeResult)
		: LegalityDecision(std::move(legalityDecision)), ProbeResult(std::move(probeResult))
	{}

	YAKSys_Hybrid_CPU::Core::LegalityDecision InterCoreDomainGuardDecision::getLegalityDecision() const
	{
		return LegalityDecision;
	}

	DomainIsolationProbeResult InterCoreDomainGuardDecision::getProbeResult() const
	{
		return ProbeResult;
	}

	bool InterCoreDomainGuardDecision::isAllowed() const
	{
		return getLegalityDecision().isAllowed();
	}

	LegalityDecision::LegalityDecision(bool isAllowed, YAKSys_Hybrid_CPU::Core::RejectKind rejectKind, CertificateRejectDetail certificateDetail, LegalityAuthoritySource authoritySource, bool attemptedReplayCertificateReuse)
		: IsAllowed(isAllowed), RejectKind(rejectKind), CertificateDetail(certificateDetail), AuthoritySource(authoritySource), AttemptedReplayCertificateReuse(attemptedReplayCertificateReuse)
	{}

	bool LegalityDecision::isAllowed() const
	{
		return IsAllowed;
	}

	YAKSys_Hybrid_CPU::Core::RejectKind LegalityDecision::getRejectKind() const
	{
		return RejectKind;
	}

	CertificateRejectDetail LegalityDecision::getCertificateDetail() const
	{
		return CertificateDetail;
	}

	LegalityAuthoritySource LegalityDecision::getAuthoritySource() const
	{
		return AuthoritySource;
	}

	bool LegalityDecision::getAttemptedReplayCertificateReuse() const
	{
		return AttemptedReplayCertificateReuse;
	}

	bool LegalityDecision::getReusedReplayCertificate() const
	{
		return getAuthoritySource() == LegalityAuthoritySource::ReplayPhaseCertificate;
	}

	LegalityDecision LegalityDecision::Allow(LegalityAuthoritySource authoritySource, bool attemptedReplayCertificateReuse)
	{
		return LegalityDecision(true, RejectKind::None, CertificateRejectDetail::None, authoritySource, attemptedReplayCertificateReuse);
	}

	LegalityDecision LegalityDecision::Reject(YAKSys_Hybrid_CPU::Core::RejectKind rejectKind, CertificateRejectDetail certificateDetail, LegalityAuthoritySource authoritySource, bool attemptedReplayCertificateReuse)
	{
		return LegalityDecision(false, rejectKind, certificateDetail, authoritySource, attemptedReplayCertificateReuse);
	}

	TypedSlotFactMode TypedSlotFactStaging::getCurrentMode()
	{
		return TypedSlotFactMode::ValidationOnly;
	}

	bool TypedSlotFactStaging::getAllowsCanonicalExecutionWithoutFacts()
	{
		return getCurrentMode() != TypedSlotFactMode::RequiredForAdmission;
	}

	std::shared_ptr<ILegalityChecker> RuntimeLegalityCheckerFactory::CreateCompatibilityDefault()
	{
		return std::make_shared<SafetyVerifier>();
	}

	VerificationCondition::VerificationCondition()
	{
		Type = VerificationConditionType::RegisterIsolation;
		Description = L"";
		IsValid = true;
		CounterExample = L"";
		FormalSpec = L"";
	}

	VerificationContext::VerificationContext()
	{
		ThreadMemoryDomains = std::unordered_map<int, MemoryDomain>();
		GeneratedConditions = std::vector<VerificationCondition>();
		EnableFormalChecks = false;
	}

	SecurityContext::SecurityContext()
	{
		MinAddr = 0;
		MaxAddr = 0;
		ActiveThreads = 0;
		OwnerThreadId = 0;
	}
}
