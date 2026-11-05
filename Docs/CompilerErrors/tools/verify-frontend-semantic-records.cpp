#include "../../Frontend/Contracts/SemanticRecords.h"
#include <cassert>
using namespace HybridCpu::ShadowFrontend;
int main(){ unsigned char bytes[]={1,2}; auto a=std::make_shared<FrozenBytes>(bytes); auto b=std::make_shared<FrozenBytes>(bytes); Snapshot x{L"raw",a}, y{L"raw",b}; assert(x==y); Snapshot z{L"other",b}; assert(!(x==z)); TypedDecodeFailure e{1,2,L"field",L"hash",L"message"}; assert(e==e); }
