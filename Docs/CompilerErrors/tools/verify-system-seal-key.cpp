#include "../../CloseToHSL/Core/Runtime/Completion/Commit/SystemSealKey.h"
#include <cassert>
int main(){ const auto first=YAKSys_Hybrid_CPU::Core::GenerateSystemSealKey(); const auto second=YAKSys_Hybrid_CPU::Core::GenerateSystemSealKey(); assert(first.size()==32 && second.size()==32); assert(first != second); }
