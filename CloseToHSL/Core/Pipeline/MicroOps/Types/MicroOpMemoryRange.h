#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	/// Address and length of a memory range accessed by a micro-operation.
	struct MicroOpMemoryRange
	{
		unsigned long long Address;
		unsigned long long Length;
	};
}
