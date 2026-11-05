#pragma once

#include "../../../../../Architecture/Common/InstructionOpcode.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"

using namespace YAKSys_Hybrid_CPU::Core;
using YAKSys_Hybrid_CPU::InstructionsEnum;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile

{

	enum class MatrixTileRuntimeResourceClass : unsigned char
	{
		None = 0,
		MatrixTileMemory = 1,
		MatrixTileCompute = 2
	};

	class MatrixTileResourceContour final
	{
	public:
		static constexpr unsigned char TileStreamLaneMask = 0b0100'0000;
		static constexpr unsigned char TileStreamLaneId = 6;
		static constexpr int StreamEngineChannel = 0;

		static bool IsMatrixTileMemoryOpcode(InstructionsEnum opcode);

		static bool IsMatrixTileMemoryOpcode(unsigned int opcode);

		static bool IsMatrixTileComputeOpcode(InstructionsEnum opcode);

		static bool IsMatrixTileComputeOpcode(unsigned int opcode);

		static bool IsMatrixTileOpcode(InstructionsEnum opcode);

		static bool IsMatrixTileOpcode(unsigned int opcode);

		static MatrixTileRuntimeResourceClass Classify(InstructionsEnum opcode);

		static MatrixTileRuntimeResourceClass Classify(unsigned int opcode);

		static SlotClass ResolveSlotClass(MatrixTileRuntimeResourceClass resourceClass);
	};

}
