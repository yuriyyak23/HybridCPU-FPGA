#pragma once

#include "../../MicroOps/Types/MicroOp.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Retained compatibility slot admission decision: a free slot that may be
	/// filled by an operation from the specified donor virtual thread.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct FspPilferDecision
	class FspPilferDecision
	{
		/// <summary>Slot index in the bundle (0-7).</summary>
	public:
		const int SlotIndex;

		/// <summary>Virtual-thread ID of the recommended donor.</summary>
		const int DonorVtId;

		FspPilferDecision(int slotIndex, int donorVtId);

		FspPilferDecision() : SlotIndex(0), DonorVtId(0)
		{
		}
	};

	/// <summary>
	/// Retained compatibility result from <see cref="FspProcessor.TryPilfer"/>.
	/// The legacy public member names are preserved; their current semantics are
	/// metadata-gated bundle densification and slot admission.
	/// </summary>
	class FspResult final : public std::enable_shared_from_this<FspResult>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<FspPilferDecision> _decisions = new();
		std::vector<FspPilferDecision> _decisions = std::vector<FspPilferDecision>();

		/// <summary>Legacy-named ordered list of slot admission decisions.</summary>
	public:
		const std::vector<FspPilferDecision> getPilferDecisions() const;

		/// <summary><c>true</c> if at least one slot was admitted.</summary>
		bool getHasPilfers() const;

		/// <summary>Number of eligible slots found.</summary>
		int getPilferCount() const;

		void AddPilfer(int slotIndex, int donorVtId);
	};

	/// <summary>
	/// Retained compatibility processor for metadata-gated bundle densification.
	/// Current architecture vocabulary is slot admission over `BundleMetadata` and
	/// `SlotMetadata`, not a central replay or assist explanation.
	///
	/// Bounded determinism contract: given the same <paramref name="bundle"/> and
	/// <paramref name="bundleMeta"/> inputs, <see cref="TryPilfer"/> always produces
	/// the same <see cref="FspResult"/>. The donor-selection fallback uses a
	/// lowest-VT scan that is fixed for a fixed bundle snapshot.
	/// </summary>
	class FspProcessor final : public std::enable_shared_from_this<FspProcessor>
	{
		/// <summary>
		/// Identify free slots in <paramref name="bundle"/> that are eligible for
		/// slot admission according to the per-slot policy in <paramref name="bundleMeta"/>.
		/// The method does not modify <paramref name="bundle"/>; the caller is
		/// responsible for materializing any donor operation.
		/// </summary>
		/// <param name="bundle">
		/// Current bundle snapshot (array of 8 MicroOp slots; <c>null</c> or
		/// <see cref="NopMicroOp"/> slots are treated as free).
		/// </param>
		/// <param name="bundleMeta">
		/// Metadata for the bundle. Must not be <c>null</c>.
		/// </param>
		/// <returns>
		/// An <see cref="FspResult"/> listing every free, admissible slot and its
		/// recommended donor VT. Empty if none qualify or if
		/// <see cref="BundleMetadata.FspBoundary"/> is set.
		/// </returns>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public FspResult TryPilfer(MicroOp?[] bundle, BundleMetadata bundleMeta)
		std::shared_ptr<FspResult> TryPilfer(std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleMetadata> bundleMeta);

		/// <summary>
		/// Scan <paramref name="bundle"/> for the lowest virtual-thread ID that owns
		/// a filled (non-null, non-NOP) slot other than <paramref name="excludeSlot"/>.
		/// Returns <c>0xFF</c> if no filled slot is found.
		/// </summary>
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static int SelectLowestAvailableVt(MicroOp?[] bundle, int excludeSlot)
		static int SelectLowestAvailableVt(std::vector<std::shared_ptr<MicroOp>> &bundle, int excludeSlot);
	};
}
