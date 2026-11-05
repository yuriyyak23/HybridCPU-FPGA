#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VectorStreamStateAuthority : unsigned char
	{
		RuntimeOwned = 0,
		CompatibilityProjection = 1
	};


}
