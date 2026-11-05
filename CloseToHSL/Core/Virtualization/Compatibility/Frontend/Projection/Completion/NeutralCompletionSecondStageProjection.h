#pragma once

#include "../../../../../Runtime/Completion/Commit/ArchitecturalCompletionCommitOwner.h"
#include "NeutralCompletionReasonQualificationProjection.h"
#include <string>
#include <limits>
#include <stdexcept>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralCompletionSecondStageProjection(NeutralCompletionCompatibilityProjectionDecision Decision, ulong GuestPhysicalAddress, ulong EptViolationQualification, string Reason)
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
	/// Exact compatibility-only projection of one committed neutral CPU
	/// second-stage translation-fault tuple. This owner cannot create completion
	/// state and does not consume VMCS, nested or IOMMU authority.
	/// </summary>
    struct NeutralCompletionSecondStageProjection final {
        NeutralCompletionCompatibilityProjectionDecision Decision;
        unsigned long long GuestPhysicalAddress, EptViolationQualification;
        std::wstring Reason;
        bool isProjected() const noexcept { return Decision == NeutralCompletionCompatibilityProjectionDecision::Projected; }
    };
	class NeutralCompletionSecondStageProjectionOwner final : public std::enable_shared_from_this<NeutralCompletionSecondStageProjectionOwner>
	{
	private:
		static constexpr unsigned long long ReadAccessBit = 1ULL << 0;
		static constexpr unsigned long long WriteAccessBit = 1ULL << 1;
		static constexpr unsigned long long ExecuteAccessBit = 1ULL << 2;
		static constexpr unsigned long long GuestLinearAddressValidBit = 1ULL << 7;
		static constexpr unsigned long long MisconfigurationBit = 1ULL << 9;
		const unsigned long long _producerOwnerIdentity;
		const unsigned long long _producerOwnerEpoch;

	public:
		NeutralCompletionSecondStageProjectionOwner(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer);

		std::shared_ptr<NeutralCompletionSecondStageProjection> Project(std::shared_ptr<NeutralCompletionObservationSnapshot> snapshot);

	private:
		static std::shared_ptr<NeutralCompletionSecondStageProjection> Deny(NeutralCompletionCompatibilityProjectionDecision decision, const std::wstring &reason);
	};

}
