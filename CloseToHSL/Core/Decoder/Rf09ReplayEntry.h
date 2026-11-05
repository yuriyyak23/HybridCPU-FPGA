#pragma once

#include "CanonicalDecodedContracts.h"
#include "ReplayShadowTypes.h"
#include <string>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_stringbuilder.h"

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	class CanonicalBundle;

	/// <summary>
	/// Immutable validation witness for one frozen semantic replay entry.
	/// The digest is an integrity witness over canonical decoded content; it is
	/// never sufficient for lookup equality and carries no scheduler decision or
	/// issued-attempt identity.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ReplayEntryValidationFingerprint(ushort SchemaVersion, string CanonicalContentSha256)
//	{
//		public const ushort CurrentSchemaVersion = 1;
//
//		public bool IsValid
//		{
//			get
//			{
//				return SchemaVersion == CurrentSchemaVersion && CanonicalContentSha256 is { Length: 64 };
//			}
//		}
//	}

	/// <summary>
	/// Frozen cacheable decode result for RF-09 semantic replay.
	///
	/// This contract is deliberately not a serving cache. It owns only immutable
	/// canonical decode data and semantic lookup identity. Replay lookup,
	/// nomination and Stage A must not allocate a VLIW operation identity; each
	/// successful attempt remains responsible for its own post-Stage-B identity.
	/// </summary>
	class ReplayEntry final : public std::enable_shared_from_this<ReplayEntry>
	{
	private:
		const SemanticInstructionKey SemanticKey;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> CanonicalBundle;
		const std::shared_ptr<ReplayEntryValidationFingerprint> ValidationFingerprint;

		ReplayEntry(SemanticInstructionKey semanticKey, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle, std::shared_ptr<ReplayEntryValidationFingerprint> validationFingerprint);

	public:
		SemanticInstructionKey getSemanticKey() const;

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> getCanonicalBundle() const;

		std::shared_ptr<ReplayEntryValidationFingerprint> getValidationFingerprint() const;

		/// <summary>
		/// Freezes an explicitly replay-eligible canonical bundle.
		/// Unbound decode context is rejected rather than silently becoming a
		/// cache-serving identity.
		/// </summary>
		static std::shared_ptr<ReplayEntry> Create(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle);

		/// <summary>
		/// Revalidates the frozen entry without treating its digest as lookup
		/// equality. Semantic key equality remains the authoritative comparison.
		/// </summary>
		bool HasValidFrozenContent();

		/// <summary>
		/// Compares full frozen semantic content after authoritative semantic-key
		/// equality. Bundle address and transport serial are intentionally excluded:
		/// they are placement/transport facts, not decoded semantic identity.
		/// </summary>
		bool HasSameFrozenSemanticContent(std::shared_ptr<ReplayEntry> other);

	private:
		static std::shared_ptr<ReplayEntryValidationFingerprint> ComputeValidationFingerprint(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle);

		static void Append(std::shared_ptr<StringBuilder> builder, const std::wstring &value);
	};

}
