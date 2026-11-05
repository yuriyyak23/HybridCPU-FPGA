#include "../../CloseToHSL/Core/Execution/ExternalAccelerators/Capabilities/AcceleratorOperationCapability.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
int main() { auto shape=std::make_shared<AcceleratorShapeCapability>(L"matrix",1,8,1,2); AcceleratorOperationCapability op(L"matmul",{L"F32",L"f32",L"  ",L"I8"},{shape}); assert(op.getSupportedDatatypes().size()==2); assert(op.SupportsDatatype(L"f32") && !op.SupportsDatatype(L"f16")); assert(op.SupportsShape(L"MATRIX",8,2) && !op.SupportsShape(L"matrix",9,2)); bool failed=false; try{AcceleratorOperationCapability bad(L" ",{});}catch(const std::invalid_argument&){failed=true;} assert(failed); }
