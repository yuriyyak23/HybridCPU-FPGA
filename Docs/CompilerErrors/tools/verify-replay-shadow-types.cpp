#include "../../CloseToHSL/Core/Decoder/ReplayShadowTypes.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
int main(){ ReplayEntryValidationFingerprint a{1,std::wstring(64,L'a')}, b{1,std::wstring(64,L'a')}; assert(a.isValid() && a==b); b.SchemaVersion=2; assert(!b.isValid()); ReplaySemanticShadowMetrics m{7,2,3,2,1}; assert(m.getAccountedObservations()==7); }
