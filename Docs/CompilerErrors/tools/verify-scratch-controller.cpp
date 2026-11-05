#include "../../CloseToHSL/Core/Execution/Memory/Scratch/ScratchBankController.h"
#include <cassert>
#include <array>
using YAKSys_Hybrid_CPU::Core::ScratchBankController;
int main(){ ScratchBankController c; c.Initialize(); std::array<unsigned char,8> source{1,2,3,4,5,6,7,8}, dest{}; c.LoadFromMemory(source,2,4); c.StoreToMemory(dest,2,4); assert(source==dest && c.getTotalAccesses()==8); assert(c.CheckConflict(0,1,2)==0); assert(c.CheckConflict(0,4,2)==1 && c.getBankConflicts()==1); }
