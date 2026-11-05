#pragma once

#include <vector>
#include <memory>

// C# TO C++ CONVERTER TASK: C# to C++ Converter could not confirm whether this is a namespace alias or a type alias:
//using CoreBundleMetadata = YAKSys_Hybrid_CPU::Core::BundleMetadata;
// C# TO C++ CONVERTER TASK: C# to C++ Converter could not confirm whether this is a namespace alias or a type alias:
//using CoreSlotMetadata = YAKSys_Hybrid_CPU::Core::SlotMetadata;

namespace YAKSys_Hybrid_CPU::Core { class BundleMetadata; }
namespace HybridCPU_ISE::Arch { class InstructionSlotMetadata; }

namespace HybridCPU_ISE::Arch
{
	/// <summary>
	/// Sideband slot annotations that travel with a VT-local instruction stream until lowering.
	/// </summary>
	class VliwBundleAnnotations final : public std::enable_shared_from_this<VliwBundleAnnotations>
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> BundleMetadata;

		const std::vector<std::shared_ptr<InstructionSlotMetadata>> _slotMetadata;

	public:
		static std::shared_ptr<VliwBundleAnnotations> getEmpty();

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> getBundleMetadata() const;

		VliwBundleAnnotations(const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata);

		VliwBundleAnnotations(const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata, std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> bundleMetadata);

		int getCount() const;

		bool TryGetInstructionSlotMetadata(int instructionIndex, std::shared_ptr<InstructionSlotMetadata> &metadata);

	private:
		static std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> BuildBundleMetadata(const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata);
	};
}
