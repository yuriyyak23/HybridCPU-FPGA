#pragma once
#include "../../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute {
struct DmaStreamComputeReplayEvidenceComparison final {
    bool CanReuse;
    ReplayPhaseInvalidationReason InvalidationReason;
    std::wstring MismatchField;
    static const std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> ReuseHit;
    static std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> Miss(ReplayPhaseInvalidationReason reason, const std::wstring& field) {
        return std::make_shared<DmaStreamComputeReplayEvidenceComparison>(false, reason, field);
    }
};
inline const std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> DmaStreamComputeReplayEvidenceComparison::ReuseHit = std::make_shared<DmaStreamComputeReplayEvidenceComparison>(true, ReplayPhaseInvalidationReason::None, L"");
}
