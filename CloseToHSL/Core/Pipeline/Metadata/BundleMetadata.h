#pragma once
#include "SlotMetadata.h"
#include <memory>
#include <string>
#include <optional>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
 class BundleMetadata final
 {
 public:
  static constexpr int BundleSlotCount = 8;
  unsigned char SchemaVersion = 4;
  std::shared_ptr<std::vector<std::shared_ptr<SlotMetadata>>> SlotMetadata;
  bool FspBoundary = false;
  ThermalHint BundleThermalHint = static_cast<ThermalHint>(0);
  bool IsReplayAnchor = false;
  std::optional<std::wstring> DiagnosticsTag;
  static std::shared_ptr<BundleMetadata> GetDefault();
  std::shared_ptr<::YAKSys_Hybrid_CPU::Core::SlotMetadata> GetSlotMetadata(int slotIndex) const;
  std::shared_ptr<MicroOpAdmissionMetadata> GetAdmissionMetadata(int slotIndex) const;
  static std::shared_ptr<BundleMetadata> CreateAllStealable();
  static std::shared_ptr<BundleMetadata> CreateFspFence();
 };
}
