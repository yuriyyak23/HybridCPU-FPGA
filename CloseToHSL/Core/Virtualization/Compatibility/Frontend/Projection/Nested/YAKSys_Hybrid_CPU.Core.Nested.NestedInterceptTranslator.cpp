#include "YAKSys_Hybrid_CPU.Core.Nested.NestedInterceptTranslator.h"
#include "../Events/VmxTrapProjectionMapper.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	YAKSys_Hybrid_CPU::Core::TrapPolicyClass NestedInterceptTranslator::ToClass(TrapTargetKind targetKind)
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
	}

	std::shared_ptr<NestedInterceptTranslationResult> NestedInterceptTranslator::Translate(std::shared_ptr<TrapRequest> request, std::shared_ptr<TrapPolicyBitmap> l1RequestedIntercepts, std::shared_ptr<TrapPolicyBitmap> l0MandatoryIntercepts, std::shared_ptr<NestedInterceptPolicy> policy)
	{
		if (l1RequestedIntercepts == nullptr || l0MandatoryIntercepts == nullptr)
		{
			throw std::invalid_argument("Nested intercept evaluation requires both L1 and L0 trap policy bitmaps.");
		}

		std::shared_ptr<VmxTrapProjectionMapper> mapper = VmxTrapProjectionMapper::getDefault();

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		std::shared_ptr<NeutralTrapResult> l0Result = l0MandatoryIntercepts->Evaluate(request);
		if (l0Result != nullptr && l0Result->ShouldTrap)
		{
			VmExitReason reason = mapper->ProjectReason(l0Result->Kind);
			return NestedInterceptTranslationResult::ToL0(request, reason, L"L0 mandatory intercept dominates the nested L1 intercept policy.");
		}

		TrapPolicyClass requestClass = ToClass(request->TargetKind);
		if (requestClass == TrapPolicyClass::LaneOperation && !policy->AllowNestedLaneReflection)
		{
			return NestedInterceptTranslationResult::ToL0(request, VmExitReason::SecurityPolicyViolation, L"Nested Lane6/Lane7 passthrough is disabled for the first nested release.");
		}

		std::shared_ptr<NeutralTrapResult> l1Result = l1RequestedIntercepts->Evaluate(request);
		std::shared_ptr<TrapDecision> l1Decision = mapper->Project(l1Result);
		if (!l1Decision->ShouldExit)
		{
			return NestedInterceptTranslationResult::None(request);
		}

		if ((policy->VirtualizableClasses & requestClass) == requestClass)
		{
			return NestedInterceptTranslationResult::ToL1(l1Decision);
		}

		return NestedInterceptTranslationResult::ToL0(request, VmExitReason::SecurityPolicyViolation, L"L1 requested an intercept class outside the L0 nested virtualization envelope.");
	}
}
