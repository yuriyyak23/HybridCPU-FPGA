#include "FrontendTelemetry.h"
#include <numeric>
#include <stdexcept>
namespace HybridCpu::ShadowFrontend {
std::unordered_map<HotGuard, long long>& FrontendTelemetry::getGuardRejects() { return GuardRejects; }
const std::unordered_map<HotGuard, long long>& FrontendTelemetry::getGuardRejects() const { return GuardRejects; }
void FrontendTelemetry::setGuardRejects(const std::unordered_map<HotGuard, long long>& value) { GuardRejects = value; }
std::shared_ptr<TelemetrySnapshot> FrontendTelemetry::Snapshot() { return std::make_shared<TelemetrySnapshot>(shared_from_this()); }
long long FrontendTelemetry::getCompleted() const { return ColdIngress + SemanticHits + ValidatedHotHits; }
void FrontendTelemetry::VerifyAccounting() { if (Requests != getCompleted() + AccessFaults + RetryExhausted || DecodeBypassCount != SemanticHits + ValidatedHotHits || RawBundleFetches != DecoderCalls || CanonicalBytesRead != 256 * RawBundleFetches || std::accumulate(HitsByVt.begin(), HitsByVt.end(), 0LL) != SemanticHits || DecodeFaults > getCompleted() || Fills > DecoderCalls || Fills < 0 || FetchAttempts < RawBundleFetches) throw std::logic_error("Frontend accounting invariant failed."); }
static std::unordered_map<std::wstring, long long> captureValues(const FrontendTelemetry& s) { return {
 {L"Requests", s.Requests},
 {L"ColdIngress", s.ColdIngress},
 {L"SemanticHits", s.SemanticHits},
 {L"SemanticMisses", s.SemanticMisses},
 {L"HotCandidates", s.HotCandidates},
 {L"ValidatedHotHits", s.ValidatedHotHits},
 {L"RawBundleFetches", s.RawBundleFetches},
 {L"DecoderCalls", s.DecoderCalls},
 {L"CanonicalBytesRead", s.CanonicalBytesRead},
 {L"SemanticPayloadBytesRead", s.SemanticPayloadBytesRead},
 {L"FetchAttempts", s.FetchAttempts},
 {L"Fills", s.Fills},
 {L"PermissionChecks", s.PermissionChecks},
 {L"HotGuardChecks", s.HotGuardChecks},
 {L"RestoreAttempts", s.RestoreAttempts},
 {L"HotIdentityBytesRead", s.HotIdentityBytesRead},
 {L"DbcLookups", s.DbcLookups},
 {L"LookupIntegrityChecks", s.LookupIntegrityChecks},
 {L"HostIngressWaitTicks", s.HostIngressWaitTicks},
 {L"HostIngressHoldTicks", s.HostIngressHoldTicks},
 {L"TagGuardBytesRead", s.TagGuardBytesRead},
 {L"IntegrityBytesRead", s.IntegrityBytesRead},
 {L"FillBytesWritten", s.FillBytesWritten},
 {L"DecodeBypassCount", s.DecodeBypassCount},
 {L"Retries", s.Retries},
 {L"RetryExhausted", s.RetryExhausted},
 {L"AccessFaults", s.AccessFaults},
 {L"DecodeFaults", s.DecodeFaults},
 {L"UnsupportedFills", s.UnsupportedFills},
 {L"CorruptEntries", s.CorruptEntries},
}; }
TelemetrySnapshot::TelemetrySnapshot(std::shared_ptr<FrontendTelemetry> source) : HitsByVt(source->HitsByVt), MissesByVt(source->MissesByVt), EvictionsByVt(source->EvictionsByVt), GuardRejects(source->getGuardRejects()), values(captureValues(*source)) {}
std::vector<long long> TelemetrySnapshot::getHitsByVt() const { return HitsByVt; }
std::vector<long long> TelemetrySnapshot::getMissesByVt() const { return MissesByVt; }
std::vector<long long> TelemetrySnapshot::getEvictionsByVt() const { return EvictionsByVt; }
std::unordered_map<HotGuard, long long> TelemetrySnapshot::getGuardRejects() const { return GuardRejects; }
std::any TelemetrySnapshot::Deterministic() { auto scalars = values; scalars.erase(L"HostIngressWaitTicks"); scalars.erase(L"HostIngressHoldTicks"); return DeterministicTelemetry{std::move(scalars), HitsByVt, MissesByVt, EvictionsByVt, GuardRejects}; }
long long TelemetrySnapshot::getCompleted() const { return getColdIngress() + getSemanticHits() + getValidatedHotHits(); }
void TelemetrySnapshot::VerifyAccounting() { if (getRequests() != getCompleted() + getAccessFaults() + getRetryExhausted() || getDecodeBypassCount() != getSemanticHits() + getValidatedHotHits() || getRawBundleFetches() != getDecoderCalls() || getCanonicalBytesRead() != 256 * getRawBundleFetches() || std::accumulate(HitsByVt.begin(), HitsByVt.end(), 0LL) != getSemanticHits() || getDecodeFaults() > getCompleted() || getFills() > getDecoderCalls() || getFills() < 0 || getFetchAttempts() < getRawBundleFetches()) throw std::logic_error("Frontend accounting invariant failed."); }
long long TelemetrySnapshot::getRequests() const { return values.at(L"Requests"); }
long long TelemetrySnapshot::getColdIngress() const { return values.at(L"ColdIngress"); }
long long TelemetrySnapshot::getSemanticHits() const { return values.at(L"SemanticHits"); }
long long TelemetrySnapshot::getSemanticMisses() const { return values.at(L"SemanticMisses"); }
long long TelemetrySnapshot::getHotCandidates() const { return values.at(L"HotCandidates"); }
long long TelemetrySnapshot::getValidatedHotHits() const { return values.at(L"ValidatedHotHits"); }
long long TelemetrySnapshot::getRawBundleFetches() const { return values.at(L"RawBundleFetches"); }
long long TelemetrySnapshot::getDecoderCalls() const { return values.at(L"DecoderCalls"); }
long long TelemetrySnapshot::getCanonicalBytesRead() const { return values.at(L"CanonicalBytesRead"); }
long long TelemetrySnapshot::getSemanticPayloadBytesRead() const { return values.at(L"SemanticPayloadBytesRead"); }
long long TelemetrySnapshot::getFetchAttempts() const { return values.at(L"FetchAttempts"); }
long long TelemetrySnapshot::getFills() const { return values.at(L"Fills"); }
long long TelemetrySnapshot::getPermissionChecks() const { return values.at(L"PermissionChecks"); }
long long TelemetrySnapshot::getHotGuardChecks() const { return values.at(L"HotGuardChecks"); }
long long TelemetrySnapshot::getRestoreAttempts() const { return values.at(L"RestoreAttempts"); }
long long TelemetrySnapshot::getHotIdentityBytesRead() const { return values.at(L"HotIdentityBytesRead"); }
long long TelemetrySnapshot::getDbcLookups() const { return values.at(L"DbcLookups"); }
long long TelemetrySnapshot::getLookupIntegrityChecks() const { return values.at(L"LookupIntegrityChecks"); }
long long TelemetrySnapshot::getHostIngressWaitTicks() const { return values.at(L"HostIngressWaitTicks"); }
long long TelemetrySnapshot::getHostIngressHoldTicks() const { return values.at(L"HostIngressHoldTicks"); }
long long TelemetrySnapshot::getTagGuardBytesRead() const { return values.at(L"TagGuardBytesRead"); }
long long TelemetrySnapshot::getIntegrityBytesRead() const { return values.at(L"IntegrityBytesRead"); }
long long TelemetrySnapshot::getFillBytesWritten() const { return values.at(L"FillBytesWritten"); }
long long TelemetrySnapshot::getDecodeBypassCount() const { return values.at(L"DecodeBypassCount"); }
long long TelemetrySnapshot::getRetries() const { return values.at(L"Retries"); }
long long TelemetrySnapshot::getRetryExhausted() const { return values.at(L"RetryExhausted"); }
long long TelemetrySnapshot::getAccessFaults() const { return values.at(L"AccessFaults"); }
long long TelemetrySnapshot::getDecodeFaults() const { return values.at(L"DecodeFaults"); }
long long TelemetrySnapshot::getUnsupportedFills() const { return values.at(L"UnsupportedFills"); }
long long TelemetrySnapshot::getCorruptEntries() const { return values.at(L"CorruptEntries"); }
}
