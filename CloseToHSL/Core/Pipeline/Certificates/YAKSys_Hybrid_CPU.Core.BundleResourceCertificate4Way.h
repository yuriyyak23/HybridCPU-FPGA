#pragma once

#include "SafetyMask128.h"
#include "../Scheduling/SlotLegality/SlotClassCapacity.h"

namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include <string>
#include <limits>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
    class BundleResourceCertificateIdentity4Way;
    enum class CertificateRejectDetail : unsigned char;
	/// <summary>
	/// Fine-grained rejection details from certificate check.
	/// Distinguishes shared resource vs register group conflict.
	/// <para>
	/// HLS design note: 2-bit reason MUX — no additional timing cost
	/// beyond the existing <see cref="BundleResourceCertificate4Way.CanInject"/> logic.
	/// </para>
	/// </summary>
		/// <summary>No rejection — candidate was accepted.</summary>

		/// <summary>Shared resource conflict (bits 32–127): memory domains, DMA, LSU, Accel.</summary>

		/// <summary>Per-VT register group conflict (bits 0–31): RAW, WAR, or WAW hazard.</summary>

	/// <summary>
	/// Structural replay/template identity for <see cref="BundleResourceCertificate4Way"/>.
	/// This is the explicit compatibility seam used for replay reuse instead of comparing
	/// raw shared/register mask snapshots in scheduler code.
	/// </summary>


		/// <summary>
		/// Structural digest over the 4-way certificate's legality-relevant resource state.
		/// </summary>

		/// <summary>
		/// Packed class-occupancy vector (5 classes × 4 bits).
		/// </summary>

		/// <summary>
		/// Structural signature of the current lane topology and class aliases.
		/// </summary>

		/// <summary>
		/// Compact profile of which structural resource buckets are populated.
		/// </summary>

		/// <summary>
		/// Number of operations covered by the certificate.
		/// </summary>

		/// <summary>
		/// Number of lane6 descriptor-backed compute identities carried by the certificate.
		/// Phase 03 expects this to be 0 or 1 because DmaStreamClass capacity is singleton.
		/// </summary>

		/// <summary>
		/// Descriptor identity hash for a descriptor-backed lane6 compute op, when present.
		/// </summary>

		/// <summary>
		/// Normalized memory-footprint identity hash for a descriptor-backed lane6 compute op, when present.
		/// </summary>

		/// <summary>
		/// Full replay evidence envelope hash for descriptor-backed lane6 compute.
		/// This is evidence only; it is not execution or commit authority.
		/// </summary>














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
	class BundleResourceCertificate4Way
	{
		/// <summary>
		/// Bitmask constant for isolating register group bits (Low bits 0–31).
		/// </summary>
	private:
		static constexpr unsigned long long REG_BITS_MASK_LOW32 = 0xFFFF'FFFFULL;

		/// <summary>
		/// Maximum number of virtual threads per physical core.
		/// </summary>
	public:
		static constexpr int SMT_WAYS = 4;

		/// <summary>
		/// Combined safety mask for shared (non-register) resources.
		/// Bits 0–31 are always zero; only bits 32–127 are significant.
		/// Represents the union of memory domains, LSU channels, DMA, stream engines,
		/// accelerators, and extended GRLB resources across all operations in the bundle.
		/// </summary>
		SafetyMask128 SharedMask;

		/// <summary>
		/// Register group mask for virtual thread 0 (bits 0–31 of SafetyMask128.Low).
		/// Tracks both read groups (bits 0–15) and write groups (bits 16–31).
		/// </summary>
		unsigned int RegMaskVT0 = 0;

		/// <summary>
		/// Register group mask for virtual thread 1.
		/// </summary>
		unsigned int RegMaskVT1 = 0;

		/// <summary>
		/// Register group mask for virtual thread 2.
		/// </summary>
		unsigned int RegMaskVT2 = 0;

		/// <summary>
		/// Register group mask for virtual thread 3.
		/// </summary>
		unsigned int RegMaskVT3 = 0;

		/// <summary>
		/// Number of operations packed into this bundle certificate.
		/// </summary>
		int OperationCount = 0;

		/// <summary>
		/// Per-class occupancy tracked alongside the certificate.
		/// Updated each time <see cref="AddOperation"/> is called.
		/// <para>
		/// HLS: 5 × 3-bit counters = 15 flip-flops.
		/// </para>
		/// </summary>
		SlotClassCapacityState ClassOccupancy;

		/// <summary>
		/// Count of descriptor-backed lane6 compute identities included in this certificate.
		/// Normal Phase 03 admission keeps this at 0 or 1 through DmaStreamClass capacity.
		/// </summary>
		unsigned char DmaStreamComputeIdentityCount = 0;

		/// <summary>
		/// Descriptor identity hash for descriptor-backed lane6 compute evidence.
		/// </summary>
		unsigned long long DmaStreamComputeDescriptorIdentityHash = 0;

		/// <summary>
		/// Normalized memory-footprint identity hash for descriptor-backed lane6 compute evidence.
		/// </summary>
		unsigned long long DmaStreamComputeNormalizedFootprintHash = 0;

		/// <summary>
		/// Full DmaStreamCompute replay evidence envelope hash.
		/// Replay/certificate identity is evidence only and cannot authorize execution or commit.
		/// </summary>
		unsigned long long DmaStreamComputeReplayEnvelopeHash = 0;

		/// <summary>
		/// Opaque structural identity used for replay/template matching.
		/// The certificate keeps raw masks as a local structural primitive, but callers
		/// should compare this identity rather than peeking at mask fields directly.
		/// </summary>
		BundleResourceCertificateIdentity4Way getStructuralIdentity() const;

		/// <summary>
		/// Check whether a candidate micro-operation can be injected into the bundle
		/// without resource conflicts. Two-stage O(1) check:
		/// 1. Shared resources (memory domains, DMA, LSU, Accel) — full 128-bit AND.
		/// 2. Register groups — RAR-aware 32-bit check scoped to the candidate's VirtualThreadId.
		///    Read-After-Read is permitted (reads are non-destructive); only RAW, WAR, and WAW
		///    are true hazards. This eliminates conservative false conflicts from §3b audit fix.
		///    Note: AddOperation() still OR-accumulates both read and write bits into RegMaskVT*
		///    (safe because BundleResourceCertificate4Way is a per-cycle ephemeral object rebuilt
		///    from Empty on each PackBundleIntraCoreSmt call — no AND-NOT retirement concern).
		/// </summary>
		/// <param name="candidate">Candidate micro-operation to inject</param>
		/// <returns>True if injection is safe (no conflicts)</returns>
		bool CanInject(std::shared_ptr<MicroOp> candidate);

		/// <summary>
		/// Record an operation into this certificate, updating both shared and
		/// per-thread register masks. Called after a successful CanInject check.
		///
		/// HLS: OR-accumulate into SharedMask + indexed RegMaskVT, single cycle.
		/// </summary>
		/// <param name="op">Micro-operation to add to the bundle certificate</param>
		void AddOperation(std::shared_ptr<MicroOp> op);

		/// <summary>
		/// Return the per-thread register mask for the specified virtual thread.
		/// </summary>
		unsigned int GetRegMask(int virtualThreadId);

		/// <summary>
		/// Extended CanInject that returns a structured reject reason.
		/// Same algorithm as <see cref="CanInject"/>, but annotates the rejection with
		/// <see cref="CertificateRejectDetail"/> for diagnostics/telemetry.
		/// <para>
		/// HLS: identical gate cost to <see cref="CanInject"/> + 2-bit reason MUX.
		/// </para>
		/// </summary>
		/// <param name="candidate">Candidate micro-operation to inject.</param>
		/// <param name="detail">On rejection: which stage caused the conflict.</param>
		/// <returns>True if injection is safe (no conflicts).</returns>
		bool CanInjectWithReason(std::shared_ptr<MicroOp> candidate, CertificateRejectDetail &detail);

	private:
		void AddDmaStreamComputeIdentityIfNeeded(std::shared_ptr<MicroOp> op);

		/// <summary>
		/// Empty certificate with no resources claimed.
		/// </summary>
	public:
		static const BundleResourceCertificate4Way Empty;

		std::wstring ToString();
	};
}

inline const YAKSys_Hybrid_CPU::Core::BundleResourceCertificate4Way YAKSys_Hybrid_CPU::Core::BundleResourceCertificate4Way::Empty{};
