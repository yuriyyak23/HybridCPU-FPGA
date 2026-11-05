#include "LoopPhaseClassProfile.h"

namespace YAKSys_Hybrid_CPU::Core::Diagnostics
{
	LoopPhaseClassProfile::LoopPhaseClassProfile(unsigned long long loopPcAddress, int iterationsSampled, double aluFreeVariance, double lsuFreeVariance, double dmaStreamFreeVariance, double branchControlFreeVariance, double systemSingletonFreeVariance, double overallClassVariance, double templateReuseRate)
		: LoopPcAddress(loopPcAddress), IterationsSampled(iterationsSampled), AluFreeVariance(aluFreeVariance), LsuFreeVariance(lsuFreeVariance), DmaStreamFreeVariance(dmaStreamFreeVariance), BranchControlFreeVariance(branchControlFreeVariance), SystemSingletonFreeVariance(systemSingletonFreeVariance), OverallClassVariance(overallClassVariance), TemplateReuseRate(templateReuseRate)
	{
	}
}
