#include "../../CloseToHSL/Core/Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../tangible_exception_helper.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main() {
 using C=SlotClass;
 const C classes[]={C::AluClass,C::LsuClass,C::DmaStreamClass,C::BranchControl,C::SystemSingleton,C::MatrixTileStreamClass,C::Unclassified};
 const unsigned masks[]={15,48,64,128,128,64,255};
 const int caps[]={4,2,1,1,1,1,8};
 for(int i=0;i<7;++i) { assert(SlotClassLaneMap::GetLaneMask(classes[i])==masks[i]); assert(SlotClassLaneMap::GetClassCapacity(classes[i])==caps[i]); auto aliases=SlotClassLaneMap::GetAliasedClasses(classes[i]); assert(SlotClassLaneMap::HasAliasedLanes(classes[i])==!aliases.empty()); if(!aliases.empty()) assert(SlotClassLaneMap::GetLaneMask(aliases[0])==masks[i]); }
 auto saved=SlotClassLaneMap::GetAliasedClasses(C::BranchControl); SlotClassLaneMap::GetAliasedClasses(C::DmaStreamClass); assert(saved.size()==1 && saved[0]==C::SystemSingleton);
 try { SlotClassLaneMap::GetLaneMask(static_cast<C>(6)); assert(false); } catch(const ArgumentOutOfRangeException&) {}
}
