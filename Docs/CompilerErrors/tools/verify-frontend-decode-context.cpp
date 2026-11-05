#include "../../Frontend/Contracts/DecodeContext.h"
#include <cassert>
using HybridCpu::ShadowFrontend::DecodeContext;
int main(){ DecodeContext c{L"1",L"m",L"e",L"d",L"v",L"p",L"domain",L"as",L"vec",0,0,true,L"inc",L"sb"}; assert(c.isBound()); c.Domain=L"unbound"; assert(!c.isBound()); c.Domain=L"ok"; c.Privilege=L"  "; assert(!c.isBound()); c.Privilege=L"p"; c.ReplayEligible=false; assert(!c.isBound()); }
