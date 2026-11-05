#pragma once
#include "AddressSpaceId.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class AddressSpaceAuthority : unsigned char
	{
		RuntimeOwned = 0,
		CompatibilityProjection = 1
	};


}
