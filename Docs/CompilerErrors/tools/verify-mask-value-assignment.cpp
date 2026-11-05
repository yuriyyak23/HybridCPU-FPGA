#include "../../CloseToHSL/Core/Pipeline/Certificates/ArchLegalityMask.h"
#include <type_traits>
using namespace YAKSys_Hybrid_CPU::Core;
static_assert(std::is_copy_assignable_v<ArchLegalityMask>);
static_assert(std::is_copy_assignable_v<BackendHazardMask>);
void verify_assignment(ArchLegalityMask& a, BackendHazardMask& b, const ArchLegalityMask& x, const BackendHazardMask& y) { a = x; b = y; }
