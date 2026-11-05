#include "../../CloseToHSL/Core/Architecture/BinaryFormat/SlotEncoding/SlotId.h"
#include <cassert>
using YAKSys_Hybrid_CPU::Core::SlotId;
int main() {
 SlotId id = SlotId::Create(7); assert(int(id) == 7 && id.ToString() == L"slot7");
 assert(!SlotId::TryCreate(-1,id) && int(id)==0); assert(!SlotId::TryCreate(8,id) && int(id)==0);
 for(int i=0;i<8;++i){assert(SlotId::TryCreate(i,id)); assert(id.ToRawValue()==i);}
 bool thrown=false; try{(void)SlotId::Create(8);}catch(const std::out_of_range&){thrown=true;} assert(thrown);
}
