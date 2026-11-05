#include "CloseToHSL/Core/Virtualization/Compatibility/Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main() {
  CompatAliasMap m; auto s=CompatAliasMap::getCanonicalSchema();
  assert(CompatAliasMap::getEntries().size()==11 && m.MatchesCanonicalSchema(s));
  assert(m.ContainsSource(CompatAliasSourceKind::Opcode,L"VMXON"));
  assert(!m.ContainsSource(CompatAliasSourceKind::Opcode,L"vmxon"));
  assert(m.IsFrozenAbiSource(CompatAliasSourceKind::Csr,L"VmxCaps"));
  std::shared_ptr<CompatAliasMapEntry> e; assert(!m.TryGetEntry(CompatAliasSourceKind::Csr,L"unknown",e) && !e);
}
