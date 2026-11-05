#pragma once
namespace YAKSys_Hybrid_CPU::Core {
struct Lane7QuotaPolicy final {
 unsigned short MaxVirtualHandles=0, MaxInflightTokens=0, MaxSubmitPollPerWindow=0;
 static Lane7QuotaPolicy Default() { return {64,64,8}; }
 Lane7QuotaPolicy Normalize() const { return {static_cast<unsigned short>(MaxVirtualHandles ? MaxVirtualHandles : 1),static_cast<unsigned short>(MaxInflightTokens ? MaxInflightTokens : 1),static_cast<unsigned short>(MaxSubmitPollPerWindow ? MaxSubmitPollPerWindow : 1)}; }
};
struct Lane7BackendBinding final {
 unsigned short ExecutionDomainTag=0, OwnerVirtualThreadId=0; unsigned long long VirtualHandle=0, BindingEpoch=0, BackendGeneration=0; bool Available=false, RequiresRebind=false;
 bool IsUsable() const { return ExecutionDomainTag && VirtualHandle && BindingEpoch && Available && !RequiresRebind; }
};
struct Lane7PressureSnapshot final {
 unsigned short ExecutionDomainTag=0, AddressSpaceTag=0, OwnerVirtualThreadId=0; int InflightTokens=0, MaxInflightTokens=0, SubmitPollCount=0, MaxSubmitPollPerWindow=0; bool QuotaExceeded=false, SchedulerPreemptionRecommended=false; unsigned long long PressureEpoch=0;
};
}
