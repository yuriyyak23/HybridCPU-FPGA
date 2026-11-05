#include "DecodedSlotLegality.h"
#include <stdexcept>

using namespace ::YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{

    DecodedSlotLegality::DecodedSlotLegality(int slotIndex, bool isOccupied, YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned char hardRejectPeerMask, unsigned char needsRuntimeCheckPeerMask, HazardEffectKind dominantEffectKind)
        : SlotIndex(slotIndex), IsOccupied(isOccupied), SlotClass(isOccupied ? slotClass : YAKSys_Hybrid_CPU::Core::SlotClass::Unclassified),
          HardRejectPeerMask(isOccupied ? hardRejectPeerMask : 0), NeedsRuntimeCheckPeerMask(isOccupied ? needsRuntimeCheckPeerMask : 0), DominantEffectKind(dominantEffectKind)
    {
        ValidateSlotIndex(slotIndex);
        ValidatePeerMask(L"hardRejectPeerMask", slotIndex, hardRejectPeerMask);
        ValidatePeerMask(L"needsRuntimeCheckPeerMask", slotIndex, needsRuntimeCheckPeerMask);
    }
	int DecodedSlotLegality::getSlotIndex() const
	{
		return SlotIndex;
	}

	bool DecodedSlotLegality::isOccupied() const
	{
		return IsOccupied;
	}

	YAKSys_Hybrid_CPU::Core::SlotClass DecodedSlotLegality::getSlotClass() const
	{
		return SlotClass;
	}

	unsigned char DecodedSlotLegality::getHardRejectPeerMask() const
	{
		return HardRejectPeerMask;
	}

	unsigned char DecodedSlotLegality::getNeedsRuntimeCheckPeerMask() const
	{
		return NeedsRuntimeCheckPeerMask;
	}

	YAKSys_Hybrid_CPU::Core::HazardEffectKind DecodedSlotLegality::getDominantEffectKind() const
	{
		return DominantEffectKind;
	}

	unsigned char DecodedSlotLegality::getBlockingPeerMask() const
	{
		return static_cast<unsigned char>(getHardRejectPeerMask() | getNeedsRuntimeCheckPeerMask());
	}

	bool DecodedSlotLegality::getHasHardRejectPeers() const
	{
		return getHardRejectPeerMask() != 0;
	}

	bool DecodedSlotLegality::getNeedsRuntimeChecks() const
	{
		return getNeedsRuntimeCheckPeerMask() != 0;
	}

	bool DecodedSlotLegality::getHasPeerConflicts() const
	{
		return getBlockingPeerMask() != 0;
	}

	bool DecodedSlotLegality::isWideCandidate() const
	{
		return isOccupied() && getHardRejectPeerMask() == 0;
	}

	DecodedSlotLegality DecodedSlotLegality::CreateEmpty(int slotIndex)
	{
		return DecodedSlotLegality(slotIndex, false);
	}

	DecodedSlotLegality DecodedSlotLegality::CreateOccupied(int slotIndex, YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned char hardRejectPeerMask, unsigned char needsRuntimeCheckPeerMask, HazardEffectKind dominantEffectKind)
	{
		return DecodedSlotLegality(slotIndex, true, slotClass, hardRejectPeerMask, needsRuntimeCheckPeerMask, dominantEffectKind);
	}

	void DecodedSlotLegality::ValidateSlotIndex(int slotIndex)
	{
		if (static_cast<unsigned int>(slotIndex) >= BundleSlotCount)
		{
			throw std::out_of_range("Slot index is outside bundle width.");
		}
	}

	void DecodedSlotLegality::ValidatePeerMask(const std::wstring &paramName, int slotIndex, unsigned char peerMask)
	{
		if ((peerMask & ~AllSlotMask) != 0)
		{
			throw std::out_of_range("Peer mask sets bits outside bundle width.");
		}

		unsigned char selfBit = static_cast<unsigned char>(1 << slotIndex);
		if ((peerMask & selfBit) != 0)
		{
			throw std::out_of_range("Peer mask must not include the slot itself.");
		}
	}
}
