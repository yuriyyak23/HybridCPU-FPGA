#pragma once

#include "../../../../../Runtime/Completion/Commit/ArchitecturalCompletionCommitOwner.h"
#include <string>
#include <limits>
#include <stdexcept>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NeutralCompletionCompatibilityProjectionDecision : unsigned char
	{
		Projected = 0,
		DeniedProducer = 1,
		DeniedCompletionClass = 2,
		DeniedReasonAbsent = 3,
		DeniedQualificationAbsent = 4,
		DeniedReasonSemantic = 5,
		DeniedQualificationSemantic = 6,
		DeniedAuxiliarySemantic = 7
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralCompletionReasonQualificationProjection(NeutralCompletionCompatibilityProjectionDecision Decision, VmExitReason ExitReason, ulong ExitQualification, string Reason)
//	{
//		internal bool IsProjected
//		{
//			get
//			{
//				return Decision == NeutralCompletionCompatibilityProjectionDecision.Projected;
//			}
//		}
//	}

	/// <summary>
	/// Exact compatibility-only mapping for committed neutral completion facts.
	/// The mapper owns no completion state and cannot create or mutate runtime
	/// authority. Unlisted producer/reason/qualification tuples are denied.
	/// </summary>
    enum class VmExitReason : int;
    struct NeutralCompletionReasonQualificationProjection final {
        NeutralCompletionCompatibilityProjectionDecision Decision;
        VmExitReason ExitReason;
        unsigned long long ExitQualification;
        std::wstring Reason;
        bool isProjected() const noexcept { return Decision == NeutralCompletionCompatibilityProjectionDecision::Projected; }
    };
	class NeutralCompletionReasonQualificationProjectionOwner final : public std::enable_shared_from_this<NeutralCompletionReasonQualificationProjectionOwner>
	{
	private:
		static constexpr unsigned long long QualificationReservedMask = 0x0000'0000'FFFF'FFFFULL;
		const unsigned long long _producerOwnerIdentity;
		const unsigned long long _producerOwnerEpoch;

	public:
		NeutralCompletionReasonQualificationProjectionOwner(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer);

		std::shared_ptr<NeutralCompletionReasonQualificationProjection> Project(std::shared_ptr<NeutralCompletionObservationSnapshot> snapshot);

	private:
		static std::shared_ptr<NeutralCompletionReasonQualificationProjection> Deny(NeutralCompletionCompatibilityProjectionDecision decision, const std::wstring &reason);
	};

}
