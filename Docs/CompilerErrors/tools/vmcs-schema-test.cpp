#include "CloseToHSL/Core/Virtualization/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main(){ auto entries=VmcsFieldProjectionSchema::getEntries(); auto artifact=VmcsFieldProjectionSchema::getCanonicalArtifact(); assert(entries.size()==23 && artifact->EntryCount==23 && artifact->IsCanonical); for(auto e:entries){std::shared_ptr<VmcsFieldProjectionSchemaEntry> found; assert(VmcsFieldProjectionSchema::TryGet(e->Field,found) && found==e); assert(VmcsFieldProjectionSchema::CanRead(e) && !VmcsFieldProjectionSchema::CanWrite(e));} std::shared_ptr<VmcsFieldProjectionSchemaEntry> missing; assert(!VmcsFieldProjectionSchema::TryGet(static_cast<VmcsField>(6),missing)); }
