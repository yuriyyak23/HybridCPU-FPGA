#include <cassert>
#include "../../CloseToHSL/Core/Architecture/Registers/Architectural/RegisterIdentity.h"
using YAKSys_Hybrid_CPU::Core::Registers::PhysRegId;
int main() {
 PhysRegId id;
 assert(PhysRegId::TryCreate(127,id) && id.getValue()==127);
 assert(!PhysRegId::TryCreate(128,id) && id == PhysRegId::Zero);
 assert(!PhysRegId::TryCreate(-1,id));
 bool rejected=false; try { (void)PhysRegId::Create(128); } catch(const std::out_of_range&) { rejected=true; }
 assert(rejected && PhysRegId::Create(7).ToString()==L"p7");
}
