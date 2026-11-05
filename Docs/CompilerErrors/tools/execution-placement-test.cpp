#include "../../CloseToHSL/Core/Decoder/ExecutionPlacement.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core; using namespace YAKSys_Hybrid_CPU::Core::Decoder;
int main(){auto f=ExecutionPlacement::Create(SlotClass::AluClass,SlotPinningKind::ClassFlexible,7,42); LaneId lane; assert(f->PinnedLaneId==0 && f->DomainTag==42 && !f->TryGetHardPinnedLaneId(lane)); auto p=ExecutionPlacement::CreateHardPinned(SlotClass::LsuClass,LaneId::Create(7),13); assert(p->TryGetHardPinnedLaneId(lane) && lane.ToRawValue()==7 && p->DomainTag==13); try{ExecutionPlacement::Create(SlotClass::AluClass,SlotPinningKind::HardPinned,8);assert(false);}catch(const std::out_of_range&){} }
