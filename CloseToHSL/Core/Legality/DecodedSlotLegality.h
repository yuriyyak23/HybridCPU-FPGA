#pragma once

#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Decoder/HazardEffectKind.h"
#include "../Pipeline/Metadata/BundleWidth.h"
#include <string>
#include <format>

using namespace ::YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{
	/// <summary>
	/// Canonical Phase 03 legality result for one physical bundle slot.
	/// Carries only slot identity, structural class, and bundle-local peer conflict masks.
	/// Excludes cluster issue intent, scheduler placement metadata, and runtime ownership fields.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DecodedSlotLegality
	class DecodedSlotLegality
	{
	private:
		const int SlotIndex;
		const bool IsOccupied;
		const YAKSys_Hybrid_CPU::Core::SlotClass SlotClass;
		const unsigned char HardRejectPeerMask;
		const unsigned char NeedsRuntimeCheckPeerMask;
		const HazardEffectKind DominantEffectKind;

		static constexpr unsigned char AllSlotMask = static_cast<unsigned char>((1 << BundleSlotCount) - 1);

	public:
		DecodedSlotLegality(int slotIndex, bool isOccupied, YAKSys_Hybrid_CPU::Core::SlotClass slotClass = SlotClass::Unclassified, unsigned char hardRejectPeerMask = 0, unsigned char needsRuntimeCheckPeerMask = 0, HazardEffectKind dominantEffectKind = HazardEffectKind::RegisterData);

		int getSlotIndex() const;

		bool isOccupied() const;

		YAKSys_Hybrid_CPU::Core::SlotClass getSlotClass() const;

		unsigned char getHardRejectPeerMask() const;

		unsigned char getNeedsRuntimeCheckPeerMask() const;

		HazardEffectKind getDominantEffectKind() const;

		unsigned char getBlockingPeerMask() const;

		bool getHasHardRejectPeers() const;

		bool getNeedsRuntimeChecks() const;

		bool getHasPeerConflicts() const;

		bool isWideCandidate() const;

		static DecodedSlotLegality CreateEmpty(int slotIndex);

		static DecodedSlotLegality CreateOccupied(int slotIndex, YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned char hardRejectPeerMask = 0, unsigned char needsRuntimeCheckPeerMask = 0, HazardEffectKind dominantEffectKind = HazardEffectKind::RegisterData);

	private:
		static void ValidateSlotIndex(int slotIndex);

		static void ValidatePeerMask(const std::wstring &paramName, int slotIndex, unsigned char peerMask);

		DecodedSlotLegality() = default;
	};
}
