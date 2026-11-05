#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class BundleLegalityAuthority : unsigned char
	{
		Runtime = 0,
		CompilerEvidence = 1,
		CompatibilityProjection = 2
	};


}
