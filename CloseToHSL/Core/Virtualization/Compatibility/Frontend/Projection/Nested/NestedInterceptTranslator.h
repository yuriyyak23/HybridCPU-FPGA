#pragma once

#include "../../../../../Runtime/Events/Traps/TrapPolicyBitmap.h"
#include "../../../../../Runtime/Events/Traps/TrapRequest.h"
#include "../Events/TrapDecision.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	enum class NestedExitTarget : unsigned char
	{
		None = 0,
		L1 = 1,
		L0 = 2
	};

	struct NestedInterceptPolicy final
	{
		TrapPolicyClass VirtualizableClasses;
		bool AllowNestedLaneReflection;
	};

	struct NestedInterceptTranslationResult final
	{
		NestedExitTarget Target;
		VmExitReason ExitReason;
		std::shared_ptr<TrapDecision> Decision;
		std::wstring Message;
		bool getShouldExit() const { return Target != NestedExitTarget::None; }
		static std::shared_ptr<NestedInterceptTranslationResult> None(std::shared_ptr<TrapRequest> request) { return std::make_shared<NestedInterceptTranslationResult>(NestedInterceptTranslationResult{NestedExitTarget::None, VmExitReason::None, TrapDecision::NoExit(std::move(request)), L""}); }
		static std::shared_ptr<NestedInterceptTranslationResult> ToL1(std::shared_ptr<TrapDecision> decision) { return std::make_shared<NestedInterceptTranslationResult>(NestedInterceptTranslationResult{NestedExitTarget::L1, decision->ExitReason, std::move(decision), L"L1 requested intercept is virtualized inside the L0 policy envelope."}); }
		static std::shared_ptr<NestedInterceptTranslationResult> ToL0(std::shared_ptr<TrapRequest> request, VmExitReason reason, const std::wstring &message) { return std::make_shared<NestedInterceptTranslationResult>(NestedInterceptTranslationResult{NestedExitTarget::L0, reason, TrapDecision::Exit(std::move(request), reason), message}); }
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedInterceptPolicy(TrapPolicyClass VirtualizableClasses, bool AllowNestedLaneReflection)
//	{
//		public static NestedInterceptPolicy FirstReleaseNoLanePassthrough
//		{
//			get;
//		} = new(TrapPolicyClass.Instruction | TrapPolicyClass.Csr | TrapPolicyClass.Memory | TrapPolicyClass.CompatibilityOperation, AllowNestedLaneReflection: false);
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedInterceptTranslationResult(NestedExitTarget Target, VmExitReason ExitReason, TrapDecision Decision, string Message)
//	{
//		public bool ShouldExit
//		{
//			get
//			{
//				return Target != NestedExitTarget.None;
//			}
//		}
//
//		public static NestedInterceptTranslationResult None(TrapRequest request)
//		{
//			return new(NestedExitTarget.None, VmExitReason.None, TrapDecision.NoExit(request), string.Empty);
//		}
//
//		public static NestedInterceptTranslationResult ToL1(TrapDecision decision)
//		{
//			return new(NestedExitTarget.L1, decision.ExitReason, decision, "L1 requested intercept is virtualized inside the L0 policy envelope.");
//		}
//
//		public static NestedInterceptTranslationResult ToL0(TrapRequest request, VmExitReason reason, string message)
//		{
//			return new(NestedExitTarget.L0, reason, TrapDecision.Exit(request, reason), message);
//		}
//	}


}
