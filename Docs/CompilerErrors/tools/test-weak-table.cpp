#include "../../CloseToHSL/Core/Pipeline/Safety/ConditionalWeakTable.h"
#include <cassert>
struct K {}; struct V {};
int main(){ ConditionalWeakTable<std::shared_ptr<K>,std::shared_ptr<V>> t; auto k=std::make_shared<K>(); auto v=std::make_shared<V>(); std::weak_ptr<V> w=v; t.Add(k,v); v.reset(); std::shared_ptr<V> out; assert(t.TryGetValue(k,out)); out.reset(); k.reset(); auto k2=std::make_shared<K>(); assert(!t.TryGetValue(k2,out)); assert(w.expired()); }