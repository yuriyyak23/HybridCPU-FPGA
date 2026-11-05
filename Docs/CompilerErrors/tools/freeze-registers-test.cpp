#include "../../CloseToHSL/Core/Decoder/FreezeRegisters.h"
#include <cassert>
using YAKSys_Hybrid_CPU::Core::Decoder::FreezeRegisterIds;
int main(){std::vector<int> input{0,31,42}; auto frozen=FreezeRegisterIds(input);input[0]=9;assert(frozen[0]==0 && frozen[2]==42);assert(FreezeRegisterIds({}).empty());try{FreezeRegisterIds({1,-1});assert(false);}catch(const std::out_of_range&){} }
