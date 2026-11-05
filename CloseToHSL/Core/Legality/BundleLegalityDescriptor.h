#pragma once

#include "../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"
#include "../Decoder/DecodedBundleDescriptor.h"
#include "DecodedSlotLegality.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>

using namespace ::YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{
	/// <summary>
	/// Canonical Phase 03 legality-only descriptor for one decoded bundle.
	/// Carries bundle identity, slot occupancy, typed slot facts, and the
	/// bundle-local hazard summary, but excludes cluster issue preparation,
	/// narrow-vs-wide admission prep, and scheduler-specific artifacts.
	/// </summary>
	class BundleLegalityDescriptor final : public std::enable_shared_from_this<BundleLegalityDescriptor>
	{
	private:
		const unsigned long long BundleAddress;
		const unsigned long long BundleSerial;
		const unsigned char OccupiedSlotMask;
		const DecodedBundleTypedSlotFacts TypedSlotFacts;
		const DecodedBundleFlags Flags;
		const unsigned char MaxVirtualThreadSpan;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<DecodedBundleDependencySummary> DependencySummary;
		const std::optional<DecodedBundleDependencySummary> DependencySummary;
		const bool HasDecodeFault;

		static constexpr unsigned char AllSlotMask = static_cast<unsigned char>((1 << BundleSlotCount) - 1);
		const std::vector<DecodedSlotLegality> _slotLegalities;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public BundleLegalityDescriptor(ulong bundleAddress, ulong bundleSerial, byte occupiedSlotMask, DecodedBundleTypedSlotFacts typedSlotFacts = default, DecodedBundleFlags flags = DecodedBundleFlags.None, byte maxVirtualThreadSpan = 0, IReadOnlyList<DecodedSlotLegality>? slotLegalities = null, System.Nullable<DecodedBundleDependencySummary> dependencySummary = null, bool hasDecodeFault = false)
		BundleLegalityDescriptor(unsigned long long bundleAddress, unsigned long long bundleSerial, unsigned char occupiedSlotMask, DecodedBundleTypedSlotFacts typedSlotFacts = YAKSys_Hybrid_CPU::Core::DecodedBundleTypedSlotFacts(), DecodedBundleFlags flags = DecodedBundleFlags::None, unsigned char maxVirtualThreadSpan = 0, const std::vector<DecodedSlotLegality> &slotLegalities = {}, const std::optional<DecodedBundleDependencySummary> &dependencySummary = std::nullopt, bool hasDecodeFault = false);

		unsigned long long getBundleAddress() const;

		unsigned long long getBundleSerial() const;

		unsigned char getOccupiedSlotMask() const;

		unsigned char getEmptySlotMask() const;

		DecodedBundleTypedSlotFacts getTypedSlotFacts() const;

		DecodedBundleFlags getFlags() const;

		unsigned char getMaxVirtualThreadSpan() const;

		const std::vector<DecodedSlotLegality> getSlotLegalities() const;

		std::optional<DecodedBundleDependencySummary> getDependencySummary() const;

		/// <summary>
		/// True when canonical bundle decode faulted and the runtime continued through the
		/// fallback materializer contour. Legality authority remains absent in this state.
		/// </summary>
		bool getHasDecodeFault() const;

		int getSlotCount() const;

		int getOccupiedSlotCount() const;

		bool isEmpty() const;

		bool getHasDependencySummary() const;

		bool getHasControlFlow() const;

		bool getHasMemoryOps() const;

		bool getHasVectorOps() const;

		bool getHasCrossThreadSpan() const;

		DecodedSlotLegality GetSlotLegality(int index);

		static std::shared_ptr<BundleLegalityDescriptor> CreateEmpty(unsigned long long bundleAddress, unsigned long long bundleSerial = 0);

		static std::shared_ptr<BundleLegalityDescriptor> CreateDecodeFault(unsigned long long bundleAddress, unsigned long long bundleSerial = 0);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedSlotLegality[] NormalizeSlotLegalities(IReadOnlyList<DecodedSlotLegality>? slotLegalities, byte occupiedSlotMask)
		static std::vector<DecodedSlotLegality> NormalizeSlotLegalities(const std::vector<DecodedSlotLegality> &slotLegalities, unsigned char occupiedSlotMask);


		static void ValidateSlotIndex(const std::wstring &paramName, int slotIndex);

		static void ValidateSlotMask(const std::wstring &paramName, unsigned char slotMask);
	};
}
