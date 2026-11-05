#include "../../CloseToHSL/Core/Pipeline/Scheduling/LaneId.h"
#include <cassert>
using YAKSys_Hybrid_CPU::Core::LaneId;
int main(){static_assert(sizeof(LaneId)==1); LaneId lane; for(int i=-1;i<=8;++i){bool ok=LaneId::TryCreate(i,lane); assert(ok==(i>=0 && i<8)); assert(lane.ToRawValue()==(ok?i:0));} assert(LaneId::Zero.ToRawValue()==0 && LaneId::Create(7).ToString()==L"lane7");try{LaneId::Create(8);assert(false);}catch(const std::out_of_range&){} }
