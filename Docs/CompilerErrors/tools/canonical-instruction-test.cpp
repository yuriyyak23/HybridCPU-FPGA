#include "../../CloseToHSL/Core/Decoder/CanonicalDecodedInstruction.h"
#include <type_traits>
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
static_assert(!std::is_copy_assignable_v<CanonicalDecodedInstruction>);
int main() {
    CanonicalDecodedInstruction slot(2, true, 42, {}, {}, 3, 4, 5, -7, {}, false, true, nullptr, nullptr, nullptr);
    return slot.SlotIndex == 2 && slot.Opcode == 42 && slot.Rs1 == 4 && slot.Immediate == -7 && slot.ReleaseOrdering && !slot.StaticBinding ? 0 : 1;
}
