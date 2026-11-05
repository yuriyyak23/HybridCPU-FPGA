#include "../../CloseToHSL/Core/Execution/DmaStreamCompute/DmaStreamComputeStagedWrite.h"
#include <cassert>
using YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute::DmaStreamComputeStagedWrite;
int main() {
 std::vector<unsigned char> bytes{1,2,3}; DmaStreamComputeStagedWrite staged(42, bytes); bytes[0]=9;
 assert(staged.Address == 42 && staged.Length == 3 && staged.Data[0] == 1);
 DmaStreamComputeStagedWrite empty(1, {}); assert(empty.Length == 0 && empty.Data.empty());
}
