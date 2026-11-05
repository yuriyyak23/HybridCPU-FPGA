#pragma once

#include "BranchHint.h"
#include "StealabilityPolicy.h"
#include "LocalityHint.h"
#include "ThermalHint.h"

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata
{
	/// Compiler-generated scheduling hints for one instruction slot.
	/// Defaults mean that no compiler policy overrides the runtime defaults.
	struct CompilerAnnotation final
	{
		YAKSys_Hybrid_CPU::Core::BranchHint BranchHint = YAKSys_Hybrid_CPU::Core::BranchHint::None;
		YAKSys_Hybrid_CPU::Core::StealabilityPolicy StealabilityPolicy = YAKSys_Hybrid_CPU::Core::StealabilityPolicy::Stealable;
		unsigned char DonorVtHint = 0xFF;
		YAKSys_Hybrid_CPU::Core::LocalityHint LocalityHint = YAKSys_Hybrid_CPU::Core::LocalityHint::None;
		unsigned char PreferredVt = 0xFF;
		YAKSys_Hybrid_CPU::Core::ThermalHint ThermalHint = YAKSys_Hybrid_CPU::Core::ThermalHint::None;
	};
}
