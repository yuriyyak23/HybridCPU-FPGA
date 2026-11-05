#include "../../CloseToHSL/Core/Architecture/Registers/Architectural/ArchContextState.h"
#include <cassert>
#include <stdexcept>
using namespace YAKSys_Hybrid_CPU::Core::Registers;
struct Lease : Disposable { int& releases; explicit Lease(int& n):releases(n){} void dispose() override { ++releases; } };
int main() {
 auto state=std::make_shared<ArchContextState>(4,1); auto view=state->getCommittedRegs();
 view->setIndexer(1,42); assert((*view)[1]==42 && view->Clone()[1]==42);
 auto lease=state->ReserveStandaloneRetirement(); auto core=std::make_shared<int>(1); int reads=0,writes=0,releases=0;
 auto binding=std::make_shared<CoreContextBinding>(core,[&](CanonicalStateAccessKind){++reads;return std::make_shared<Lease>(releases);},[&](ArchContextWriteKind k,int i,unsigned long long v,std::function<void()> action){assert(k==ArchContextWriteKind::Register && i==1 && v==55);++writes;action();});
 bool denied=false; try {state->AttachCore(binding);} catch(const std::logic_error&) {denied=true;} assert(denied);
 lease->dispose(); state->AttachCore(binding); assert(state->BelongsTo(core));
 view->setIndexer(1,55); assert(writes==1 && (*view)[1]==55 && reads==1 && releases==1);
 state.reset(); assert((*view)[1]==55 && reads==2 && releases==2);
 bool bounds=false; try {view->setIndexer(4,0);} catch(const std::out_of_range&) {bounds=true;} assert(bounds);
}
