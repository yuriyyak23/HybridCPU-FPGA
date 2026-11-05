#include "YAKSys_Hybrid_CPU.Core.TrapPolicyService.h"
#include "../../../../../Runtime/Domains/Validation/DomainValidationResult.h"
#include "../../../../../Runtime/Domains/Validation/YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"
#include "VmxTrapProjectionMapper.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public TrapPolicyEvaluationResult Evaluate(TrapPolicyDescriptor? descriptor, System.Nullable<TrapPolicyBitmap> bitmap, TrapRequest request, bool domainValidated = true)
	std::shared_ptr<TrapPolicyEvaluationResult> TrapPolicyService::Evaluate(std::shared_ptr<TrapPolicyDescriptor> descriptor, std::shared_ptr<TrapPolicyBitmap> bitmap, std::shared_ptr<TrapRequest> request, bool domainValidated)
	{
		if (descriptor == nullptr)
		{
			return TrapPolicyEvaluationResult::Fail(DomainValidationFailureReason::MissingTrapPolicyDescriptor, request, L"Trap evaluation requires a runtime-owned trap policy descriptor.");
		}

		if (!descriptor->isRuntimeAuthoritative())
		{
			return TrapPolicyEvaluationResult::Fail(DomainValidationFailureReason::RuntimeAuthorityMissing, request, L"Compatibility projection cannot own trap policy evaluation.");
		}

		if (descriptor->getRequiresValidatedDomain() && !domainValidated)
		{
			return TrapPolicyEvaluationResult::Fail(DomainValidationFailureReason::RuntimeAuthorityMissing, request, L"Trap policy evaluation requires a validated domain descriptor.");
		}

		TrapPolicyClass policyClass = ToPolicyClass(request->TargetKind);
		if (policyClass == TrapPolicyClass::None || !descriptor->AllowsClass(policyClass))
		{
			return TrapPolicyEvaluationResult::Fail(DomainValidationFailureReason::TrapPolicyClassDenied, request, L"Trap policy descriptor denies the requested trap class.");
		}

		std::shared_ptr<NeutralTrapResult> neutralResult = bitmap == nullptr ? NeutralTrapResult::Continue(request) : bitmap->Evaluate(request);
		return std::make_shared<TrapPolicyEvaluationResult>(DomainValidationResult::getPassed(), VmxTrapProjectionMapper::getDefault()->Project(neutralResult));
	}

	TrapPolicyClass TrapPolicyService::ToPolicyClass(TrapTargetKind targetKind)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return targetKind switch
	//			{
	//				TrapTargetKind.InstructionOpcode => TrapPolicyClass.Instruction,
	//				TrapTargetKind.CsrAddress => TrapPolicyClass.Csr,
	//				TrapTargetKind.MemoryRange => TrapPolicyClass.Memory,
	//				TrapTargetKind.CompatibilityOperation => TrapPolicyClass.CompatibilityOperation,
	//				TrapTargetKind.LaneOperation => TrapPolicyClass.LaneOperation,
	//				_ => TrapPolicyClass.None,
	//			};
		switch (targetKind)
		{
		case TrapTargetKind::InstructionOpcode: return TrapPolicyClass::Instruction;
		case TrapTargetKind::CsrAddress: return TrapPolicyClass::Csr;
		case TrapTargetKind::MemoryRange: return TrapPolicyClass::Memory;
		case TrapTargetKind::CompatibilityOperation: return TrapPolicyClass::CompatibilityOperation;
		case TrapTargetKind::LaneOperation: return TrapPolicyClass::LaneOperation;
		default: return TrapPolicyClass::None;
		}
	}
}
