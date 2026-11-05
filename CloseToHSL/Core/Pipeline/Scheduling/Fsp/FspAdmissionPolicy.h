#pragma once

#include "../SlotLegality/SlotAssignment.h"
#include "../../Metadata/BundleMetadata.h"
#include "../../Metadata/SlotMetadata.h"
#include "../../Metadata/StealabilityPolicy.h"
#include "../../MicroOps/IR/InstructionIR.h"
#include <vector>
#include <stdexcept>
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Scheduling
{
	/// <summary>
	/// Retained compatibility slot admission policy for metadata-gated bundle densification.
	///
	/// The public type name is kept for older tests and adapters. Current architecture
	/// vocabulary should describe this as bundle densification / slot admission, not as
	/// the central replay or assist story.
	///
	/// The policy depends exclusively on:
	/// <list type="bullet">
	///   <item><see cref="BundleMetadata.FspBoundary"/>: when true, no free slots
	///     are admitted across this bundle boundary.</item>
	///   <item><see cref="SlotMetadata.StealabilityPolicy"/>: per-slot opt-out for
	///     privileged, control-flow, or atomic sequences.</item>
	/// </list>
	///
	/// This class deliberately has no knowledge of <c>MicroOp.CanBeStolen</c>,
	/// opcode tables, or ISA-specific fields. It is a compatibility adapter over
	/// metadata-owned slot-admission facts.
	///
	/// Bounded determinism contract: given the same <paramref name="bundle"/> and
	/// <paramref name="bundleMeta"/> inputs, <see cref="Evaluate"/> returns the same
	/// <see cref="SlotAssignment"/>.
	/// </summary>
	class FspAdmissionPolicy final : public std::enable_shared_from_this<FspAdmissionPolicy>
	{
		/// <summary>
		/// Evaluate which free slots in <paramref name="bundle"/> may be admitted for
		/// bundle densification according to the per-slot policy in
		/// <paramref name="bundleMeta"/>.
		///
		/// A slot is admitted iff:
		/// <list type="bullet">
		///   <item>The bundle does not have <see cref="BundleMetadata.FspBoundary"/> set,</item>
		///   <item>The slot position in <paramref name="bundle"/> is <see langword="null"/>
		///     and therefore free, and</item>
		///   <item>The corresponding <see cref="SlotMetadata.StealabilityPolicy"/> is
		///     <see cref="StealabilityPolicy.Stealable"/>.</item>
		/// </list>
		/// </summary>
		/// <param name="bundle">
		/// Bundle snapshot. <see langword="null"/> elements indicate free slots.
		/// Non-<see langword="null"/> elements are occupied and are never admitted.
		/// </param>
		/// <param name="bundleMeta">
		/// Per-bundle metadata containing the boundary flag and per-slot admission
		/// policies. Must not be <see langword="null"/>.
		/// </param>
		/// <param name="donorVtId">
		/// VT identifier of the thread that may fill admitted free slots.
		/// </param>
		/// <returns>
		/// A <see cref="SlotAssignment"/> listing every admitted free slot.
		/// Returns <see cref="SlotAssignment.Empty"/> when the bundle boundary blocks
		/// admission or when no free admissible slots exist.
		/// </returns>
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SlotAssignment Evaluate(IReadOnlyList<System.Nullable<InstructionIR>> bundle, BundleMetadata bundleMeta, int donorVtId)
		std::shared_ptr<SlotAssignment> Evaluate(const std::vector<std::optional<std::shared_ptr<InstructionIR>>> &bundle, std::shared_ptr<BundleMetadata> bundleMeta, int donorVtId);
	};
}

using namespace YAKSys_Hybrid_CPU::Core;
