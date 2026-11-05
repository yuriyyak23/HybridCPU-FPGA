#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../Memory/IAtomicMemoryUnit.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// MEM/WB-facing latches and per-core memory bindings. Referenced memory
	/// subsystem/controller, main memory and atomic unit retain their authorities.
	/// </summary>
	class MemoryPipelineState final : public std::enable_shared_from_this<MemoryPipelineState>
	{
	public:
		Processor::CPU_Core::MemoryStage Memory;
		Processor::CPU_Core::WriteBackStage WriteBack;
		std::shared_ptr<std::vector<unsigned char>> ExplicitPacketImmediateReadBuffer;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Processor.MainMemoryArea? MainMemory;
		std::shared_ptr<Processor::MainMemoryArea> MainMemory;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal YAKSys_Hybrid_CPU.Memory.MemorySubsystem? MemorySubsystem;
		std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> MemorySubsystem;
		bool MemorySubsystemCaptured = false;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Memory.IAtomicMemoryUnit? AtomicMemoryUnit;
		std::shared_ptr<Memory::IAtomicMemoryUnit> AtomicMemoryUnit;
	};

}
