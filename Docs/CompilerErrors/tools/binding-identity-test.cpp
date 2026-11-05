#include "../../CloseToHSL/Core/Decoder/ExecutionBindingIdentity.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
int main(){MaterializerId a(L"  materializer \t"); assert(a.Value==L"materializer" && a==MaterializerId(L"materializer")); RuntimeExecutionProviderBinding b(RuntimeExecutionProviderId(L" p "),L" schema "); assert(b.Id.Value==L"p" && b.PayloadSchema==L"schema"); assert(LatencyModelId(L" latency ").ToString()==L"latency"); try{MaterializerId a(L" \t");assert(false);}catch(const std::invalid_argument&){} }
