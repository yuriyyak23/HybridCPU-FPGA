#include "../../CloseToHSL/Core/Pipeline/MicroOps/Types/MicroOp.Exceptions.h"
#include <cassert>
#include <string>
using namespace YAKSys_Hybrid_CPU::Core;
int main(){PageFaultException p(0x123,true);assert(p.getFaultAddress()==0x123 && p.isWrite() && std::string(p.what()).find("0x123")!=std::string::npos); PageFaultException nested(L"message",std::runtime_error("inner"),4,false); assert(nested.getInnerException()); DomainFaultException d(2,0x10,3,4);assert(d.getVirtualThreadId()==2 && d.getFaultingPC()==16 && d.getOperationDomainTag()==3 && d.getActiveCert()==4);}
