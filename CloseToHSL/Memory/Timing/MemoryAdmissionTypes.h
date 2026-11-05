#pragma once
#include "MemoryRequestId.h"
#include <optional>
#include <string>
#include <utility>
namespace HybridCPU_ISE::CloseToHSL::Memory::Timing {
enum class MemoryAdmissionStatus : unsigned char { Accepted, Backpressured, Rejected };
struct MemoryAdmissionResult final {
    MemoryAdmissionStatus Status = MemoryAdmissionStatus::Accepted;
    MemoryRequestId RequestId{};
    std::optional<std::wstring> Reason;
    static MemoryAdmissionResult Accepted(MemoryRequestId id) { return {MemoryAdmissionStatus::Accepted, id, std::nullopt}; }
    static MemoryAdmissionResult Backpressured() { return {MemoryAdmissionStatus::Backpressured, {}, std::nullopt}; }
    static MemoryAdmissionResult Rejected(std::wstring reason) { return {MemoryAdmissionStatus::Rejected, {}, std::move(reason)}; }
};
}
