#pragma once

#include "../Contracts/RequestTypes.h"

#include <unordered_map>
#include <limits>
#include <any>
#include <functional>
#include <mutex>
#include "tangible_exception_helper.h"
#include <memory>

namespace HybridCpu::ShadowFrontend

{
        struct FreshnessSnapshot;

	enum class FrontendEvent
	{
		ExecutableWrite,
		FenceI,
		Reset,
		Restore,
		Context,
		Domain,
		Privilege,
		Configuration,
		Sideband,
		Policy,
		Redirect,
		Trap,
		Rollback,
		ReplayPhase,
		Serializing
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record FreshnessSnapshot(FreshnessToken Token, ulong ReplayGeneration, ulong CommittedOrder, bool Poisoned, ImmutableDictionary<FrontendEvent,long> Invalidations);
	    struct FreshnessSnapshot final {
        FreshnessToken Token;
        unsigned long long ReplayGeneration, CommittedOrder;
        bool Poisoned;
        std::unordered_map<FrontendEvent, long long> Invalidations;
    };
class FreshnessState final : public std::enable_shared_from_this<FreshnessState>
	{
	private:
		mutable std::recursive_mutex Gate;

	public:
		std::recursive_mutex& getGate() const;
	private:
		unsigned long long incarnation = 1, generation = 0, replayGeneration = 0, committedOrder = 0;
		bool poisoned = false;
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [ThreadStatic] private static int publicationDepth;
		static inline thread_local int publicationDepth = 0;
	public:
		static void RejectCacheLockFromPublication();
	private:
		void EnsureUsable() const;
	public:
		std::shared_ptr<FreshnessToken> Capture();
		unsigned long long getReplayGeneration() const;
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<FrontendEvent, long> invalidations = new();
		std::unordered_map<FrontendEvent, long long> invalidations = std::unordered_map<FrontendEvent, long long>();
	public:
		std::unordered_map<FrontendEvent, long long> getInvalidations() const;
		std::shared_ptr<FreshnessSnapshot> Snapshot();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void Apply(FrontendEvent kind, ulong order, bool committed = true, System.Action? publish = null)
		void Apply(FrontendEvent kind, unsigned long long order, bool committed = true, std::function<void()> publish = nullptr);
	private:
		void Advance(unsigned long long &value);
		// Explicit model initialization for overflow experiments, never a restore API.
	public:
		static std::shared_ptr<FreshnessState> OverflowExperiment();
		// New isolated model lifetime only; never restore an existing cache or poisoned state.
		static std::shared_ptr<FreshnessState> ObservationExperiment(std::shared_ptr<FreshnessToken> token, unsigned long long replay, unsigned long long order);
	};

}
