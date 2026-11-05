#include "../../CloseToHSL/Core/Pipeline/Scheduling/SlotLegality/SlotClassCapacity.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main() {
 SlotClassCapacityState s; s.InitializeFromLaneMap();
 assert(s.GetFreeCapacity(SlotClass::AluClass)==4 && s.GetFreeCapacity(SlotClass::LsuClass)==2);
 assert(s.HasFreeCapacity(SlotClass::BranchControl) && s.HasFreeCapacity(SlotClass::SystemSingleton));
 s.IncrementOccupancy(SlotClass::SystemSingleton); assert(!s.HasFreeCapacity(SlotClass::BranchControl));
 s.IncrementOccupancy(SlotClass::DmaStreamClass); assert(!s.HasFreeCapacity(SlotClass::MatrixTileStreamClass));
 s.IncrementOccupancy(SlotClass::AluClass); assert(s.GetOccupied(SlotClass::AluClass)==1 && s.GetFreeCapacity(SlotClass::AluClass)==3);
 s.ResetOccupancy(); assert(s.GetOccupied(SlotClass::AluClass)==0 && s.HasFreeCapacity(SlotClass::BranchControl) && s.HasFreeCapacity(SlotClass::DmaStreamClass));
}
