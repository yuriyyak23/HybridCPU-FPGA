#include "../../CloseToHSL/Memory/Cache/SharedPodCache.h"
#include <cassert>
using HybridCPU_ISE::CloseToHSL::Memory::Cache::SharedPodCache;
int main() { SharedPodCache c; unsigned char data[]={1,2,3}; assert(!c.Lookup(0x100,2,3)); assert(c.getMissPendingMask()==4); c.Fill(0x100,data,2,3); assert(c.getMissPendingMask()==0); assert(c.Lookup(0x100,2,3)); assert(!c.Lookup(0x100,5,4)); assert(c.getMissPendingMask()==32); assert(c.Lookup(0x100,5,0)); c.Invalidate(0x100); assert(!c.Lookup(0x100,0,3)); auto [n,h,m]=c.GetDomainAccessCounts(3); assert(n==3 && h==1 && m==2); c.Reset(); assert(c.getTotalLookups()==0 && c.getMissPendingMask()==0); auto [a,b,d]=c.GetDomainAccessCounts(3); assert(a==0 && b==0 && d==0); }
