#include "../../ExternalRuntime/Contracts/ExternalOperationIdentity.h"
#include <map>
#include <cassert>
using namespace HybridCPU::ExternalRuntime::Contracts;
int main(){auto id=Guid::NewGuid();ExternalOperationIdentity a{{id},{1}}, b{{id},{1}}, c{{id},{2}}; std::map<ExternalOperationIdentity,int> entries; entries.emplace(a,7);assert(entries.contains(b));assert(!entries.contains(c));assert(!entries.emplace(b,8).second);}
