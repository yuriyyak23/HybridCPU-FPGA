#pragma once

#include "../Replay/HotCompatibility.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include "tangible_exception_helper.h"
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace HybridCpu::ShadowFrontend { class TelemetrySnapshot; }

namespace HybridCpu::ShadowFrontend
{

	class FrontendTelemetry final : public std::enable_shared_from_this<FrontendTelemetry>
	{
	private:
		std::unordered_map<HotGuard, long long> GuardRejects = std::unordered_map<HotGuard, long long>();

	public:
		long long Requests = 0, ColdIngress = 0, SemanticHits = 0, SemanticMisses = 0, HotCandidates = 0, ValidatedHotHits = 0;
		long long RawBundleFetches = 0, DecoderCalls = 0, CanonicalBytesRead = 0, SemanticPayloadBytesRead = 0;
		long long FetchAttempts = 0;
		long long Fills = 0;
		long long PermissionChecks = 0, HotGuardChecks = 0, RestoreAttempts = 0, HotIdentityBytesRead = 0;
		long long DbcLookups = 0, LookupIntegrityChecks = 0, HostIngressWaitTicks = 0, HostIngressHoldTicks = 0;
		long long TagGuardBytesRead = 0, IntegrityBytesRead = 0, FillBytesWritten = 0, DecodeBypassCount = 0;
		long long Retries = 0, RetryExhausted = 0, AccessFaults = 0, DecodeFaults = 0, UnsupportedFills = 0, CorruptEntries = 0;
		std::vector<long long> HitsByVt = std::vector<long long>(4), MissesByVt = std::vector<long long>(4), EvictionsByVt = std::vector<long long>(4);
		std::unordered_map<HotGuard, long long>& getGuardRejects();
		const std::unordered_map<HotGuard, long long>& getGuardRejects() const;
	private:
		void setGuardRejects(const std::unordered_map<HotGuard, long long> &value);
	public:
		std::shared_ptr<TelemetrySnapshot> Snapshot();
		long long getCompleted() const;
		void VerifyAccounting();
	};

	// Read-only observation; no live mutable arrays or dictionaries escape the cache.
	struct DeterministicTelemetry final { std::unordered_map<std::wstring, long long> Scalars; std::vector<long long> HitsByVt, MissesByVt, EvictionsByVt; std::unordered_map<HotGuard, long long> GuardRejects; };
	class TelemetrySnapshot final : public std::enable_shared_from_this<TelemetrySnapshot>
	{
	private:
		const std::vector<long long> HitsByVt;
		const std::vector<long long> MissesByVt;
		const std::vector<long long> EvictionsByVt;
		const std::unordered_map<HotGuard, long long> GuardRejects;

		const std::unordered_map<std::wstring, long long> values;
	public:
		std::vector<long long> getHitsByVt() const;
		std::vector<long long> getMissesByVt() const;
		std::vector<long long> getEvictionsByVt() const;
		std::unordered_map<HotGuard, long long> getGuardRejects() const;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		TelemetrySnapshot(std::shared_ptr<FrontendTelemetry> source);
		std::any Deterministic();
		long long getCompleted() const;
		void VerifyAccounting();
		long long getRequests() const;
		long long getColdIngress() const;
		long long getSemanticHits() const;
		long long getSemanticMisses() const;
		long long getHotCandidates() const;
		long long getValidatedHotHits() const;
		long long getRawBundleFetches() const;
		long long getDecoderCalls() const;
		long long getCanonicalBytesRead() const;
		long long getSemanticPayloadBytesRead() const;
		long long getFetchAttempts() const;
		long long getFills() const;
		long long getPermissionChecks() const;
		long long getHotGuardChecks() const;
		long long getRestoreAttempts() const;
		long long getHotIdentityBytesRead() const;
		long long getDbcLookups() const;
		long long getLookupIntegrityChecks() const;
		long long getHostIngressWaitTicks() const;
		long long getHostIngressHoldTicks() const;
		long long getTagGuardBytesRead() const;
		long long getIntegrityBytesRead() const;
		long long getFillBytesWritten() const;
		long long getDecodeBypassCount() const;
		long long getRetries() const;
		long long getRetryExhausted() const;
		long long getAccessFaults() const;
		long long getDecodeFaults() const;
		long long getUnsupportedFills() const;
		long long getCorruptEntries() const;
	};

}
