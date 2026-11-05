#include "../../Frontend/Contracts/Contracts.h"
#include <type_traits>
#include <cassert>
using namespace HybridCpu::ShadowFrontend;
static_assert(!std::is_assignable_v<decltype((AccessDecision::Permit->Allowed)), bool>);
static_assert(!std::is_assignable_v<decltype((AccessDecision::Permit->Fault)), std::optional<std::wstring>>);
int main(){ assert(AccessDecision::Permit->Allowed && !AccessDecision::Permit->Fault); }
