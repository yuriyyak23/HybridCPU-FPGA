#pragma once

namespace YAKSys_Hybrid_CPU::Core::Diagnostics
{
	/// <summary>Loop-local replay/class-capacity telemetry exported for compiler-side steady-state shaping.</summary>
	struct LoopPhaseClassProfile final
	{
		const unsigned long long LoopPcAddress;
		const int IterationsSampled;
		const double AluFreeVariance;
		const double LsuFreeVariance;
		const double DmaStreamFreeVariance;
		const double BranchControlFreeVariance;
		const double SystemSingletonFreeVariance;
		const double OverallClassVariance;
		const double TemplateReuseRate;

		LoopPhaseClassProfile(unsigned long long loopPcAddress, int iterationsSampled, double aluFreeVariance, double lsuFreeVariance, double dmaStreamFreeVariance, double branchControlFreeVariance, double systemSingletonFreeVariance, double overallClassVariance, double templateReuseRate);
	};
}
