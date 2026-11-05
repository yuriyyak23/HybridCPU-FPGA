#include "../../CloseToHSL/Core/Decoder/HazardEffectKind.h"
using YAKSys_Hybrid_CPU::Core::HazardEffectKind;
static_assert(static_cast<unsigned>(HazardEffectKind::RegisterData) == 0);
static_assert(static_cast<unsigned>(HazardEffectKind::PinnedLane) == 4);
