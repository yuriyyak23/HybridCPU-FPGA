$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$header = Join-Path $root 'Frontend\\Telemetry\FrontendTelemetry.h'
$cpp = Join-Path $root 'Frontend\\Telemetry\FrontendTelemetry.cpp'
$s = Get-Content -LiteralPath $header -Raw
$names = [regex]::Matches($s, 'long long get(\w+)\(\) const;') | ForEach-Object { $_.Groups[1].Value } | Where-Object { $_ -ne 'Completed' }
$names = @($names | Select-Object -Unique)
$lines = [System.Collections.Generic.List[string]]::new()
$lines.Add('#include "FrontendTelemetry.h"')
$lines.Add('#include <numeric>')
$lines.Add('#include <stdexcept>')
$lines.Add('namespace HybridCpu::ShadowFrontend {')
$lines.Add('std::unordered_map<HotGuard, long long>& FrontendTelemetry::getGuardRejects() { return GuardRejects; }')
$lines.Add('const std::unordered_map<HotGuard, long long>& FrontendTelemetry::getGuardRejects() const { return GuardRejects; }')
$lines.Add('void FrontendTelemetry::setGuardRejects(const std::unordered_map<HotGuard, long long>& value) { GuardRejects = value; }')
$lines.Add('std::shared_ptr<TelemetrySnapshot> FrontendTelemetry::Snapshot() { return std::make_shared<TelemetrySnapshot>(shared_from_this()); }')
$lines.Add('long long FrontendTelemetry::getCompleted() const { return ColdIngress + SemanticHits + ValidatedHotHits; }')
$lines.Add('void FrontendTelemetry::VerifyAccounting() { if (Requests != getCompleted() + AccessFaults + RetryExhausted || DecodeBypassCount != SemanticHits + ValidatedHotHits || RawBundleFetches != DecoderCalls || CanonicalBytesRead != 256 * RawBundleFetches || std::accumulate(HitsByVt.begin(), HitsByVt.end(), 0LL) != SemanticHits || DecodeFaults > getCompleted() || Fills > DecoderCalls || Fills < 0 || FetchAttempts < RawBundleFetches) throw std::logic_error("Frontend accounting invariant failed."); }')
$lines.Add('static std::unordered_map<std::wstring, long long> captureValues(const FrontendTelemetry& s) { return {')
foreach ($name in $names) { $lines.Add((' {{L"{0}", s.{0}}},' -f $name)) }
$lines.Add('}; }')
$lines.Add('TelemetrySnapshot::TelemetrySnapshot(std::shared_ptr<FrontendTelemetry> source) : HitsByVt(source->HitsByVt), MissesByVt(source->MissesByVt), EvictionsByVt(source->EvictionsByVt), GuardRejects(source->getGuardRejects()), values(captureValues(*source)) {}')
foreach ($name in @('HitsByVt','MissesByVt','EvictionsByVt')) { $lines.Add(('std::vector<long long> TelemetrySnapshot::get{0}() const {{ return {0}; }}' -f $name)) }
$lines.Add('std::unordered_map<HotGuard, long long> TelemetrySnapshot::getGuardRejects() const { return GuardRejects; }')
$lines.Add('std::any TelemetrySnapshot::Deterministic() { auto scalars = values; scalars.erase(L"HostIngressWaitTicks"); scalars.erase(L"HostIngressHoldTicks"); return DeterministicTelemetry{std::move(scalars), HitsByVt, MissesByVt, EvictionsByVt, GuardRejects}; }')
$lines.Add('long long TelemetrySnapshot::getCompleted() const { return getColdIngress() + getSemanticHits() + getValidatedHotHits(); }')
$lines.Add('void TelemetrySnapshot::VerifyAccounting() { if (getRequests() != getCompleted() + getAccessFaults() + getRetryExhausted() || getDecodeBypassCount() != getSemanticHits() + getValidatedHotHits() || getRawBundleFetches() != getDecoderCalls() || getCanonicalBytesRead() != 256 * getRawBundleFetches() || std::accumulate(HitsByVt.begin(), HitsByVt.end(), 0LL) != getSemanticHits() || getDecodeFaults() > getCompleted() || getFills() > getDecoderCalls() || getFills() < 0 || getFetchAttempts() < getRawBundleFetches()) throw std::logic_error("Frontend accounting invariant failed."); }')
foreach ($name in $names) { $lines.Add(('long long TelemetrySnapshot::get{0}() const {{ return values.at(L"{0}"); }}' -f $name)) }
$lines.Add('}')
Set-Content -LiteralPath $cpp -Value $lines -Encoding UTF8
