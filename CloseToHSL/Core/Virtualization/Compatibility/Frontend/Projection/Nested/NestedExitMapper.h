#pragma once

#include "NestedInterceptTranslator.h"
#include "../Events/TrapDecision.h"
#include "../../Retire/VmxRetireModel.h"
#include "../../../../../Runtime/Events/Injection/EventInjectionDescriptor.h"
#include "../../../../../Runtime/Events/Traps/TrapRequest.h"
#include "../../../../../Runtime/Nested/MemoryComposition/NestedMemoryCompositionService.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	struct NestedExitMapping final
	{
		NestedExitTarget Target;
		VmExitReason ExitReason;
		unsigned long long Qualification;
		std::wstring Message;
		bool getExitsToL1() const { return Target == NestedExitTarget::L1; }
		bool getExitsToL0() const { return Target == NestedExitTarget::L0; }
		static std::shared_ptr<NestedExitMapping> None() { return std::make_shared<NestedExitMapping>(NestedExitMapping{NestedExitTarget::None, VmExitReason::None, 0, L""}); }
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedExitMapping(NestedExitTarget Target, VmExitReason ExitReason, ulong Qualification, string Message)
//	{
//		public bool ExitsToL1
//		{
//			get
//			{
//				return Target == NestedExitTarget.L1;
//			}
//		}
//
//		public bool ExitsToL0
//		{
//			get
//			{
//				return Target == NestedExitTarget.L0;
//			}
//		}
//
//		public static NestedExitMapping None
//		{
//			get;
//		} = new(NestedExitTarget.None, VmExitReason.None, 0, string.Empty);
//	}


}
