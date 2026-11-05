#pragma once

#include "../../../NonRTL/Processor/Core/Processor.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <functional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Storage-only containment for stable core identity and explicit platform,
	/// execution-mode and interrupt-dispatch bindings. Construction and explicit
	/// platform replacement retain lifecycle authority.
	/// </summary>
	class CoreBindingState final : public std::enable_shared_from_this<CoreBindingState>
	{
	public:
		unsigned int CoreId = 0;
		std::shared_ptr<CpuCorePlatformContext> PlatformContext;
		ProcessorMode ExecutionMode = static_cast<ProcessorMode>(0);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal System.Func<Processor.DeviceType, ushort, ulong, byte>? InterruptDispatcher;
		std::function<unsigned char(Processor::DeviceType, unsigned short, unsigned long long)> InterruptDispatcher;
	};

}
