#include "../../CloseToHSL/Core/Decoder/OperationAttemptIssuer.h"
#include <cassert>
using YAKSys_Hybrid_CPU::Core::Decoder::OperationAttemptIssuer;
int main(){OperationAttemptIssuer a,b; assert(a.IssueNextAttempt()==1 && a.IssueNextAttempt()==2 && b.IssueNextAttempt()==1);}
