#include "../../CloseToHSL/Core/Pipeline/Safety/SafetyVerifier.VirtualizationOperationAdmission.h"
#include <cassert>
#include <stdexcept>
using YAKSys_Hybrid_CPU::Core::VirtualizationE2Digest;
int main() {
 auto compute=[](const std::wstring& hex){return VirtualizationE2Digest::Compute(1,2,3,-1,5,6,7,8,L"\u0442\u0435\u0441\u0442",hex,L"12ab",9,10,11,L"NS",L"op",12,L"3456",L"789a",13,14,15,16);};
 assert(compute(L"00ff")==L"843a46e395e459253d659626004cdb3bfd4483deaa0292725234cb3f818b769e");
 bool bad=false;try {compute(L"0z");} catch(const std::invalid_argument&) {bad=true;} assert(bad);
}
