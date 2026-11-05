#include "../../CloseToHSL/Memory/Timing/MemoryAdmissionTypes.h"
#include <cassert>
using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;
int main(){ auto a=MemoryAdmissionResult::Accepted({42}); assert(a.Status==MemoryAdmissionStatus::Accepted && a.RequestId.isValid() && !a.Reason); auto b=MemoryAdmissionResult::Backpressured(); assert(!b.RequestId.isValid() && !b.Reason); auto c=MemoryAdmissionResult::Rejected(L"denied"); assert(!c.RequestId.isValid() && c.Reason==L"denied"); }
