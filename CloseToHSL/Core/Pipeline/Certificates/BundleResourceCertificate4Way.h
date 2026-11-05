#pragma once

#include "YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"
#include "../Scheduling/SlotLegality/SlotClassCapacity.h"
#include "StructuralIdentity/HardwareHash.h"
#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <string>
#include <any>
#include <format>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Fine-grained rejection details from certificate check.
	/// Distinguishes shared resource vs register group conflict.
	/// <para>
	/// HLS design note: 2-bit reason MUX — no additional timing cost
	/// beyond the existing <see cref="BundleResourceCertificate4Way.CanInject"/> logic.
	/// </para>
	/// </summary>
	enum class CertificateRejectDetail : unsigned char
	{
		/// <summary>No rejection — candidate was accepted.</summary>
		None = 0,

		/// <summary>Shared resource conflict (bits 32–127): memory domains, DMA, LSU, Accel.</summary>
		SharedResourceConflict = 1,

		/// <summary>Per-VT register group conflict (bits 0–31): RAW, WAR, or WAW hazard.</summary>
		RegisterGroupConflict = 2
	};

	/// <summary>
	/// Structural replay/template identity for <see cref="BundleResourceCertificate4Way"/>.
	/// This is the explicit compatibility seam used for replay reuse instead of comparing
	/// raw shared/register mask snapshots in scheduler code.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BundleResourceCertificateIdentity4Way : IEquatable<BundleResourceCertificateIdentity4Way>
	class BundleResourceCertificateIdentity4Way
	{
	private:
		unsigned long long BundleShapeId{};
		unsigned int ClassVector{};
		unsigned short LaneTopologySignature{};
		unsigned char ResourceProfile{};
		int OperationCount{};
		unsigned char DmaStreamComputeIdentityCount{};
		unsigned long long DmaStreamComputeDescriptorIdentityHash{};
		unsigned long long DmaStreamComputeNormalizedFootprintHash{};
		unsigned long long DmaStreamComputeReplayEnvelopeHash{};

		static constexpr int OCCUPANCY_BITS_PER_CLASS = 4;
		static const unsigned short CurrentLaneTopologySignature;

	public:
		BundleResourceCertificateIdentity4Way(unsigned long long bundleShapeId, unsigned int classVector, unsigned short laneTopologySignature, unsigned char resourceProfile, int operationCount, unsigned char dmaStreamComputeIdentityCount = 0, unsigned long long dmaStreamComputeDescriptorIdentityHash = 0, unsigned long long dmaStreamComputeNormalizedFootprintHash = 0, unsigned long long dmaStreamComputeReplayEnvelopeHash = 0);

		/// <summary>
		/// Structural digest over the 4-way certificate's legality-relevant resource state.
		/// </summary>
		unsigned long long getBundleShapeId() const;

		/// <summary>
		/// Packed class-occupancy vector (5 classes × 4 bits).
		/// </summary>
		unsigned int getClassVector() const;

		/// <summary>
		/// Structural signature of the current lane topology and class aliases.
		/// </summary>
		unsigned short getLaneTopologySignature() const;

		/// <summary>
		/// Compact profile of which structural resource buckets are populated.
		/// </summary>
		unsigned char getResourceProfile() const;

		/// <summary>
		/// Number of operations covered by the certificate.
		/// </summary>
		int getOperationCount() const;

		/// <summary>
		/// Number of lane6 descriptor-backed compute identities carried by the certificate.
		/// Phase 03 expects this to be 0 or 1 because DmaStreamClass capacity is singleton.
		/// </summary>
		unsigned char getDmaStreamComputeIdentityCount() const;

		/// <summary>
		/// Descriptor identity hash for a descriptor-backed lane6 compute op, when present.
		/// </summary>
		unsigned long long getDmaStreamComputeDescriptorIdentityHash() const;

		/// <summary>
		/// Normalized memory-footprint identity hash for a descriptor-backed lane6 compute op, when present.
		/// </summary>
		unsigned long long getDmaStreamComputeNormalizedFootprintHash() const;

		/// <summary>
		/// Full replay evidence envelope hash for descriptor-backed lane6 compute.
		/// This is evidence only; it is not execution or commit authority.
		/// </summary>
		unsigned long long getDmaStreamComputeReplayEnvelopeHash() const;

		bool isValid() const;

		static BundleResourceCertificateIdentity4Way Create(BundleResourceCertificate4Way const certificate);

		bool Equals(BundleResourceCertificateIdentity4Way other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

		std::wstring ToString() const;

	private:
		static unsigned int EncodeClassVector(SlotClassCapacityState state);

		static unsigned char ComputeResourceProfile(BundleResourceCertificate4Way const certificate);

		static unsigned short ComputeLaneTopologySignature();

		static void CompressClass(HardwareHash &hasher, SlotClass slotClass);

		public:
        BundleResourceCertificateIdentity4Way() = default;
	};

	/// <summary>
	/// Intra-core SMT legality certificate and structural witness for the active typed-slot path.
	///
	/// Purpose: carry shared-resource state, per-VT register-group state, and typed-slot
	/// occupancy for the current bundle while exposing <see cref="StructuralIdentity"/> as
	/// the replay/template compatibility seam.
	///
	/// Design:
	/// - SharedMask (SafetyMask128): OR of all operation masks with bits 0–31 cleared.
	///   Tracks shared resources (memory domains, DMA, stream engines, accelerators, LSU).
	/// - RegMaskVT0..VT3 (uint): Per-thread register group masks (bits 0–31 only).
	///   Isolates RAW/WAW/WAR checks to the owning virtual thread.
	///
	/// Conflict resolution is two-stage:
	/// 1. Shared resource check: (SharedMask &amp; candidateShared).IsNonZero → reject.
	/// 2. Register group check: (RegMaskVT[vt] &amp; candidateReg) != 0 → reject.
	///
	/// Repository-facing architectural surfaces should describe legality in terms of the
	/// certificate's semantics and structural identity rather than raw mask layout alone.
	///
	/// HLS synthesis notes (KiWi):
	/// - Clearing bits 0–31 maps to wire truncation (zero gate delay).
	/// - The VirtualThreadId switch compiles to a parallel 4-to-1 MUX (single LUT6 layer).
	/// - No heap allocation; all fields are value types.
	/// </summary>
}
