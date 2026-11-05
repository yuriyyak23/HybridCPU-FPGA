#include "../../CloseToHSL/Core/Pipeline/Safety/ResourceBitset.h"
#include "../../CloseToHSL/Core/Pipeline/Certificates/SafetyMask128.h"
#include <any>
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main() {
 ResourceBitset a(1,0), b(1,0), w(1ULL<<16,0), high(0,1); assert(a.Equals(std::any(b))); assert(!a.Equals(std::any(1))); assert(a.GetHashCode()==b.GetHashCode());
 assert(!a.ConflictsWith(b) && a.ConflictsWith(w) && high.ConflictsWith(high)); assert(a.ToString()==L"ResourceBitset(Low=0x0000000000000001, High=0x0000000000000000)");
 ResourceBitset banks(0x0000000100000001ULL,0x0000000100000001ULL); assert(banks.ActiveBanks()==15 && banks.GetBank(0)==1 && banks.GetBank(1)==1 && banks.GetBank(2)==1 && banks.GetBank(3)==1 && banks.GetBank(4)==0);
 auto combined=a|high; assert(combined.Low==1 && combined.High==1); assert(ResourceBitset::Zero.isZero());
 SafetyMask128 x(1,0), y(1,0), z(1ULL<<16,0); assert(x.Equals(std::any(y)) && !x.Equals(std::any(1))); assert(x.GetHashCode()==y.GetHashCode()); assert(!x.ConflictsWith(y) && x.ConflictsWith(z)); assert(x.ToString()==L"SafetyMask128(Low=0x0000000000000001, High=0x0000000000000000)"); assert(SafetyMask128::Zero.isZero());
}
