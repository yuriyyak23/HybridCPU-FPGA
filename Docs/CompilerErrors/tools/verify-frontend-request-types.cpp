#include "../../Frontend/Contracts/RequestTypes.h"
#include <cassert>
using namespace HybridCpu::ShadowFrontend;
int main(){ BundleRequest r{std::numeric_limits<std::uint64_t>::max()-255,3,7,8}; r.Validate(); r.Pc++; bool failed=false; try {r.Validate();} catch(const std::out_of_range&){failed=true;} assert(failed); r.Pc=0; r.VirtualThread=-1; failed=false; try {r.Validate();} catch(const std::out_of_range&){failed=true;} assert(failed); assert((FreshnessToken{1,2} == FreshnessToken{1,2})); }
