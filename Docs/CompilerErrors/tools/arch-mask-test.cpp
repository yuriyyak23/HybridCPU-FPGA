#include "../../CloseToHSL/Core/Pipeline/Certificates/ArchLegalityMask.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main(){ ArchLegalityMask a(1,0), b(1ULL<<16,0); assert(a.ConflictsWith(b)); assert(a.Equals(std::any(ArchLegalityMask(1,0)))); assert(!a.Equals(std::any(1))); assert((a|b).Mask.Low==65537); assert(a.ToString().find(L"ArchLegalityMask(")==0); BackendHazardMask x(1,0), y(1ULL<<16,0); assert(x.ConflictsWith(y)); assert(x.Equals(std::any(BackendHazardMask(1,0)))); assert((x|y).Mask.Low==65537); }
