#pragma once

#include "VmxCompatDecodeBoundary.h"
#include <memory>

// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCompatDecodeBoundary final : public std::enable_shared_from_this<VmxCompatDecodeBoundary>
	{
	public:
		std::shared_ptr<VmxCompatDecodeResult> Decode(std::shared_ptr<VmxCompatDecodeRequest> request);

		static bool IsFrozenVmxOpcode(unsigned short opcode);
	};
}
