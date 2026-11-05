#include "YAKSys_Hybrid_CPU.Core.VmxOpcodeAliasSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::span<const std::shared_ptr<VmxOpcodeAlias>> VmxOpcodeAliasSet::getFrozenAliases()
	{
		return FrozenAliasTable;
	}

	bool VmxOpcodeAliasSet::Contains(unsigned short opcode)
	{
		for (auto alias : getFrozenAliases())
		{
			if (alias->Opcode == opcode)
			{
				return true;
			}
		}

		return false;
	}

	bool VmxOpcodeAliasSet::TryGetAlias(unsigned short opcode, std::shared_ptr<VmxOpcodeAlias> &alias)
	{
		for (auto candidate : getFrozenAliases())
		{
			if (candidate->Opcode == opcode)
			{
				alias = candidate;
				return true;
			}
		}

		alias = nullptr;
		return false;
	}
}
