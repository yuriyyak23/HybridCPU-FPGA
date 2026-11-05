#pragma once

#include "FrozenBytes.h"
#include "RequestTypes.h"
#include "SemanticRecords.h"
#include "DecodeContext.h"

#include <vector>
#include <stdexcept>
#include <any>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>

namespace HybridCpu::ShadowFrontend
{
    struct AccessDecision; struct CanonicalFetchResult;

	// No interpretation factor has been projected away. Binding is a trusted-port claim,
	// not inferred from a hash or from a caller's numeric ASID.
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DecodeContext(string ManifestVersion, string ManifestHash, string Extensions, string DecoderEpoch, string DecoderVersion, string Privilege, string Domain, string AddressSpace, string VectorConfiguration, ulong ExecutableEpoch, ulong CodeEpoch, bool ReplayEligible, string ContextIncarnation, string SidebandIdentity)
//	{
//		public bool IsBound
//		{
//			get
//			{
//				return ReplayEligible && new[] { ManifestVersion, ManifestHash, Extensions, DecoderEpoch, DecoderVersion, Privilege, Domain, AddressSpace, VectorConfiguration, ContextIncarnation, SidebandIdentity } .All(s => !string.IsNullOrWhiteSpace(s) && s != "unbound");
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record BundleRequest(ulong Pc, int VirtualThread, ulong TransportSerial, int Cursor = 0)
//	{
//		public void Validate()
//		{
//			if (VirtualThread is < 0 or > 3 || Cursor is < 0 or > 8 || Pc > ulong.MaxValue - 255)
//				throw new ArgumentOutOfRangeException(nameof(BundleRequest));
//		}
//	}
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct FreshnessToken(ulong Incarnation, ulong Generation);
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record LookupTag(ulong Pc, int VirtualThread, DecodeContext Context, FreshnessToken Freshness);

    struct LookupTag final {
        std::uint64_t Pc = 0;
        int VirtualThread = 0;
        std::shared_ptr<DecodeContext> Context;
        FreshnessToken Freshness;
    };

	// Immutable byte strings are deep copies; consumers cannot recover a writable backing array.
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record Snapshot(string Kind, FrozenBytes Content);
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record SemanticSlot(int SlotIndex, bool IsOccupied, uint Opcode, System.Nullable<int> InstructionClass, System.Nullable<int> SerializationClass, byte Rd, byte Rs1, byte Rs2, long Immediate, System.Nullable<ushort> CsrAddress, bool AcquireOrdering, bool ReleaseOrdering, Snapshot RawSlot, Snapshot InstructionPayload, Snapshot SlotSideband, Snapshot StaticBinding, Snapshot ScalarLoadAddressPlan);
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record TypedDecodeFailure(ushort Code, int SlotIndex, string Field, string RawHash, string Message);

	// PC/transport serial/cursor are intentionally absent from reusable facts.
	class DecodeResult final : public std::enable_shared_from_this<DecodeResult>
	{
	private:
		const int Schema;
		const std::shared_ptr<FrozenBytes> Raw;
		const std::vector<std::shared_ptr<SemanticSlot>> Slots;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<Snapshot> BundleSideband;
		std::optional<std::shared_ptr<Snapshot>> BundleSideband;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<TypedDecodeFailure> Failure;
		std::optional<std::shared_ptr<TypedDecodeFailure>> Failure;

