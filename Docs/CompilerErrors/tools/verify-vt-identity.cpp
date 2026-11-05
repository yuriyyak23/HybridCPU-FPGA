#include "../../CloseToHSL/Core/Architecture/Registers/Architectural/RegisterIdentity.h"
#include <cassert>
using YAKSys_Hybrid_CPU::Core::Registers::VtId;
int main() {
  VtId id = VtId::Create(3);
  assert(id.getValue() == 3 && id.ToRawValue() == 3 && id.ToString() == L"vt3");
  assert(!VtId::TryCreate(-1, id) && id.getValue() == 0);
  assert(!VtId::TryCreate(4, id) && id.getValue() == 0);
  for (int i = 0; i < 4; ++i) { assert(VtId::TryCreate(i, id)); assert(int(id) == i); }
  bool thrown = false; try { (void)VtId::Create(4); } catch (const std::out_of_range&) { thrown = true; } assert(thrown);
}
