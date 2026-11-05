#pragma once

#include "../Architecture/Registers/Physical/PhysicalRegisterFile.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Live PRF/rename backend containment. The referenced register structures
	/// retain their allocation, restore and retire publication protocols.
	/// </summary>
	class BackendState final : public std::enable_shared_from_this<BackendState>
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal PhysicalRegisterFile PhysicalRegisters = null!;
	public:
		std::shared_ptr<PhysicalRegisterFile> PhysicalRegisters = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal RenameMap RenameMap = null!;
		std::shared_ptr<RenameMap> RenameMap = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal CommitMap CommitMap = null!;
		std::shared_ptr<CommitMap> CommitMap = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal FreeList FreeList = null!;
		std::shared_ptr<FreeList> FreeList = nullptr;
	};

}
