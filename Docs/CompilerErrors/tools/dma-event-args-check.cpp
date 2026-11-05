#include "../../tangible_event.h"
#include <type_traits>
struct TransferArgs : EventArgs {};
static_assert(std::has_virtual_destructor_v<EventArgs>);
static_assert(std::is_base_of_v<EventArgs, TransferArgs>);
