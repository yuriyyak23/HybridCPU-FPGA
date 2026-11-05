#pragma once

#include "Certificates/SafetyMask128.h"
#include "../Runtime/Completion/Commit/NeutralArchitecturalCompletionFacts.h"
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{
    // Event data mirrors the PipelineEvent records in the C# source.
    struct PipelineEvent {
        virtual ~PipelineEvent() = default;
        unsigned char VtId = 0;
        unsigned long long BundleSerial = 0;
    };
    struct EcallEvent final : PipelineEvent { long long EcallCode = 0; };
    struct EbreakEvent final : PipelineEvent {};
    struct TrapEntryEvent final : PipelineEvent {
        unsigned long long CauseCode = 0;
        unsigned long long FaultAddress = 0;
        bool HasFaultAddress = false;
        NeutralFaultAddressSemantic FaultAddressSemantic = NeutralFaultAddressSemantic::None;
    };
    struct MretEvent final : PipelineEvent {};
    struct SretEvent final : PipelineEvent {};
    struct FenceEvent final : PipelineEvent { bool IsInstructionFence = false; };
    struct MemoryOrderingEvent final : PipelineEvent { bool IsAcquire = false; bool IsRelease = false; };
    struct WfiEvent final : PipelineEvent {};
    struct YieldEvent final : PipelineEvent {};
    struct WfeEvent final : PipelineEvent {};
    struct SevEvent final : PipelineEvent {};
    struct PodBarrierEvent final : PipelineEvent {};
    struct VtBarrierEvent final : PipelineEvent {};
    struct ResetEvent final : PipelineEvent {};
    struct HaltEvent final : PipelineEvent { std::wstring Reason; };
    struct ClusterSyncEnterEvent final : PipelineEvent { unsigned short AffinityMask = 0; };
    struct ClusterSyncExitEvent final : PipelineEvent {};
    struct PtwWalkStartEvent final : PipelineEvent { unsigned long long FaultAddress = 0; };
    struct PtwWalkCompleteEvent final : PipelineEvent { unsigned long long PhysicalAddress = 0; };
    struct PtwWalkFaultEvent final : PipelineEvent { unsigned long long FaultAddress = 0; bool IsWrite = false; };
    struct ClockGatedDonorEnterEvent final : PipelineEvent {};
    struct ClockGatedDonorExitEvent final : PipelineEvent {};
    struct BackpressureEvent final : PipelineEvent {
        SafetyMask128 OverloadedResources{};
        int SuppressedSlotCount = 0;
    };
}