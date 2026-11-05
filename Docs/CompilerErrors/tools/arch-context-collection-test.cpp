#include <cassert>
#include "../../CloseToHSL/Core/Architecture/Registers/Architectural/ArchContextCollection.h"
namespace YAKSys_Hybrid_CPU::Core::Registers { class ArchContextState {}; }
using namespace YAKSys_Hybrid_CPU::Core::Registers;
int main() {
 auto a=std::make_shared<ArchContextState>(), b=std::make_shared<ArchContextState>();
 std::vector<std::shared_ptr<ArchContextState>> source{a};
 ArchContextCollection unowned(source); source[0]=b;
 assert(unowned[0]==a && unowned.Clone()[0]==a);
 bool denied=false; try { unowned.setIndexer(0,b); } catch(const std::logic_error&) { denied=true; }
 assert(denied && unowned[0]==a);
 bool called=false; ArchContextCollection owned(source,[&](int i, auto value){called=i==0 && value==a;});
 owned.setIndexer(0,a); assert(called && owned[0]==b);
 owned.StoreFromCore(0,a); assert(owned[0]==a);
}
