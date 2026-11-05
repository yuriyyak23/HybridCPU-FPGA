#include "BundleLegalityDescriptor.h"
#include <bit>
#include "../../../tangible_exception_helper.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Pipeline/Metadata/BundleMetadata.h"

using namespace ::YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public BundleLegalityDescriptor(ulong bundleAddress, ulong bundleSerial, byte occupiedSlotMask, DecodedBundleTypedSlotFacts typedSlotFacts = default, DecodedBundleFlags flags = DecodedBundleFlags.None, byte maxVirtualThreadSpan = 0, IReadOnlyList<DecodedSlotLegality>? slotLegalities = null, System.Nullable<DecodedBundleDependencySummary> dependencySummary = null, bool hasDecodeFault = false)
	BundleLegalityDescriptor::BundleLegalityDescriptor(unsigned long long bundleAddress, unsigned long long bundleSerial, unsigned char occupiedSlotMask, DecodedBundleTypedSlotFacts typedSlotFacts, DecodedBundleFlags flags, unsigned char maxVirtualThreadSpan, const std::vector<DecodedSlotLegality> &slotLegalities, const std::optional<DecodedBundleDependencySummary> &dependencySummary, bool hasDecodeFault) : BundleAddress(bundleAddress), BundleSerial(bundleSerial), OccupiedSlotMask(occupiedSlotMask), TypedSlotFacts(typedSlotFacts), Flags(flags), MaxVirtualThreadSpan(maxVirtualThreadSpan), DependencySummary(dependencySummary), HasDecodeFault(hasDecodeFault), _slotLegalities(NormalizeSlotLegalities(slotLegalities, occupiedSlotMask))
	{
	}

	unsigned long long BundleLegalityDescriptor::getBundleAddress() const
	{
		return BundleAddress;
	}

	unsigned long long BundleLegalityDescriptor::getBundleSerial() const
	{
		return BundleSerial;
	}

	unsigned char BundleLegalityDescriptor::getOccupiedSlotMask() const
	{
		return OccupiedSlotMask;
	}

	unsigned char BundleLegalityDescriptor::getEmptySlotMask() const
	{
		return static_cast<unsigned char>(AllSlotMask & ~getOccupiedSlotMask());
	}

	YAKSys_Hybrid_CPU::Core::DecodedBundleTypedSlotFacts BundleLegalityDescriptor::getTypedSlotFacts() const
	{
		return TypedSlotFacts;
	}

	YAKSys_Hybrid_CPU::Core::DecodedBundleFlags BundleLegalityDescriptor::getFlags() const
	{
		return Flags;
	}

	unsigned char BundleLegalityDescriptor::getMaxVirtualThreadSpan() const
	{
		return MaxVirtualThreadSpan;
	}

	const std::vector<DecodedSlotLegality> BundleLegalityDescriptor::getSlotLegalities() const
	{
		return _slotLegalities;
	}

	std::optional<DecodedBundleDependencySummary> BundleLegalityDescriptor::getDependencySummary() const
	{
		return DependencySummary;
	}

	bool BundleLegalityDescriptor::getHasDecodeFault() const
	{
		return HasDecodeFault;
	}

	int BundleLegalityDescriptor::getSlotCount() const
	{
		return BundleMetadata::BundleSlotCount;
	}

	int BundleLegalityDescriptor::getOccupiedSlotCount() const
	{
		return std::popcount(static_cast<unsigned int>(getOccupiedSlotMask()));
	}

	bool BundleLegalityDescriptor::isEmpty() const
	{
		return getOccupiedSlotMask() == 0;
	}

	bool BundleLegalityDescriptor::getHasDependencySummary() const
	{
		return getDependencySummary().has_value();
	}

	bool BundleLegalityDescriptor::getHasControlFlow() const
	{
		return (static_cast<int>(getFlags()) & static_cast<int>(YAKSys_Hybrid_CPU::Core::DecodedBundleFlags::HasControlFlow)) != 0;
	}

	bool BundleLegalityDescriptor::getHasMemoryOps() const
	{
		return (static_cast<int>(getFlags()) & static_cast<int>(YAKSys_Hybrid_CPU::Core::DecodedBundleFlags::HasMemoryOps)) != 0;
	}

	bool BundleLegalityDescriptor::getHasVectorOps() const
	{
		return (static_cast<int>(getFlags()) & static_cast<int>(YAKSys_Hybrid_CPU::Core::DecodedBundleFlags::HasVectorOps)) != 0;
	}

	bool BundleLegalityDescriptor::getHasCrossThreadSpan() const
	{
		return (static_cast<int>(getFlags()) & static_cast<int>(YAKSys_Hybrid_CPU::Core::DecodedBundleFlags::HasCrossThreadSpan)) != 0;
	}

	DecodedSlotLegality BundleLegalityDescriptor::GetSlotLegality(int index)
	{
		ValidateSlotIndex(L"index", index);
		return _slotLegalities[index];
	}

	std::shared_ptr<BundleLegalityDescriptor> BundleLegalityDescriptor::CreateEmpty(unsigned long long bundleAddress, unsigned long long bundleSerial)
	{
		return std::make_shared<BundleLegalityDescriptor>(bundleAddress, bundleSerial, 0, YAKSys_Hybrid_CPU::Core::DecodedBundleTypedSlotFacts(), DecodedBundleFlags::None, 0, std::vector<DecodedSlotLegality>(), std::nullopt, false);
	}

	std::shared_ptr<BundleLegalityDescriptor> BundleLegalityDescriptor::CreateDecodeFault(unsigned long long bundleAddress, unsigned long long bundleSerial)
	{
		return std::make_shared<BundleLegalityDescriptor>(bundleAddress, bundleSerial, 0, YAKSys_Hybrid_CPU::Core::DecodedBundleTypedSlotFacts(), DecodedBundleFlags::None, 0, std::vector<DecodedSlotLegality>(), std::nullopt, true);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedSlotLegality[] NormalizeSlotLegalities(IReadOnlyList<DecodedSlotLegality>? slotLegalities, byte occupiedSlotMask)
		std::vector<DecodedSlotLegality> BundleLegalityDescriptor::NormalizeSlotLegalities(const std::vector<DecodedSlotLegality> &slotLegalities, unsigned char occupiedSlotMask)
	{
		ValidateSlotMask(L"occupiedSlotMask", occupiedSlotMask);
		std::vector<std::optional<DecodedSlotLegality>> supplied(BundleMetadata::BundleSlotCount);
		for (const DecodedSlotLegality &slotLegality : slotLegalities)
		{
			const int slotIndex = slotLegality.getSlotIndex();
			ValidateSlotIndex(L"slotLegalities", slotIndex);

			if (supplied[slotIndex].has_value())
			{
				throw std::invalid_argument(std::format("Slot legality list contains duplicate slot {}.", slotIndex));
			}

			supplied[slotIndex].emplace(slotLegality);
		}

		std::vector<DecodedSlotLegality> normalized;
		normalized.reserve(BundleMetadata::BundleSlotCount);
		for (int slotIndex = 0; slotIndex < BundleMetadata::BundleSlotCount; slotIndex++)
		{
			const bool isOccupied = (occupiedSlotMask & (1 << slotIndex)) != 0;
			if (!supplied[slotIndex].has_value())
			{
				normalized.push_back(isOccupied ? DecodedSlotLegality::CreateOccupied(slotIndex, SlotClass::Unclassified) : DecodedSlotLegality::CreateEmpty(slotIndex));
				continue;
			}

			const DecodedSlotLegality &slotLegality = supplied[slotIndex].value();
			if (slotLegality.isOccupied() != isOccupied)
			{
				throw std::invalid_argument(std::format("Slot legality occupancy for slot {} does not match occupied slot mask 0x{:02X}.", slotIndex, static_cast<unsigned int>(occupiedSlotMask)));
			}

			normalized.push_back(slotLegality);
		}

		return normalized;
	}
	void BundleLegalityDescriptor::ValidateSlotIndex(const std::wstring &paramName, int slotIndex)
	{
		if (static_cast<unsigned int>(slotIndex) >= BundleMetadata::BundleSlotCount)
		{
			throw ArgumentOutOfRangeException(std::format("{}: Slot index must be in [0, {}].", std::string(paramName.begin(), paramName.end()), BundleMetadata::BundleSlotCount - 1));
		}
	}

	void BundleLegalityDescriptor::ValidateSlotMask(const std::wstring &paramName, unsigned char slotMask)
	{
		if ((slotMask & ~AllSlotMask) != 0)
		{
			throw ArgumentOutOfRangeException(std::format("{}: Slot mask must not set bits outside bundle width {}.", std::string(paramName.begin(), paramName.end()), BundleMetadata::BundleSlotCount));
		}
	}
}
