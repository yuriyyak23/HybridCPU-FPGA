#include "YAKSys_Hybrid_CPU.Core.Nested.NestedExitMapper.h"
#include "NestedInterceptTranslator.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../../../Runtime/Memory/Translation/NestedTranslationResult.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	std::shared_ptr<NestedExitMapping> NestedExitMapper::FromIntercept(std::shared_ptr<NestedInterceptTranslationResult> intercept)
	{
		if (intercept == nullptr || intercept->Target == NestedExitTarget::None)
		{
			return NestedExitMapping::None();
		}
		return std::make_shared<NestedExitMapping>(NestedExitMapping{intercept->Target, intercept->ExitReason, intercept->Decision->ExitQualification->Encode(), intercept->Message});
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return intercept.Target switch
	//			{
	//				NestedExitTarget.L1 => new(
	//					NestedExitTarget.L1, intercept.ExitReason, intercept.Decision.ExitQualification.Encode(), intercept.Message), NestedExitTarget.L0 => new(
	//					NestedExitTarget.L0,
	//					intercept.ExitReason,
	//					intercept.Decision.ExitQualification.Encode(),
	//					intercept.Message),
	//				_ => NestedExitMapping.None,
	//			};
	}

	std::shared_ptr<NestedExitMapping> NestedExitMapper::InvalidChildDomainIntent(unsigned short fieldId, const std::wstring &message)
	{
		return std::make_shared<NestedExitMapping>(NestedExitMapping{NestedExitTarget::L0, VmExitReason::SecurityPolicyViolation, fieldId, message});
	}

	std::shared_ptr<NestedExitMapping> NestedExitMapper::InvalidVmcs12(unsigned short fieldId, const std::wstring &message)
	{
		return InvalidChildDomainIntent(fieldId, message);
	}

	std::shared_ptr<NestedExitMapping> NestedExitMapper::UnsupportedOperation(VmxOperationKind operation)
	{
		return std::make_shared<NestedExitMapping>(NestedExitMapping{NestedExitTarget::L0, VmExitReason::SecurityPolicyViolation, static_cast<unsigned long long>(operation), L"Nested compatibility operation is unsupported by the L0 policy envelope."});
	}

	std::shared_ptr<NestedExitMapping> NestedExitMapper::VirtualInterrupt(std::shared_ptr<EventInjectionDescriptor> descriptor, bool reflectToL1)
	{
		if (!descriptor->IsValid())
		{
			return NestedExitMapping::None();
		}

		return std::make_shared<NestedExitMapping>(NestedExitMapping{reflectToL1 ? NestedExitTarget::L1 : NestedExitTarget::L0, VmExitReason::VirtualInterrupt, descriptor->Payload, reflectToL1 ? L"Virtual interrupt is deterministically reflected to L1." : L"Virtual interrupt requires L0 handling by nested policy."});
	}

	std::shared_ptr<NestedExitMapping> NestedExitMapper::PreemptionTimer(std::shared_ptr<TrapDecision> decision, bool reflectToL1)
	{
		if (!decision->ShouldExit)
		{
			return NestedExitMapping::None();
		}

		return std::make_shared<NestedExitMapping>(NestedExitMapping{reflectToL1 ? NestedExitTarget::L1 : NestedExitTarget::L0, VmExitReason::VmxPreemptionTimerExpired, decision->ExitQualification->Encode(), reflectToL1 ? L"L2 preemption timer expiry is reflected to L1." : L"L2 preemption timer expiry is retained by L0 scheduling policy."});
	}

	std::shared_ptr<NestedExitMapping> NestedExitMapper::LaneOperationBlocked(std::shared_ptr<TrapRequest> request)
	{
		return std::make_shared<NestedExitMapping>(NestedExitMapping{NestedExitTarget::L0, VmExitReason::SecurityPolicyViolation, request->Target | ((request->Auxiliary & 0xFFFFULL) << 16), L"Nested Lane6/Lane7 passthrough is blocked; no host token, backend binding, or completion evidence is exposed."});
	}

	std::shared_ptr<NestedExitMapping> NestedExitMapper::FromNestedMemoryComposition(std::shared_ptr<NestedMemoryCompositionResult> result)
	{
		if (result->Succeeded || (result->Translation->Status != NestedTranslationStatus::SecondStageViolation && result->Translation->Status != NestedTranslationStatus::SecondStageMisconfiguration))
		{
			return NestedExitMapping::None();
		}

		VmExitReason reason = result->Translation->Status == NestedTranslationStatus::SecondStageMisconfiguration ? VmExitReason::EptMisconfiguration : VmExitReason::EptViolation;

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return result.FaultStage switch
	//			{
	//				NestedMemoryCompositionStage.ChildDomainTranslation => new(
	//					NestedExitTarget.L1, reason, result.Translation.Violation.QualificationBits, "Child-domain translation fault is reflected as an L2-to-L1 virtual VM-exit."), NestedMemoryCompositionStage.HostDomainTranslation => new(
	//					NestedExitTarget.L0,
	//					reason,
	//					result.Translation.Violation.QualificationBits,
	//					"Host-domain translation/IOMMU policy fault remains an L1-to-L0 VM-exit."),
	//				_ => new(
	//					NestedExitTarget.L0,
	//					VmExitReason.SecurityPolicyViolation,
	//					result.Translation.Violation.QualificationBits,
	//					"Nested translation fault could not be safely attributed to L1 policy."),
	//			};
	}
}
