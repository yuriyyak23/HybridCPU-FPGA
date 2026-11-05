#include "../../Frontend/Contracts/SemanticRecords.h"
#include <cassert>
using namespace HybridCpu::ShadowFrontend;
int main(){ unsigned char x[]={3}; auto a=std::make_shared<Snapshot>(L"raw",std::make_shared<FrozenBytes>(x)); auto b=std::make_shared<Snapshot>(L"raw",std::make_shared<FrozenBytes>(x)); SemanticSlot s{0,true,1,{}, {},1,2,3,4,{},false,false,a,{},{},{},{}}; auto t=s; t.RawSlot=b; assert(s==t); t.Immediate++; assert(!(s==t)); t=s; t.RawSlot={}; assert(!(s==t)); }
