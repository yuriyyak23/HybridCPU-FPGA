#include "../../CloseToHSL/Core/Pipeline/PipelineEvent.h"
#include <type_traits>
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
static_assert(std::is_base_of_v<PipelineEvent, TrapEntryEvent>);
static_assert(std::is_base_of_v<PipelineEvent, BackpressureEvent>);
void check(TrapEntryEvent& event) {
    event.VtId = 2;
    event.BundleSerial = 10;
    event.CauseCode = 2;
    event.FaultAddress = 0;
    event.HasFaultAddress = true;
    event.FaultAddressSemantic = YAKSys_Hybrid_CPU::Core::NeutralFaultAddressSemantic::VirtualAddress;
}
