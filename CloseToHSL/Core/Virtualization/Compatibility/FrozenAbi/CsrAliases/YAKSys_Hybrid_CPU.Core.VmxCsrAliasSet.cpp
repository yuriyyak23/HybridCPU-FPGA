#include "YAKSys_Hybrid_CPU.Core.VmxCsrAliasSet.h"

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core
{

	std::span<const std::shared_ptr<VmxCsrAlias>> VmxCsrAliasSet::getFrozenAliases()
	{
		return FrozenAliasTable;
	}

	bool VmxCsrAliasSet::Contains(unsigned short address)
	{
		for (auto alias : getFrozenAliases())
		{
			if (alias->Address == address)
			{
				return true;
			}
		}

		return false;
	}

	bool VmxCsrAliasSet::TryGetAlias(unsigned short address, std::shared_ptr<VmxCsrAlias> &alias)
	{
		for (auto candidate : getFrozenAliases())
		{
			if (candidate->Address == address)
			{
				alias = candidate;
				return true;
			}
		}

		alias = nullptr;
		return false;
	}

	bool VmxCsrAliasSet::IsReadOnlyProjection(unsigned short address)
	{
		std::shared_ptr<VmxCsrAlias> alias;
		return TryGetAlias(address, alias) && alias->IsReadOnlyProjection;
	}
}
