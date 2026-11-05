#include "NeutralCompletionReasonQualificationProjection.h"
#include "../../../../../Runtime/Completion/Commit/NeutralArchitecturalCompletionFacts.h"
#include "../../../../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"

using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	NeutralCompletionReasonQualificationProjectionOwner::NeutralCompletionReasonQualificationProjectionOwner(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer)
		: _producerOwnerIdentity(producer ? producer->getOwnerIdentity() : throw std::invalid_argument("producer")),
		  _producerOwnerEpoch(producer ? producer->getOwnerEpoch() : throw std::invalid_argument("producer"))
	{
		if (producer->getPolicy()->CompletionClass != NeutralArchitecturalCompletionClass::TranslationFault || !producer->getPolicy()->RequiresReason || !producer->getPolicy()->AllowsQualification || producer->getPolicy()->AllowedAddressSemantic != NeutralFaultAddressSemantic::VirtualAddress || producer->getPolicy()->AllowedAuxiliarySemantic != NeutralFaultAuxiliarySemantic::TranslationFault)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Completion compatibility mapping requires the exact neutral CPU translation-fault producer policy.", nameof(producer));
			throw std::invalid_argument("Completion compatibility mapping requires the exact neutral CPU translation-fault producer policy.");
		}

	}

	std::shared_ptr<NeutralCompletionReasonQualificationProjection> NeutralCompletionReasonQualificationProjectionOwner::Project(std::shared_ptr<NeutralCompletionObservationSnapshot> snapshot)
	{
		if (!snapshot || !snapshot->Facts)
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedCompletionClass, L"Completion observation snapshot is missing neutral facts.");
		}
		if (snapshot->ProducerOwnerIdentity != _producerOwnerIdentity || snapshot->ProducerOwnerEpoch != _producerOwnerEpoch)
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedProducer, L"Completion snapshot was not issued by the exact accepted CPU translation-fault producer.");
		}
		if (snapshot->CompletionClass != NeutralArchitecturalCompletionClass::TranslationFault || snapshot->Facts->CompletionClass != NeutralArchitecturalCompletionClass::TranslationFault)
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedCompletionClass, L"Only the exact neutral CPU translation-fault completion class is mapped.");
		}
		if (!snapshot->Facts->Reason.IsPresent)
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedReasonAbsent, L"ExitReason mapping requires a present neutral reason fact.");
		}
		if (!snapshot->Facts->Qualification.IsPresent)
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedQualificationAbsent, L"ExitQualification mapping requires a present reason-bound neutral qualification fact.");
		}
		if (!snapshot->Facts->FaultAuxiliary.IsPresent || snapshot->Facts->FaultAuxiliary.Semantic != NeutralFaultAuxiliarySemantic::TranslationFault || snapshot->Facts->FaultAuxiliary.Value == 0)
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedAuxiliarySemantic, L"CPU translation compatibility mapping requires its neutral non-zero owner-epoch auxiliary fact.");
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: CpuInstructionTranslationFaultReason reason = unchecked((CpuInstructionTranslationFaultReason)(byte)snapshot.Facts.Reason.Value);
		CpuInstructionTranslationFaultReason reason = static_cast<CpuInstructionTranslationFaultReason>(static_cast<unsigned char>(snapshot->Facts->Reason.Value));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (snapshot->Facts->Reason.Value > std::numeric_limits<unsigned char>::max() || (reason != CpuInstructionTranslationFaultReason::AccessDenied && reason != CpuInstructionTranslationFaultReason::OwnerScopeMismatch))
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedReasonSemantic, L"The neutral translation reason has no owner-approved compatibility ExitReason mapping.");
		}

		unsigned long long qualification = snapshot->Facts->Qualification.Value;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: byte encodedReason = unchecked((byte)(qualification >> 56));
		unsigned char encodedReason = static_cast<unsigned char>(qualification >> 56);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: byte encodedAccess = unchecked((byte)(qualification >> 48));
		unsigned char encodedAccess = static_cast<unsigned char>(qualification >> 48);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ushort encodedSize = unchecked((ushort)(qualification >> 32));
		unsigned short encodedSize = static_cast<unsigned short>(qualification >> 32);
		if (encodedReason != static_cast<unsigned char>(reason) ||
			encodedAccess < static_cast<unsigned char>(CpuInstructionTranslationAccessKind::InstructionFetch) ||
			encodedAccess > static_cast<unsigned char>(CpuInstructionTranslationAccessKind::ScalarStore) ||
			encodedSize == 0 || (qualification & QualificationReservedMask) != 0)
		{
			return Deny(NeutralCompletionCompatibilityProjectionDecision::DeniedQualificationSemantic, L"Neutral qualification is not the exact reason/access/size encoding accepted by the mapping owner.");
		}

		return std::make_shared<NeutralCompletionReasonQualificationProjection>(NeutralCompletionReasonQualificationProjection{
			NeutralCompletionCompatibilityProjectionDecision::Projected, VmExitReason::SecurityPolicyViolation, qualification,
			L"Exact CPU access-policy translation fault projected to the frozen compatibility reason and its reason-bound qualification."});
	}

	std::shared_ptr<NeutralCompletionReasonQualificationProjection> NeutralCompletionReasonQualificationProjectionOwner::Deny(NeutralCompletionCompatibilityProjectionDecision decision, const std::wstring &reason)
	{
		return std::make_shared<NeutralCompletionReasonQualificationProjection>(
			NeutralCompletionReasonQualificationProjection{decision, VmExitReason::None, 0, reason});
	}
}
