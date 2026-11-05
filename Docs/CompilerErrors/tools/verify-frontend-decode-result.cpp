#include "../../Frontend/Contracts/Contracts.h"
#include <cassert>
using namespace HybridCpu::ShadowFrontend;
int main(){
 std::vector<unsigned char> raw(256); for(int i=0;i<256;++i) raw[i]=static_cast<unsigned char>(i);
 auto bytes=std::make_shared<FrozenBytes>(raw); std::vector<std::shared_ptr<SemanticSlot>> slots;
 for(int i=0;i<8;++i){ auto snap=std::make_shared<Snapshot>(L"raw",std::make_shared<FrozenBytes>(std::span<const unsigned char>(raw).subspan(i*32,32))); slots.push_back(std::make_shared<SemanticSlot>(SemanticSlot{i,true,1,{},{},0,0,0,0,{},false,false,snap,{},{},{},{}})); }
 auto first=DecodeResult::Success(bytes,slots,{}); auto second=DecodeResult::Success(bytes,slots,{}); assert(first->SameFacts(second));
 slots[0]->Opcode=4; assert(first->SameFacts(second)); auto returned=first->getSlots(); returned[0]->Opcode=5; assert(first->SameFacts(second));
 auto failure=std::make_shared<TypedDecodeFailure>(TypedDecodeFailure{1,-1,L"f",L"h",L"m"}); auto bad=DecodeResult::Illegal(bytes,failure); failure->Code=9; assert((*bad->getFailure())->Code==1);
 auto bad2=DecodeResult::Illegal(bytes,std::make_shared<TypedDecodeFailure>(TypedDecodeFailure{1,-1,L"f",L"h",L"m"})); assert(bad->SameFacts(bad2));
 raw[0]=9; auto wrong=std::make_shared<FrozenBytes>(raw); bool threw=false; try{DecodeResult::Success(wrong,slots,{});}catch(const std::logic_error&){threw=true;} assert(threw);
}
