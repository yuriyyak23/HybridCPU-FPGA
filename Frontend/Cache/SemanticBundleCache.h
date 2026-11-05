#pragma once

#include "../Invalidation/FreshnessState.h"
#include "../Contracts/Contracts.h"
#include "../Telemetry/FrontendTelemetry.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <limits>
#include <span>
#include <any>
#include <functional>
#include <optional>
#include <mutex>
#include "tangible_exception_helper.h"
#include <memory>

namespace HybridCpu::ShadowFrontend
{

	enum class ReplacementPolicy
	{
		Lru,
		QuotaBorrow
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record CacheSnapshot(bool Enabled, int ResidentBytes, ImmutableArray<int> Occupancy, TelemetrySnapshot Counters, ReplacementPolicy Policy, byte ActiveVirtualThreads, FreshnessSnapshot Freshness);
    struct CacheSnapshot final {
        bool Enabled;
        int ResidentBytes;
        std::vector<int> Occupancy;
        std::shared_ptr<TelemetrySnapshot> Counters;
        ReplacementPolicy Policy;
        unsigned char ActiveVirtualThreads;
        std::shared_ptr<FreshnessSnapshot> Freshness;
    };
	class SemanticBundleCache final : public std::enable_shared_from_this<SemanticBundleCache>
	{
	private:
		const int ByteBudget;
		unsigned char ActiveVirtualThreads = 15;
		ReplacementPolicy Policy = ReplacementPolicy::QuotaBorrow;

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private sealed record Entry(LookupTag Tag, FrozenBytes Encoded, FrozenBytes Integrity, long LastUse, int Footprint);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<Entry>[] entries;
        struct Entry final { std::shared_ptr<LookupTag> Tag; std::shared_ptr<FrozenBytes> Encoded, Integrity; long long LastUse; int Footprint; };
		std::vector<std::optional<std::shared_ptr<Entry>>> entries;
		const int ways;
		long long clock = 0;
		mutable std::recursive_mutex ingressGate;
		const std::shared_ptr<FreshnessState> freshness;
		const std::function<std::shared_ptr<DecodeContext>(std::shared_ptr<BundleRequest>)> contextProvider;
		const std::shared_ptr<ICanonicalSource> source;
		const std::shared_ptr<ICanonicalDecoder> decoder;
		const std::shared_ptr<FrontendTelemetry> telemetry = std::make_shared<FrontendTelemetry>();
	public:
		std::shared_ptr<TelemetrySnapshot> getTelemetry() const;
	private:
		bool enabled = true;
	public:
		bool getEnabled() const;
		void setEnabled(bool value);
		int getByteBudget() const;
		unsigned char getActiveVirtualThreads() const;
	private:
		void setActiveVirtualThreads(unsigned char value);
	public:
		ReplacementPolicy getPolicy() const;
	private:
		void setPolicy(ReplacementPolicy value);
	public:
		int getCapacity() const;
		int getResidentBytes() const;
		std::vector<int> getOccupancy() const;
		std::shared_ptr<CacheSnapshot> Snapshot();
		SemanticBundleCache(std::shared_ptr<FreshnessState> freshness, std::function<std::shared_ptr<DecodeContext>(std::shared_ptr<BundleRequest>)> contextProvider, std::shared_ptr<ICanonicalSource> source, std::shared_ptr<ICanonicalDecoder> decoder, int capacity = 64, int ways = 4, int byteBudget = (std::numeric_limits<int>::max)());
		void Clear();
		void SetPolicy(ReplacementPolicy policy, unsigned char activeVts);
		void SetActiveVirtualThreads(unsigned char mask);
		std::shared_ptr<LookupTag> CaptureTag(std::shared_ptr<BundleRequest> request);
		std::shared_ptr<IngressResult> Request(std::shared_ptr<BundleRequest> request, std::optional<std::shared_ptr<HotCandidate>> candidate = std::nullopt, std::optional<std::shared_ptr<HotInputs>> hot = std::nullopt, int maxRetries = 2);
	private:
		std::shared_ptr<IngressResult> Fault(std::shared_ptr<BundleRequest> r, std::shared_ptr<AccessDecision> a);
		long long Tick();
		int SetStart(unsigned long long pc);
	public:
		static int TagBytes(std::shared_ptr<LookupTag> tag);
	private:
		static long long IdentityBytes(std::shared_ptr<SemanticIdentity> identity);
	public:
		std::shared_ptr<RetainedGraphAccounting> RetainedGraph();
	private:
		static std::vector<unsigned char> EntryIntegrity(std::shared_ptr<LookupTag> tag, std::span<const unsigned char> payload);
		int Victim(std::vector<int> &indices, int requestor);
		void Evict(int index);
		void Fill(std::shared_ptr<LookupTag> tag, std::vector<unsigned char> &encoded);
		// Controlled corruption experiment; no writable serving payload is exposed.
	public:
		bool CorruptForExperiment(unsigned long long pc);
		bool CorruptTagForExperiment(unsigned long long pc);
	};

}
