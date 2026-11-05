#pragma once
#include "CanonicalDecodeContext.h"
#include "CanonicalDecodedInstruction.h"
#include "CanonicalPayloadSnapshot.h"
#include "SemanticInstructionKey.h"
#include "DecodeFailure.h"
#include "Sha256Digest.h"

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <string>
#include <vector>
#include <span>
#include <stdexcept>
#include <any>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_string_helper.h"
#include "../../../tangible_stringbuilder.h"

namespace YAKSys_Hybrid_CPU::Core { class BundleMetadata; }
namespace YAKSys_Hybrid_CPU::Core::Decoder { class DecodedInstruction; }
using namespace HybridCPU_ISE::Arch;





namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// Stable reason code for expected decode rejection. RF-04 defines the carrier
	/// only; RF-05 maps legacy decoder rejection sites to this taxonomy.
	/// </summary>


	/// <summary>
	/// Typed expected-illegal-input result. It is deliberately not an exception and
	/// therefore cannot turn a programming/invariant failure into an ordinary stall.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DecodeFailure(DecodeFailureCode Code, int SlotIndex, string Field, string RawHash, string Message)
//	{
//		public static DecodeFailure Create(DecodeFailureCode code, int slotIndex, string field, ReadOnlySpan<byte> rawBytes, string message)
//		{
//			ArgumentException.ThrowIfNullOrWhiteSpace(field);
//			ArgumentException.ThrowIfNullOrWhiteSpace(message);
//			return new DecodeFailure(code, slotIndex, field, CanonicalPayloadSnapshot.Hash(rawBytes), message);
//		}
//	}

	/// <summary>
	/// Immutable deep-copy payload used at the canonical decode boundary. Content is
	/// a UTF-8 JSON snapshot, never a reference to mutable decoded sideband state.
	/// </summary>


	/// <summary>
	/// Full non-dynamic context needed for replay identity. The default context is
	/// explicit and non-replay-eligible: missing runtime evidence never aliases a
	/// valid context or silently enables cache serving.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record CanonicalDecodeContext
//	{
//		public required string ManifestVersion
//		{
//			get;
//			set;
//		}
//		public required string ManifestHash
//		{
//			get;
//			set;
//		}
//		public required string ExtensionConfigurationFingerprint
//		{
//			get;
//			set;
//		}
//		public required string DecoderEpoch
//		{
//			get;
//			set;
//		}
//		public required string DecoderVersion
//		{
//			get;
//			set;
//		}
//		public required string PrivilegeContext
//		{
//			get;
//			set;
//		}
//		public required string DomainIdentity
//		{
//			get;
//			set;
//		}
//		public required string AddressSpaceIdentity
//		{
//			get;
//			set;
//		}
//		public required string VectorConfigurationFingerprint
//		{
//			get;
//			set;
//		}
//		public required ulong ExecutableMemoryInvalidationEpoch
//		{
//			get;
//			set;
//		}
//		public required ulong CodeGenerationEpoch
//		{
//			get;
//			set;
//		}
//		public required bool IsReplayEligible
//		{
//			get;
//			set;
//		}
//
//		public static CanonicalDecodeContext Unbound
//		{
//			get;
//		} = new() { ManifestVersion = GeneratedIsaCatalog.ManifestVersion, ManifestHash = GeneratedIsaCatalog.ManifestSha256, ExtensionConfigurationFingerprint = "unbound", DecoderEpoch = "rf04-shadow", DecoderVersion = "VliwDecoderV4-legacy", PrivilegeContext = "unbound", DomainIdentity = "unbound", AddressSpaceIdentity = "unbound", VectorConfigurationFingerprint = "unbound", ExecutableMemoryInvalidationEpoch = 0, CodeGenerationEpoch = 0, IsReplayEligible = false};
//	}

	/// <summary>
	/// Immutable identity of decoded semantics. It intentionally has no admission,
	/// issue, attempt, or <c>VliwOperationId</c> field. Equality compares raw bytes,
	/// not only their digest, so a hash collision cannot create a replay identity hit.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct SemanticInstructionKey : IEquatable<SemanticInstructionKey>


	/// <summary>Immutable semantic projection for one fixed physical VLIW slot.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record CanonicalDecodedInstruction(int SlotIndex, bool IsOccupied, uint Opcode, System.Nullable<InstructionClass> InstructionClass, System.Nullable<SerializationClass> SerializationClass, byte Rd, byte Rs1, byte Rs2, long Immediate, System.Nullable<ushort> CsrAddress, bool AcquireOrdering, bool ReleaseOrdering, CanonicalPayloadSnapshot RawSlot, CanonicalPayloadSnapshot InstructionPayload, CanonicalPayloadSnapshot SlotSideband)
