#include "../../CloseToHSL/Core/Execution/DmaStreamCompute/DmaStreamComputeProgressDiagnostics.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
int main() {
 auto first = DmaStreamComputeProgressDiagnostics::Empty->Add(1, 2, 3, 4, 5);
 assert(first->BytesRead == 1 && first->BytesStaged == 2 && first->ElementOperations == 3 && first->ModeledLatencyCycles == 4 && first->BackendStepCount == 5);
 auto max = std::numeric_limits<unsigned long long>::max();
 auto second = first->Add(max, max, max, max, max);
 assert(second->BytesRead == max && second->BackendStepCount == max && !second->isAuthoritative() && !second->getCanPublishMemory());
 assert(DmaStreamComputeProgressDiagnostics::Empty->BytesRead == 0);
}
