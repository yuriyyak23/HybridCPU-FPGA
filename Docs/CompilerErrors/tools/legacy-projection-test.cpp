#include "../../CloseToHSL/Core/Decoder/LegacyAdmissionProjection.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core; using namespace YAKSys_Hybrid_CPU::Core::Decoder;
int main(){auto p=ExecutionPlacement::Create(SlotClass::AluClass,SlotPinningKind::ClassFlexible); std::vector<int> r{1}; LegacyAdmissionProjection x(1,2,3,true,r,{},p,ResourceBitset{}); r[0]=2; assert(x.ReadRegisters[0]==1 && x.DomainTag==3); AdmissionProjectionFingerprint a(L" hash "),b(L"hash"); assert(a==b); try{AdmissionProjectionFingerprint c(L" ");assert(false);}catch(const std::invalid_argument&){} }
