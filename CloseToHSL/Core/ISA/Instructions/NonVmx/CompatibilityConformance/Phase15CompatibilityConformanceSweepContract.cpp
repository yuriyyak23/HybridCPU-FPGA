#include "Phase15CompatibilityConformanceSweepContract.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::CompatibilityConformance
{

	std::vector<std::wstring> Phase15CompatibilityConformanceSweepContract::getExecutableEvidenceChain()
	{
		return ExecutableEvidenceChain;
	}

	std::vector<std::wstring> Phase15CompatibilityConformanceSweepContract::getDeferredEvidenceGuards()
	{
		return DeferredEvidenceGuards;
	}

	std::vector<std::wstring> Phase15CompatibilityConformanceSweepContract::getSweepBoundaries()
	{
		return SweepBoundaries;
	}
}
