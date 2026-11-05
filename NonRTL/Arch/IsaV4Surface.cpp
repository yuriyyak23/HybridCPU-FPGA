#include "IsaV4Surface.h"

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Arch
{

	std::shared_ptr<const std::unordered_set<std::wstring>> IsaV4Surface::Policy(const std::wstring &policyId)
	{
		return GeneratedIsaCatalog::GetStaticPolicy(policyId);
	}
}