//	{
//		/// <summary>
//		/// The generated static binding captured once at the canonical decode boundary.
//		/// Empty slots have no binding; occupied generated slots must carry one.
//		/// </summary>
//		public System.Nullable<GeneratedStaticBinding> StaticBinding
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Derived classification only; no caller-settable security authority.</summary>
//		public CanonicalSecurityOperationKind SecurityOperationKind
//		{
//			get
//			{
//				return IsOccupied && (StaticBinding == nullptr ? nullptr : StaticBinding.Opcode) == Opcode ? CanonicalSecurityOperationIdentity.FromGeneratedBinding(StaticBinding) : CanonicalSecurityOperationKind.Unknown;
//			}
//		}
//
//		/// <summary>
//		/// Decoder-owned static scalar-load addressing shape. This deliberately
//		/// contains no resolved address, footprint, bank or runtime memory state.
//		/// </summary>
//		public System.Nullable<CanonicalScalarLoadAddressPlan> ScalarLoadAddressPlan
//		{
//			get;
//			set;
//		}
//	}

	/// <summary>
	/// Immutable static shape of a scalar load address calculation. It is not a
	/// <see cref="MemoryCapability"/> and must never be used as a resolved memory
	/// request, scheduler bank identity, footprint or completion token.
	/// </summary>
	class CanonicalScalarLoadAddressPlan final : public std::enable_shared_from_this<CanonicalScalarLoadAddressPlan>
	{
	private:
		const std::shared_ptr<GeneratedStaticBinding> GeneratedBinding;
		const unsigned char DestinationRegisterId;
		const unsigned char BaseRegisterId;
		const long long SignedDisplacement;
		const unsigned char AccessSize;

		CanonicalScalarLoadAddressPlan(std::shared_ptr<GeneratedStaticBinding> generatedBinding, unsigned char destinationRegisterId, unsigned char baseRegisterId, long long signedDisplacement, unsigned char accessSize);

	public:
		std::shared_ptr<GeneratedStaticBinding> getGeneratedBinding() const;
		unsigned char getDestinationRegisterId() const;
		unsigned char getBaseRegisterId() const;
		long long getSignedDisplacement() const;
		unsigned char getAccessSize() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryCreate(CanonicalDecodedInstruction canonical, out CanonicalScalarLoadAddressPlan? plan)
		static bool TryCreate(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<CanonicalScalarLoadAddressPlan> &plan);

		void EnsureMatches(std::shared_ptr<CanonicalDecodedInstruction> canonical);

	private:
		static bool TryResolveAccessSize(unsigned int opcode, unsigned char &accessSize);
	};

	/// <summary>
	/// Frozen canonical decoded bundle. This is additive shadow output in RF-04; the
	/// legacy <see cref="DecodedInstructionBundle"/> remains the active public adapter.
	/// </summary>
	class CanonicalBundle final : public std::enable_shared_from_this<CanonicalBundle>
	{
	private:
		const unsigned long long BundleAddress;
		const unsigned long long BundleSerial;
		const std::vector<std::shared_ptr<CanonicalDecodedInstruction>> Slots;
		const std::shared_ptr<CanonicalPayloadSnapshot> BundleSideband;
		const SemanticInstructionKey SemanticKey;

		CanonicalBundle(unsigned long long bundleAddress, unsigned long long bundleSerial, std::vector<std::shared_ptr<CanonicalDecodedInstruction>> slots, std::shared_ptr<CanonicalPayloadSnapshot> bundleSideband, SemanticInstructionKey semanticKey);

	public:
		unsigned long long getBundleAddress() const;
		unsigned long long getBundleSerial() const;
		std::vector<std::shared_ptr<CanonicalDecodedInstruction>> getSlots() const;
		std::shared_ptr<CanonicalPayloadSnapshot> getBundleSideband() const;
		SemanticInstructionKey getSemanticKey() const;
		bool isReplayEligible() const;

		std::shared_ptr<CanonicalDecodedInstruction> GetSlot(int slotIndex);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static CanonicalBundle Create(std::span<const VLIW_Instruction> rawSlots, IReadOnlyList<DecodedInstruction> decodedSlots, BundleMetadata bundleMetadata, ulong bundleAddress, ulong bundleSerial, System.Nullable<CanonicalDecodeContext> context = null)
		static std::shared_ptr<CanonicalBundle> Create(std::span<const VLIW_Instruction> rawSlots, const std::vector<std::shared_ptr<DecodedInstruction>> &decodedSlots, std::shared_ptr<BundleMetadata> bundleMetadata, unsigned long long bundleAddress, unsigned long long bundleSerial, std::shared_ptr<CanonicalDecodeContext> context = nullptr);

		/// <summary>
		/// Creates a frozen canonical bundle directly from the RF-05 immutable slot
		/// contracts. Unlike the legacy-adapter overload this path never observes an
		/// <see cref="InstructionIR"/> and therefore cannot reconstruct mutable IR.
		/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static CanonicalBundle CreateFromCanonicalSlots(std::span<const VLIW_Instruction> rawSlots, IReadOnlyList<CanonicalDecodedInstruction> canonicalSlots, BundleMetadata bundleMetadata, ulong bundleAddress, ulong bundleSerial, System.Nullable<CanonicalDecodeContext> context = null)
		static std::shared_ptr<CanonicalBundle> CreateFromCanonicalSlots(std::span<const VLIW_Instruction> rawSlots, const std::vector<std::shared_ptr<CanonicalDecodedInstruction>> &canonicalSlots, std::shared_ptr<BundleMetadata> bundleMetadata, unsigned long long bundleAddress, unsigned long long bundleSerial, std::shared_ptr<CanonicalDecodeContext> context = nullptr);

	private:
		static std::wstring ComputeAnnotationsHash(std::shared_ptr<CanonicalPayloadSnapshot> bundleSideband, const std::vector<std::shared_ptr<CanonicalDecodedInstruction>> &slots);
	};

}
