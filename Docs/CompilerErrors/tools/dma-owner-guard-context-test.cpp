#include "../../CloseToHSL/Core/Execution/DmaStreamCompute/DmaStreamComputeOwnerGuardContext.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
int main(){ DmaStreamComputeOwnerGuardContext a(1,2,3,4,5,6); assert(a.OwnerVirtualThreadId==1 && a.OwnerContextId==2 && a.OwnerCoreId==3 && a.OwnerPodId==4 && a.OwnerDomainTag==5 && a.ActiveDomainCertificate==6 && a.DeviceId==6); auto b=a;assert(a==b);b.DeviceId=7;assert(!(a==b)); DmaStreamComputeOwnerGuardContext z;assert(z.DeviceId==0); }
