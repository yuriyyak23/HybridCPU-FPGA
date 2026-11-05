#include "../../CloseToHSL/Memory/Subsystem/CanonicalVectorPhysicalBankEnvelope.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Memory;
int main() {
 MemoryBankGeometryGeneration generation; assert(!generation.isIssued()); assert(!MemoryBankGeometryGeneration::TryCreate(0,generation)); assert(MemoryBankGeometryGeneration::TryCreate(3,generation));
 PhysicalMemoryBankIndex bank; assert(!PhysicalMemoryBankIndex::TryCreate(-1,bank)); assert(PhysicalMemoryBankIndex::TryCreate(2,bank));
 std::vector<PhysicalMemoryBankIndex> input{bank, PhysicalMemoryBankIndex(4)}; CanonicalVectorPhysicalBankEnvelope env;
 assert(!CanonicalVectorPhysicalBankEnvelope::TryCreate({},input,env)); assert(!env.isWellFormed()); assert(CanonicalVectorPhysicalBankEnvelope::TryCreate(generation,input,env));
 input[0]=PhysicalMemoryBankIndex(7); auto copy=env.CopySourceBankIndexes(); copy[1]=PhysicalMemoryBankIndex(8); assert(env.GetSourceBankIndex(0).getValue()==2 && env.GetSourceBankIndex(1).getValue()==4);
 bool caught=false; try { env.GetSourceBankIndex(-1); } catch(const std::out_of_range&) { caught=true; } assert(caught);
}
