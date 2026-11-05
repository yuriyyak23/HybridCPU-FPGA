#include "HotCompatibility.h"
namespace HybridCpu::ShadowFrontend {
std::optional<HotGuard> HotCompatibility::Reject(std::shared_ptr<HotCandidate> candidate, std::shared_ptr<LookupTag> current, unsigned long long replayGeneration, std::shared_ptr<HotInputs> inputs, std::function<void(HotGuard)> observe) {
 const auto& tag = *candidate->Tag;
 const auto& binding = *candidate->Binding;
 const auto& now = *inputs->Current;
 if (!current->Context->isBound() || tag.Freshness != current->Freshness || tag.Pc != current->Pc) return HotGuard::Freshness;
 if (!candidate->Identity->getResult()->isSuccess() || *candidate->Identity->getContext() != *tag.Context || !inputs->ExpectedIdentity || !candidate->Identity->Equals(inputs->ExpectedIdentity)) return HotGuard::Integrity;
 if (!Bound(binding.Structural) || !Bound(now.Structural) || binding.Structural != now.Structural) return HotGuard::Structural;
 if (!Bound(binding.Certificate) || !Bound(now.Certificate) || binding.Certificate != now.Certificate) return HotGuard::Certificate;
 if (!Bound(binding.Owner) || !Bound(now.Owner) || tag.VirtualThread != current->VirtualThread || binding.Owner != now.Owner) return HotGuard::Owner;
 if (tag.Context->Domain != current->Context->Domain) return HotGuard::Domain;
 if (*tag.Context != *current->Context) return HotGuard::Context;
 if (!Bound(binding.Boundary) || !Bound(now.Boundary) || binding.Boundary != now.Boundary) return HotGuard::Boundary;
 if (binding.Serializing != now.Serializing) return HotGuard::Serializing;
 if (candidate->ReplayGeneration != replayGeneration || binding.Phase != now.Phase) return HotGuard::ReplayPhase;
 if (binding.Cursor != now.Cursor) return HotGuard::Cursor;
 for (int value = static_cast<int>(HotGuard::Freshness); value <= static_cast<int>(HotGuard::Vdsa); ++value) {
  auto guard = static_cast<HotGuard>(value);
  if (observe) observe(guard);
  if (!inputs->CurrentCheck(guard)) return guard;
 }
 return std::nullopt;
}
bool HotCompatibility::Bound(const std::wstring& value) { return !StringHelper::isEmptyOrWhiteSpace(value) && value != L"unbound"; }
}
