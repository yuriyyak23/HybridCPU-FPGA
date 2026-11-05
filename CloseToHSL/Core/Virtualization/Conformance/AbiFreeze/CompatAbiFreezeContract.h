#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompatAbiFreezeViolation : unsigned char
	{
		None = 0,
		OpcodeOutsideFrozenSet = 1,
		CsrOutsideFrozenAliasSet = 2,
		VmcsFieldOutsideFrozenAliasSet = 3
	};


}