		DecodeResult(int schema, std::shared_ptr<FrozenBytes> raw, std::vector<std::shared_ptr<SemanticSlot>> slots, std::optional<std::shared_ptr<Snapshot>> &sideband, std::optional<std::shared_ptr<TypedDecodeFailure>> &failure);
	public:
		int getSchema() const;
		std::shared_ptr<FrozenBytes> getRaw() const;
		std::vector<std::shared_ptr<SemanticSlot>> getSlots() const;
		std::optional<std::shared_ptr<Snapshot>> getBundleSideband() const;
		std::optional<std::shared_ptr<TypedDecodeFailure>> getFailure() const;
		bool isSuccess() const;
		static std::shared_ptr<DecodeResult> Success(std::shared_ptr<FrozenBytes> raw, std::vector<std::shared_ptr<SemanticSlot>> &slots, std::shared_ptr<Snapshot> sideband, int schema = 1);
		static std::shared_ptr<DecodeResult> Illegal(std::shared_ptr<FrozenBytes> raw, std::shared_ptr<TypedDecodeFailure> failure, int schema = 1);
		bool SameFacts(std::shared_ptr<DecodeResult> other);
	};
	class SemanticIdentity final
	{
	private:
		const std::shared_ptr<DecodeContext> Context;
		const std::shared_ptr<DecodeResult> Result;

	public:
		SemanticIdentity(std::shared_ptr<DecodeContext> context, std::shared_ptr<DecodeResult> result);
		std::shared_ptr<DecodeContext> getContext() const;
		std::shared_ptr<DecodeResult> getResult() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool Equals(SemanticIdentity? other)
		bool Equals(std::shared_ptr<SemanticIdentity> other) ;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj);
		int GetHashCode();
	};
	enum class IngressOutcome
	{
		Cold,
		Warm,
		Hot,
		AccessFault,
		RetryExhausted
	};
    struct RetainedGraphAccounting final {
        int Entries, UniqueContexts, UniqueStrings;
        long long StringContentBytes, EncodedContentBytes, IntegrityContentBytes, ReferenceSlots, ModeledEntryBytes;
        std::wstring Scope;
    };
    struct IngressResult final {
        std::shared_ptr<BundleRequest> Placement;
        IngressOutcome Outcome;
        std::shared_ptr<DecodeResult> Facts;
        std::optional<std::wstring> AccessFault;
    };
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record RetainedGraphAccounting(int Entries, int UniqueContexts, int UniqueStrings, long StringContentBytes, long EncodedContentBytes, long IntegrityContentBytes, long ReferenceSlots, long ModeledEntryBytes, string Scope);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public sealed record IngressResult(BundleRequest Placement, IngressOutcome Outcome, DecodeResult? Facts, System.Nullable<string> AccessFault);
//	public sealed record IngressResult(BundleRequest Placement, IngressOutcome Outcome, DecodeResult Facts, string AccessFault);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public sealed record AccessDecision(bool Allowed, string? Fault)
//	public sealed record AccessDecision(bool Allowed, string Fault)
//	{
//		public static AccessDecision Permit
//		{
//			get;
//		} = new(true, nullptr);
//	}
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public sealed record CanonicalFetchResult(FrozenBytes? Raw, System.Nullable<string> Fault);
//	public sealed record CanonicalFetchResult(FrozenBytes Raw, string Fault);
    struct AccessDecision final {
        const bool Allowed;
        const std::optional<std::wstring> Fault;
        static const std::shared_ptr<AccessDecision> Permit;
    };
    inline const std::shared_ptr<AccessDecision> AccessDecision::Permit = std::make_shared<AccessDecision>(AccessDecision{true, std::nullopt});
    struct CanonicalFetchResult final {
        std::shared_ptr<FrozenBytes> Raw;
        std::optional<std::wstring> Fault;
    };
	class ICanonicalSource
	{
		// Must validate translation/execute permission for ALL covered executable regions.
	public:
		virtual std::shared_ptr<AccessDecision> CheckAccess(std::shared_ptr<BundleRequest> request, std::shared_ptr<DecodeContext> context) = 0;
		virtual std::shared_ptr<CanonicalFetchResult> FetchFullBundle(std::shared_ptr<BundleRequest> request, std::shared_ptr<DecodeContext> context) = 0;
	};
	class ICanonicalDecoder
	{
		// Only canonical oracle adapters implement this port. No Shadow opcode decoder.
	public:
		virtual std::shared_ptr<DecodeResult> Decode(std::shared_ptr<FrozenBytes> raw, std::shared_ptr<BundleRequest> request, std::shared_ptr<DecodeContext> context) = 0;
	};

}
