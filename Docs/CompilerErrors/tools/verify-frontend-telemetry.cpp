#include "../../Frontend/Telemetry/FrontendTelemetry.h"
#include <cassert>
using namespace HybridCpu::ShadowFrontend;
int main() {
 auto t = std::make_shared<FrontendTelemetry>();
 t->Requests=1; t->SemanticHits=1; t->DecodeBypassCount=1; t->HitsByVt[0]=1; t->HostIngressWaitTicks=9; t->getGuardRejects()[HotGuard::Owner]=2;
 t->VerifyAccounting(); auto s=t->Snapshot(); t->HitsByVt[0]=0; t->getGuardRejects()[HotGuard::Owner]=5; t->Requests=2;
 assert(s->getHitsByVt()[0]==1 && s->getGuardRejects().at(HotGuard::Owner)==2 && s->getRequests()==1); s->VerifyAccounting();
 auto d=std::any_cast<DeterministicTelemetry>(s->Deterministic()); assert(d.Scalars.count(L"Requests") && !d.Scalars.count(L"HostIngressWaitTicks") && !d.Scalars.count(L"HostIngressHoldTicks"));
 bool failed=false; try { t->VerifyAccounting(); } catch(const std::logic_error&) { failed=true; } assert(failed);
}
