#pragma once

#include "../Contracts/Contracts.h"
#include <string>
#include <functional>
#include <optional>
#include <memory>
#include "../../tangible_string_helper.h"

namespace HybridCpu::ShadowFrontend
{

	enum class HotGuard
	{
		Freshness,
		Integrity,
		Structural,
		Certificate,
		Owner,
		Context,
		Domain,
		Boundary,
		Serializing,
		ReplayPhase,
		Cursor,
		Legality,
		Capacity,
		Hazard,
		Scoreboard,
		Backpressure,
		Speculation,
		Assist,
		Fsp,
		Vdsa
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HotBinding(string Structural, string Certificate, string Owner, string Boundary, ulong Serializing, ulong Phase, int Cursor);
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HotCandidate(LookupTag Tag, SemanticIdentity Identity, HotBinding Binding, ulong ReplayGeneration);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public sealed record HotInputs(HotBinding Current, System.Func<HotGuard, bool> CurrentCheck, SemanticIdentity? ExpectedIdentity = null);
//	public sealed record HotInputs(HotBinding Current, System.Func<HotGuard, bool> CurrentCheck, SemanticIdentity ExpectedIdentity = nullptr);

    struct HotBinding final { std::wstring Structural, Certificate, Owner, Boundary; std::uint64_t Serializing = 0, Phase = 0; int Cursor = 0; };
    struct HotCandidate final { std::shared_ptr<LookupTag> Tag; std::shared_ptr<SemanticIdentity> Identity; std::shared_ptr<HotBinding> Binding; std::uint64_t ReplayGeneration = 0; };
    struct HotInputs final { std::shared_ptr<HotBinding> Current; std::function<bool(HotGuard)> CurrentCheck; std::shared_ptr<SemanticIdentity> ExpectedIdentity = nullptr; };

	class HotCompatibility final
	{
		// Consumes an externally supplied immutable existing replay observation. No store,
		// certificate issuance, authority, placement or issued-attempt allocation lives here.
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static System.Nullable<HotGuard> Reject(HotCandidate candidate, LookupTag current, ulong replayGeneration, HotInputs inputs, System.Action<HotGuard>? observe = null)
		static std::optional<HotGuard> Reject(std::shared_ptr<HotCandidate> candidate, std::shared_ptr<LookupTag> current, unsigned long long replayGeneration, std::shared_ptr<HotInputs> inputs, std::function<void(HotGuard)> observe = nullptr);
	private:
		static bool Bound(const std::wstring &value);
	};

}
