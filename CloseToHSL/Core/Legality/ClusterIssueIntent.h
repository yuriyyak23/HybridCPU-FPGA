#pragma once

#include "../Pipeline/MicroOps/Types/MicroOp.h"
#include "../Decoder/DecodedBundleDescriptor.h"
#include "../Decoder/ClusterIssuePreparation.h"
#include "../Pipeline/Metadata/BundleMetadata.h"
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <format>
#include <memory>

using namespace ::YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{
	/// <summary>
	/// Legality-layer projection of one prepared scalar-cluster entry.
	/// Mirrors the current compatibility surface without carrying runtime policy.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ClusterIssueIntentEntry
	class ClusterIssueIntentEntry
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> MicroOp;
		const unsigned char SlotIndex;
		const int VirtualThreadId;
		const int OwnerThreadId;
		const unsigned int OpCode;

	public:
		ClusterIssueIntentEntry(DecodedBundleSlotDescriptor slotDescriptor);

		ClusterIssueIntentEntry(unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp);

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> getMicroOp() const;
		DecodedBundleSlotDescriptor getSlotDescriptor() const;

		unsigned char getSlotIndex() const;

		int getVirtualThreadId() const;

		int getOwnerThreadId() const;

		unsigned int getOpCode() const;

		static ClusterIssueIntentEntry FromScalarClusterIssueEntry(ScalarClusterIssueEntry entry);

		ScalarClusterIssueEntry ToScalarClusterIssueEntry() const;

		ClusterIssueIntentEntry() = default;
	};

	/// <summary>
	/// Canonical legality-layer contract for decode-derived scalar cluster issue intent.
	/// Keeps only bundle-local scalar targeting facts and exposes compatibility
	/// projections to the existing runtime-facing group shape.
	/// </summary>
	class ClusterIssueIntent final : public std::enable_shared_from_this<ClusterIssueIntent>
	{
	private:
		const unsigned char CandidateMask;
		const unsigned char PreparedMask;

		static constexpr unsigned char AllSlotMask = static_cast<unsigned char>((1 << BundleMetadata::BundleSlotCount) - 1);
		const std::vector<ClusterIssueIntentEntry> _entries;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public ClusterIssueIntent(byte candidateMask, byte preparedMask, IReadOnlyList<ClusterIssueIntentEntry>? entries = null)
		ClusterIssueIntent(unsigned char candidateMask, unsigned char preparedMask, const std::vector<ClusterIssueIntentEntry> &entries = {});

		unsigned char getCandidateMask() const;

		unsigned char getPreparedMask() const;

		/// <summary>
		/// Prepared entries in their producer-defined order.
		/// Order is preserved so compatibility projections cannot drift from the current runtime surface.
		/// </summary>
		const std::vector<ClusterIssueIntentEntry> getPreparedEntries() const;

		/// <summary>
		/// Compatibility alias that mirrors <see cref="ScalarClusterIssueGroup.Entries"/>.
		/// </summary>
		const std::vector<ClusterIssueIntentEntry> getEntries() const;

		int getCount() const;

		VtScalarCandidateSummary BuildVtSummary();

		static std::shared_ptr<ClusterIssueIntent> CreateEmpty();

		static std::shared_ptr<ClusterIssueIntent> FromScalarClusterIssueGroup(std::shared_ptr<ScalarClusterIssueGroup> scalarClusterGroup);

		std::shared_ptr<ScalarClusterIssueGroup> ToScalarClusterIssueGroup();

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ClusterIssueIntentEntry[] NormalizeEntries(IReadOnlyList<ClusterIssueIntentEntry>? entries, byte preparedMask)
		static std::vector<ClusterIssueIntentEntry> NormalizeEntries(const std::vector<ClusterIssueIntentEntry> &entries, unsigned char preparedMask);

		static void ValidateSlotIndex(int slotIndex);

		static void ValidateSlotMask(const std::wstring &paramName, unsigned char slotMask);
	};

	/// <summary>
	/// Temporary builder that moves scalar issue-group selection out of the decoder-owned
	/// descriptor while preserving the current compatibility ordering and VT-spread heuristic.
	/// </summary>
	class ClusterIssueIntentBuilder final
	{
	private:
		static constexpr int ScalarIssueWidthCeiling = 4;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static ClusterIssueIntent Build(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte candidateMask)
		static std::shared_ptr<ClusterIssueIntent> Build(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char candidateMask);

	private:
		static bool TryGetVirtualThreadBit(int virtualThreadId, unsigned char &virtualThreadBit);
	};
}
