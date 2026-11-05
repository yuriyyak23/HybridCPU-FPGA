#pragma once

namespace YAKSys_Hybrid_CPU::Core::Memory { class CpuInstructionTranslationOwner; struct CpuInstructionTranslationFault; }
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class CpuInstructionTranslationState final : public std::enable_shared_from_this<CpuInstructionTranslationState>
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal Memory.CpuInstructionTranslationOwner Owner = null!;
	public:
		std::shared_ptr<Memory::CpuInstructionTranslationOwner> Owner = nullptr;
		std::optional<std::shared_ptr<Memory::CpuInstructionTranslationFault>> PendingFetchFault;
	};

}
