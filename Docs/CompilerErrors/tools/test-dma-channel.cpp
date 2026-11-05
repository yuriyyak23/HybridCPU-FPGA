#include "../../CloseToHSL/Memory/DMA/DmaChannelId.h"
#include <cassert>
int main(){ using HybridCPU_ISE::CloseToHSL::Memory::DMA::DmaChannelId; DmaChannelId id; assert(DmaChannelId::TryCreate(7,id) && id.ToRawValue()==7); assert(!DmaChannelId::TryCreate(8,id) && id.ToRawValue()==0); try { (void) DmaChannelId::Create(8); return 1; } catch(const std::out_of_range&) {} }