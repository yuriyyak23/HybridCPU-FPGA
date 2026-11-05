#include "../../Frontend/Invalidation/FreshnessState.h"
#include <cassert>
#include <future>
#include <chrono>
using namespace HybridCpu::ShadowFrontend;
using namespace std::chrono_literals;
int main() {
 FreshnessState s;
 { std::lock_guard lock(s.getGate()); assert(s.Capture()->Generation == 0); }
 std::promise<void> entered, release;
 auto released = release.get_future().share();
 auto writer = std::async(std::launch::async, [&] { s.Apply(FrontendEvent::ExecutableWrite, 1, true, [&] {
   bool rejected = false; try { FreshnessState::RejectCacheLockFromPublication(); } catch(const std::logic_error&) { rejected = true; } assert(rejected);
   auto other = std::async(std::launch::async, [] { FreshnessState::RejectCacheLockFromPublication(); }); other.get();
   entered.set_value(); released.wait();
 }); });
 entered.get_future().wait();
 auto reader = std::async(std::launch::async, [&] { return s.Capture(); });
 assert(reader.wait_for(100ms) == std::future_status::timeout);
 release.set_value(); writer.get(); assert(reader.get()->Generation == 1);
 auto overflow = FreshnessState::OverflowExperiment(); overflow->Apply(FrontendEvent::FenceI, 1);
 auto snap = overflow->Snapshot(); assert(snap->Token.Incarnation == 2 && snap->Token.Generation == 0 && snap->ReplayGeneration == 1);
 auto maximum = std::numeric_limits<unsigned long long>::max();
 auto terminal = FreshnessState::ObservationExperiment(std::make_shared<FreshnessToken>(FreshnessToken{maximum,maximum}), 0, 0);
 bool caught=false; try { terminal->Apply(FrontendEvent::FenceI, 1); } catch(const std::overflow_error&) { caught=true; } assert(caught && terminal->Snapshot()->Poisoned);
 caught=false; try { terminal->Capture(); } catch(const std::logic_error&) { caught=true; } assert(caught);
 FreshnessState failed; caught=false; try { failed.Apply(FrontendEvent::FenceI, 1, true, [] { throw std::runtime_error("failure"); }); } catch(const std::runtime_error&) { caught=true; } assert(caught && failed.Snapshot()->Poisoned);
}
