#include "../../CloseToHSL/Core/Runtime/Memory/Translation/CpuInstructionTranslationRegions.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core::Memory;
int main(){
 assert((CpuInstructionTranslationScope{1,2,3}.isValid())); assert(!(CpuInstructionTranslationScope{1,2,4}.isValid()));
 auto max=std::numeric_limits<unsigned long long>::max();
 assert((CpuInstructionTranslationRegion{max,0,1,true,false,false}.isValid()));
 assert(!(CpuInstructionTranslationRegion{max,0,2,true,false,false}.isValid()));
 assert(!(CpuInstructionTranslationRegion{0,0,1,false,false,false}.isValid()));
 assert((CpuInstructionStageOneRegion{0,max,1,false,true,false}.isValid()));
 assert(!(CpuInstructionStageOneRegion{0,max,2,false,true,false}.isValid()));
}
