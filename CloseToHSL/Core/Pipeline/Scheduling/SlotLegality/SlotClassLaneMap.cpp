#include "SlotClassDefinitions.h"
#include "tangible_exception_helper.h"
#include <array>
#include <bit>

namespace YAKSys_Hybrid_CPU::Core {
unsigned char SlotClassLaneMap::GetLaneMask(SlotClass slotClass) {
    switch (slotClass) {
    case SlotClass::AluClass: return AluMask;
    case SlotClass::LsuClass: return LsuMask;
    case SlotClass::DmaStreamClass: return DmaStreamMask;
    case SlotClass::MatrixTileStreamClass: return MatrixTileStreamMask;
    case SlotClass::BranchControl: return BranchMask;
    case SlotClass::SystemSingleton: return SystemMask;
    case SlotClass::Unclassified: return UnclassifiedMask;
    default: throw ArgumentOutOfRangeException("Unknown SlotClass value.");
    }
}
int SlotClassLaneMap::GetClassCapacity(SlotClass slotClass) {
    return std::popcount(static_cast<unsigned int>(GetLaneMask(slotClass)));
}
std::span<const SlotClass> SlotClassLaneMap::GetAliasedClasses(SlotClass slotClass) {
    static constexpr std::array branch{SlotClass::SystemSingleton};
    static constexpr std::array system{SlotClass::BranchControl};
    static constexpr std::array dma{SlotClass::MatrixTileStreamClass};
    static constexpr std::array matrix{SlotClass::DmaStreamClass};
    switch (slotClass) {
    case SlotClass::BranchControl: return branch;
    case SlotClass::SystemSingleton: return system;
    case SlotClass::DmaStreamClass: return dma;
    case SlotClass::MatrixTileStreamClass: return matrix;
    default: return {};
    }
}
bool SlotClassLaneMap::HasAliasedLanes(SlotClass slotClass) {
    return slotClass == SlotClass::BranchControl || slotClass == SlotClass::SystemSingleton ||
           slotClass == SlotClass::DmaStreamClass || slotClass == SlotClass::MatrixTileStreamClass;
}
}