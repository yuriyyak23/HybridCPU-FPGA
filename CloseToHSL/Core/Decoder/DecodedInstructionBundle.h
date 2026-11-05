#pragma once

#include "../Pipeline/AbstractBundle.h"
namespace YAKSys_Hybrid_CPU::Core { class BundleMetadata; }
namespace YAKSys_Hybrid_CPU::Core::Decoder { class DecodedInstruction; class CanonicalBundle; }
#include <vector>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Pipeline;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	/// <summary>
	/// Canonical phase 03 decoder contract for one decoded frontend bundle.
	/// Carries only bundle identity plus per-slot <see cref="DecodedInstruction"/> records.
	/// </summary>
	class DecodedInstructionBundle final : public IAbstractBundle
	{
	private:
		const unsigned long long BundleAddress;
		const unsigned long long BundleSerial;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> BundleMetadata;
		const bool HasDecodeFault;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> CanonicalBundle;

		const std::vector<std::shared_ptr<DecodedInstruction>> _slots;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public DecodedInstructionBundle(ulong bundleAddress, ulong bundleSerial, IReadOnlyList<DecodedInstruction>? slots = null, System.Nullable<BundleMetadata> bundleMetadata = null, bool hasDecodeFault = false, System.Nullable<CanonicalBundle> canonicalBundle = null)
		DecodedInstructionBundle(unsigned long long bundleAddress, unsigned long long bundleSerial, const std::vector<std::shared_ptr<DecodedInstruction>> &slots = {}, const std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>> &bundleMetadata = std::nullopt, bool hasDecodeFault = false, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle = nullptr);

		unsigned long long getBundleAddress() const override;

		unsigned long long getBundleSerial() const override;

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> getBundleMetadata() const;

		/// <summary>
		/// True when the canonical frontend failed to decode this fetched bundle and the
		/// runtime had to continue through the fallback materializer contour.
		/// This does not grant semantic authority to the fallback path.
		/// </summary>
		bool getHasDecodeFault() const;

		/// <summary>
		/// RF-04 immutable semantic projection. It is populated by the live legacy
		/// decoder in shadow mode; legacy adapter consumers remain unchanged until
		/// RF-06 migration closes.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CanonicalBundle? CanonicalBundle {get;}
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> getCanonicalBundle() const;

		int getSlotCount() const override;

		const std::vector<std::shared_ptr<DecodedInstruction>> getSlots() const;

		bool isEmpty() const override;

		int getOccupiedSlotCount() const;

		std::shared_ptr<DecodedInstruction> GetDecodedSlot(int index);

		std::shared_ptr<IAbstractBundleSlot> GetSlot(int index) override;

		static std::shared_ptr<DecodedInstructionBundle> CreateEmpty(unsigned long long bundleAddress, unsigned long long bundleSerial = 0);

		static std::shared_ptr<DecodedInstructionBundle> CreateDecodeFault(unsigned long long bundleAddress, unsigned long long bundleSerial = 0);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedInstruction[] NormalizeSlots(IReadOnlyList<DecodedInstruction>? slots)
		static std::vector<std::shared_ptr<DecodedInstruction>> NormalizeSlots(const std::vector<std::shared_ptr<DecodedInstruction>> &slots);
	};
}
