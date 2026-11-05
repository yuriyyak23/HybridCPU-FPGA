#pragma once
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeTokenStore.h"
namespace YAKSys_Hybrid_CPU::Core {
struct Lane6VirtualToken final {
 unsigned short IoDomainTag=0, OwnerVirtualThreadId=0;
 unsigned int DomainId=0; unsigned long long DomainTag=0; unsigned int DeviceId=0;
 unsigned long long GuestTokenId=0, VirtualTokenId=0, GuestFenceId=0, QueueEpoch=0, FenceEpoch=0;
 bool IsValid() const noexcept { return IoDomainTag && DomainId && DomainTag && DeviceId && GuestTokenId && VirtualTokenId; }
 bool ExposesHostTokenHandle(const Execution::DmaStreamCompute::DmaStreamComputeTokenHandle& h) const noexcept { return !h.IsDefault() && (VirtualTokenId == h.TokenId || GuestTokenId == h.TokenId); }
 bool operator==(const Lane6VirtualToken&) const = default;
};
struct Lane6VirtualTokenHash { size_t operator()(const Lane6VirtualToken& v) const noexcept { size_t h=0; auto mix=[&](unsigned long long x){ h ^= std::hash<unsigned long long>{}(x)+static_cast<size_t>(0x9e3779b9U)+(h<<6)+(h>>2); }; mix(v.IoDomainTag); mix(v.OwnerVirtualThreadId); mix(v.DomainId); mix(v.DomainTag); mix(v.DeviceId); mix(v.GuestTokenId); mix(v.VirtualTokenId); mix(v.GuestFenceId); mix(v.QueueEpoch); mix(v.FenceEpoch); return h; } };
}
