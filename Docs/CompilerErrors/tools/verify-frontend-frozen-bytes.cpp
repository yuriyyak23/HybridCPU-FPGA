#include "../../Frontend/Contracts/FrozenBytes.h"
#include <cassert>
using HybridCpu::ShadowFrontend::FrozenBytes;
int main(){ std::vector<unsigned char> source{1,2,3}; auto a=std::make_shared<FrozenBytes>(source); source[0]=9; assert(a->Copy()[0]==1); auto b=std::make_shared<FrozenBytes>(a->getSpan()); assert(a->Equals(b)); auto copy=a->Copy(); copy[0]=8; assert(a->getSpan()[0]==1); }
