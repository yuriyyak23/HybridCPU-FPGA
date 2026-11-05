#include "../../Arch/DataType.h"
#include "../../tangible_exception_helper.h"
#include <cassert>
using HybridCPU_ISE::Arch::DataTypeEnum;
using YAKSys_Hybrid_CPU::Arch::DataTypeUtils;
int main() {
 const int sizes[]={1,1,2,2,4,4,4,8,8,8,2,2,1,1};
 for (unsigned int n=0;n<256;++n) {
   const auto t=static_cast<DataTypeEnum>(n); assert(DataTypeUtils::IsValid(t)==(n<14));
   if(n<14) { assert(DataTypeUtils::SizeOf(t)==sizes[n]); assert(DataTypeUtils::BitWidth(t)==sizes[n]*8); assert(DataTypeUtils::IsInteger(t)==(DataTypeUtils::IsSignedInteger(t)||DataTypeUtils::IsUnsignedInteger(t))); assert(DataTypeUtils::IsInteger(t)!=DataTypeUtils::IsFloatingPoint(t)); }
   else { try { DataTypeUtils::SizeOf(t); assert(false); } catch(const ArgumentOutOfRangeException&) {} }
 }
}
