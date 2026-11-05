#include "DomainIsolationContract.h"

namespace YAKSys_Hybrid_CPU::Core::Diagnostics
{

	bool DomainIsolationContract::AreDomainsDisjoint(unsigned long long domainA, unsigned long long domainB)
	{
		return (domainA & domainB) == 0 && domainA != 0 && domainB != 0;
	}
}
