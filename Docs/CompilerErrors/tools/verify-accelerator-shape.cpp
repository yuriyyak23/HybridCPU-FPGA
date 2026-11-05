#include "../../CloseToHSL/Core/Execution/ExternalAccelerators/Capabilities/AcceleratorShapeCapability.h"
#include <cassert>
using YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities::AcceleratorShapeCapability;
int main(){ AcceleratorShapeCapability c(L"Matrix",2,10,1,3); assert(c.SupportsShape(L"mATRIX",2,1)); assert(c.SupportsShape(L"matrix",10,3)); assert(!c.SupportsShape(L"matrix",11,2)); assert(!c.SupportsShape(L"other",2,1)); bool failed=false; try{AcceleratorShapeCapability bad(L"x",5,4);}catch(const std::out_of_range&){failed=true;} assert(failed); }
