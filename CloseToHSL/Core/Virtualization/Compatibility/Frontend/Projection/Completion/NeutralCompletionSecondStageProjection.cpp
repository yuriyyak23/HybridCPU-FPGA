#include "NeutralCompletionSecondStageProjection.h"
#include "../../../../../Runtime/Completion/Commit/NeutralArchitecturalCompletionFacts.h"
#include "../../../../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"
#include "../../../../../Runtime/Memory/Translation/CpuSecondStageTranslationMechanism.h"
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> RequireSecondStageProducer(
			std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer)
		{
			if (!producer) throw std::invalid_argument("producer");
			return producer;
		}
	}

	NeutralCompletionSecondStageProjectionOwner::NeutralCompletionSecondStageProjectionOwner(
		std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer)
		: _producerOwnerIdentity(RequireSecondStageProducer(producer)->getOwnerIdentity()),
		  _producerOwnerEpoch(RequireSecondStageProducer(producer)->getOwnerEpoch())
	{
		// Keep the validated registration alive while reading its immutable policy.
		auto validatedProducer = RequireSecondStageProducer(producer);
		const auto policy = validatedProducer->getPolicy();
		if (!policy || policy->OwnerName != L"CanonicalCpuSecondStageTranslationFaultProducer" ||
			policy->CompletionClass != NeutralArchitecturalCompletionClass::TranslationFault ||
			!policy->RequiresReason || !policy->AllowsQualification ||
			policy->AllowedAddressSemantic != NeutralFaultAddressSemantic::GuestPhysicalAddress ||
			policy->AllowedAuxiliarySemantic != NeutralFaultAuxiliarySemantic::SecondStageTranslationViolation ||
			!policy->RequiresTranslationProvenance || !policy->RequiresSecondStageProvenance)
			throw std::invalid_argument("Second-stage compatibility projection requires the exact neutral CPU second-stage producer policy.");
	}

	std::shared_ptr<NeutralCompletionSecondStageProjection> NeutralCompletionSecondStageProjectionOwner::Project(
		std::shared_ptr<NeutralCompletionObservationSnapshot> snapshot)
	{
		if (!snapshot || !snapshot->Facts)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedCompletionClass, L"Completion snapshot or neutral facts are missing.");
		if (snapshot->ProducerOwnerIdentity != _producerOwnerIdentity || snapshot->ProducerOwnerEpoch != _producerOwnerEpoch)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedProducer, L"Snapshot was not issued by the exact CPU second-stage producer.");
		if (snapshot->CompletionClass != NeutralArchitecturalCompletionClass::TranslationFault ||
			snapshot->Facts->CompletionClass != NeutralArchitecturalCompletionClass::TranslationFault)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedCompletionClass, L"Only the exact neutral translation-fault completion class is admitted.");
		if (!snapshot->TranslationProvenance.hasSecondStageIdentity())
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedAuxiliarySemantic, L"Exact CPU second-stage provenance is required.");
		if (!snapshot->Facts->FaultAddress.IsPresent)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedReasonAbsent, L"GuestPhysicalAddress requires an explicitly present address fact.");
		if (snapshot->Facts->FaultAddress.Semantic != NeutralFaultAddressSemantic::GuestPhysicalAddress)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedReasonSemantic, L"The address fact is not a neutral guest-physical address.");
		if (!snapshot->Facts->Reason.IsPresent || !snapshot->Facts->Qualification.IsPresent)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedQualificationAbsent, L"Reason-bound qualification is absent.");
		if (!snapshot->Facts->FaultAuxiliary.IsPresent ||
			snapshot->Facts->FaultAuxiliary.Semantic != NeutralFaultAuxiliarySemantic::SecondStageTranslationViolation)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedAuxiliarySemantic, L"Second-stage violation auxiliary is absent or semantically incompatible.");

		CpuSecondStageFaultKind kind{};
		CpuInstructionTranslationAccessKind accessKind{};
		unsigned short accessSize = 0;
		unsigned char pageWalkLevel = 0;
		if (!CpuSecondStageNeutralAuxiliary::TryDecode(snapshot->Facts->FaultAuxiliary.Value, kind, accessKind, accessSize, pageWalkLevel) ||
			kind == CpuSecondStageFaultKind::SourceStale)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedAuxiliarySemantic, L"Neutral second-stage auxiliary is not an accepted exact fault tuple.");

		const auto rawReason = snapshot->Facts->Reason.Value;
		if (rawReason > std::numeric_limits<unsigned char>::max())
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedReasonSemantic, L"Neutral reason is outside the frozen byte encoding.");
		const auto reason = static_cast<CpuInstructionTranslationFaultReason>(static_cast<unsigned char>(rawReason));
		const auto expectedReason = kind == CpuSecondStageFaultKind::Misconfiguration
			? CpuInstructionTranslationFaultReason::SecondStageMisconfiguration
			: CpuInstructionTranslationFaultReason::SecondStageViolation;
		if (reason != expectedReason)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedReasonSemantic, L"Neutral reason does not match the exact second-stage fault kind.");

		const unsigned long long expectedNeutralQualification =
			(static_cast<unsigned long long>(static_cast<unsigned char>(reason)) << 56) |
			(static_cast<unsigned long long>(static_cast<unsigned char>(accessKind)) << 48) |
			(static_cast<unsigned long long>(accessSize) << 32);
		if (snapshot->Facts->Qualification.Value != expectedNeutralQualification)
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedQualificationSemantic, L"Neutral qualification does not match the reason/access/size auxiliary tuple.");

		unsigned long long accessBits = 0;
		switch (accessKind)
		{
		case CpuInstructionTranslationAccessKind::InstructionFetch:
			accessBits = ExecuteAccessBit;
			break;
		case CpuInstructionTranslationAccessKind::ScalarLoad:
			accessBits = ReadAccessBit;
			break;
		case CpuInstructionTranslationAccessKind::ScalarStore:
			accessBits = WriteAccessBit;
			break;
		default:
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedQualificationSemantic, L"Second-stage access kind has no explicit compatibility mapping.");
		}

		unsigned long long compatibilityQualification = GuestLinearAddressValidBit |
			((static_cast<unsigned long long>(pageWalkLevel) & 0xFULL) << 12) | accessBits;
		if (kind == CpuSecondStageFaultKind::Misconfiguration)
			compatibilityQualification |= MisconfigurationBit;

		return std::make_shared<NeutralCompletionSecondStageProjection>(NeutralCompletionSecondStageProjection{
			NeutralCompletionCompatibilityProjectionDecision::Projected, snapshot->Facts->FaultAddress.Value,
			compatibilityQualification, L"Exact committed CPU second-stage facts mapped to the frozen compatibility read tuple."});
	}

	std::shared_ptr<NeutralCompletionSecondStageProjection> NeutralCompletionSecondStageProjectionOwner::Deny(
		NeutralCompletionCompatibilityProjectionDecision decision, const std::wstring &reason)
	{
		return std::make_shared<NeutralCompletionSecondStageProjection>(
			NeutralCompletionSecondStageProjection{decision, 0, 0, reason});
	}
}
