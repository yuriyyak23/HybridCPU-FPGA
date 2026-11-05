#pragma once
#include "../../../../Frontend/Telemetry/FrontendTelemetry.h"
#include <memory>

namespace HybridCpu::ShadowFrontend {}
using namespace HybridCpu::ShadowFrontend;

namespace YAKSys_Hybrid_CPU
{
    enum class ShadowFrontendMode { Off, Observe, ColdWarm };
    struct ShadowFrontendCpuSnapshot final {
        ShadowFrontendMode Mode;
        unsigned long long Comparisons, Mismatches, ServedCold, ServedWarm, UnsupportedSources, RuntimeCanonicalDecodes, RuntimeRestores;
        std::shared_ptr<TelemetrySnapshot> Cache;
    };
}
