#pragma once

#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Scheduling
{
	/// <summary>A single retained compatibility slot admission decision.</summary>
	class SlotAdmission
	{
	public:
		const int SlotIndex;
		const int DonorVtId;
		SlotAdmission(int slotIndex, int donorVtId);
		SlotAdmission() : SlotIndex(0), DonorVtId(0) {}
	};

	/// <summary>
	/// Retained compatibility slot admission result for a single bundle cycle.
	///
	/// Produced by <see cref="FspAdmissionPolicy"/> and consumed by the
	/// instruction-fetch / dispatch stage. Each entry identifies a slot
	/// index that is free and stealable, together with the VT donor that
	/// should fill it.
	///
	/// Current architecture vocabulary should describe this as metadata-gated
	/// bundle densification and slot admission, not as a central replay or
	/// assist story.
	///
	/// A <see cref="SlotAssignment"/> is immutable once constructed.
	/// </summary>
	class SlotAssignment final : public std::enable_shared_from_this<SlotAssignment>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<SlotAdmission> _entries = new();
		std::vector<SlotAdmission> _entries = std::vector<SlotAdmission>();

		/// <summary>Ordered list of admitted (slotIndex, donorVtId) pairs.</summary>
	public:
		const std::vector<SlotAdmission> getEntries() const;

		/// <summary><c>true</c> when at least one slot was admitted.</summary>
		bool getHasAssignments() const;

		/// <summary>Number of admitted slots in this assignment.</summary>
		int getCount() const;

		/// <summary>
		/// Empty assignment returned when <see cref="BundleMetadata.FspBoundary"/>
		/// is set or when no slots are free and stealable.
		/// </summary>
		static inline const std::shared_ptr<SlotAssignment> Empty = std::make_shared<SlotAssignment>();

		void Add(int slotIndex, int donorVtId);
	};

}
