#include "../../CloseToHSL/Core/Memory/DataCacheRangeResults.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core::Memory;
int main(){
 auto a=DataCacheRangeFlushResult::EmptyNoOpProof;
 DataCacheRangeFlushResult b{true,false,true,2,3,4}; auto c=a.Merge(b);
 assert(c.RangeWellFormed && !c.IsNoOpProof && c.RequiresFutureDirtyWriteback && !c.getSucceeded());
 assert(c.getDirtyLinesObserved()==7 && c.CleanLinesObserved==2);
 DataCacheRangeInvalidationResult d{true,2,3,1}; assert(d.getSucceeded() && d.getTotalLinesInvalidated()==5);
 assert(MemoryCoherencyObserverResult::Empty.getSucceeded());
}
