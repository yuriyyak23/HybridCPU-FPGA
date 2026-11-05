#include "../../CloseToHSL/Memory/Subsystem/MemoryRequestTypes.h"
#include <type_traits>
using namespace YAKSys_Hybrid_CPU::Memory;
static_assert(std::is_base_of_v<std::enable_shared_from_this<BurstEventArgs>, BurstEventArgs>);
static_assert(std::is_constructible_v<MemoryRequestToken, unsigned long long, unsigned long long, unsigned long long, int, std::vector<unsigned char>&, bool, long long, bool, std::shared_ptr<PhysicalMemoryBankBinding>>);
