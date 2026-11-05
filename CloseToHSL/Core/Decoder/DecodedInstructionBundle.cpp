#include "DecodedInstructionBundle.h"
#include "CanonicalDecodedContracts.h"
#include "DecodedInstruction.h"
#include "../Pipeline/Metadata/BundleMetadata.h"

using namespace YAKSys_Hybrid_CPU::Core::Pipeline;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public DecodedInstructionBundle(ulong bundleAddress, ulong bundleSerial, IReadOnlyList<DecodedInstruction>? slots = null, System.Nullable<BundleMetadata> bundleMetadata = null, bool hasDecodeFault = false, System.Nullable<CanonicalBundle> canonicalBundle = null)
	DecodedInstructionBundle::DecodedInstructionBundle(unsigned long long bundleAddress, unsigned long long bundleSerial, const std::vector<std::shared_ptr<DecodedInstruction>> &slots, const std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>> &bundleMetadata, bool hasDecodeFault, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle)
		: BundleAddress(bundleAddress), BundleSerial(bundleSerial),
		  BundleMetadata(bundleMetadata.has_value() && bundleMetadata.value() != nullptr ? bundleMetadata.value() : YAKSys_Hybrid_CPU::Core::BundleMetadata::GetDefault()),
		  HasDecodeFault(hasDecodeFault), CanonicalBundle(canonicalBundle), _slots(NormalizeSlots(slots))
	{
		if (canonicalBundle != nullptr && (canonicalBundle->getBundleAddress() != bundleAddress || canonicalBundle->getBundleSerial() != bundleSerial || canonicalBundle->getSlots().size() != static_cast<std::size_t>(YAKSys_Hybrid_CPU::Core::BundleMetadata::BundleSlotCount)))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Canonical bundle identity must match its legacy adapter.", nameof(canonicalBundle));
			throw std::invalid_argument("Canonical bundle identity must match its legacy adapter.");
		}

	}

	unsigned long long DecodedInstructionBundle::getBundleAddress() const
	{
		return BundleAddress;
	}

	unsigned long long DecodedInstructionBundle::getBundleSerial() const
	{
		return BundleSerial;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> DecodedInstructionBundle::getBundleMetadata() const
	{
		return BundleMetadata;
	}

	bool DecodedInstructionBundle::getHasDecodeFault() const
	{
		return HasDecodeFault;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> DecodedInstructionBundle::getCanonicalBundle() const
	{
		return CanonicalBundle;
	}

	int DecodedInstructionBundle::getSlotCount() const
	{
		return _slots.size();
	}

	const std::vector<std::shared_ptr<DecodedInstruction>> DecodedInstructionBundle::getSlots() const
	{
		return _slots;
	}

	bool DecodedInstructionBundle::isEmpty() const
	{
		for (int i = 0; i < _slots.size(); i++)
		{
			if (_slots[i] != nullptr && _slots[i]->IsOccupied)
			{
				return false;
			}
		}

		return true;
	}

	int DecodedInstructionBundle::getOccupiedSlotCount() const
	{
		int occupied = 0;
		for (int i = 0; i < _slots.size(); i++)
		{
			if (_slots[i] != nullptr && _slots[i]->IsOccupied)
			{
				occupied++;
			}
		}

		return occupied;
	}

	std::shared_ptr<DecodedInstruction> DecodedInstructionBundle::GetDecodedSlot(int index)
	{
		if (static_cast<unsigned int>(index) >= static_cast<unsigned int>(_slots.size()))
		{
			throw ArgumentOutOfRangeException(std::format("Slot index must be in [0, {}].", _slots.size() - 1));
		}

		return _slots[index];
	}

	std::shared_ptr<IAbstractBundleSlot> DecodedInstructionBundle::GetSlot(int index)
	{
		return GetDecodedSlot(index);
	}

	std::shared_ptr<DecodedInstructionBundle> DecodedInstructionBundle::CreateEmpty(unsigned long long bundleAddress, unsigned long long bundleSerial)
	{
		return std::make_shared<DecodedInstructionBundle>(bundleAddress, bundleSerial, std::vector<std::shared_ptr<DecodedInstruction>>{}, std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>>(YAKSys_Hybrid_CPU::Core::BundleMetadata::GetDefault()));
	}

	std::shared_ptr<DecodedInstructionBundle> DecodedInstructionBundle::CreateDecodeFault(unsigned long long bundleAddress, unsigned long long bundleSerial)
	{
		return std::make_shared<DecodedInstructionBundle>(bundleAddress, bundleSerial, std::vector<std::shared_ptr<DecodedInstruction>>{}, std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>>(YAKSys_Hybrid_CPU::Core::BundleMetadata::GetDefault()), true);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedInstruction[] NormalizeSlots(IReadOnlyList<DecodedInstruction>? slots)
	std::vector<std::shared_ptr<DecodedInstruction>> DecodedInstructionBundle::NormalizeSlots(const std::vector<std::shared_ptr<DecodedInstruction>> &slots)
	{
		std::vector<std::shared_ptr<DecodedInstruction>> normalized(YAKSys_Hybrid_CPU::Core::BundleMetadata::BundleSlotCount);
		std::vector<bool> seen(normalized.size());
		for (int i = 0; i < normalized.size(); i++)
		{
			normalized[i] = DecodedInstruction::CreateEmpty(i);
		}

		if (slots.empty())
		{
			return normalized;
		}

		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i] == nullptr)
			{
				throw std::invalid_argument("Decoded bundle slots must not contain null entries.");
			}
			std::shared_ptr<DecodedInstruction> slot = slots[i];
			int slotIndex = slot->SlotIndex;

			if (static_cast<unsigned int>(slotIndex) >= static_cast<unsigned int>(normalized.size()))
			{
				throw ArgumentOutOfRangeException(std::format("Decoded slot index {} is outside bundle width {}.", slotIndex, normalized.size()));
			}

			if (seen[slotIndex])
			{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Decoded bundle contains duplicate slot {0}.", slotIndex), nameof(slots));
				throw std::invalid_argument(std::format("Decoded bundle contains duplicate slot {}.", slotIndex));
			}

			normalized[slotIndex] = slot;
			seen[slotIndex] = true;
		}

		return normalized;
	}
}
