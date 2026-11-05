#include "SlotClassDefinitions.h"
#include <stdexcept>
#include <array>
#include "../../MicroOps/Types/MicroOp.h"
#include "../SlotPlacementMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{


	SlotClass TypedSlotBundleFacts::getSlot0Class() const
	{
		return Slot0Class;
	}

	void TypedSlotBundleFacts::setSlot0Class(SlotClass value)
	{
		Slot0Class = value;
	}

	SlotClass TypedSlotBundleFacts::getSlot1Class() const
	{
		return Slot1Class;
	}

	void TypedSlotBundleFacts::setSlot1Class(SlotClass value)
	{
		Slot1Class = value;
	}

	SlotClass TypedSlotBundleFacts::getSlot2Class() const
	{
		return Slot2Class;
	}

	void TypedSlotBundleFacts::setSlot2Class(SlotClass value)
	{
		Slot2Class = value;
	}

	SlotClass TypedSlotBundleFacts::getSlot3Class() const
	{
		return Slot3Class;
	}

	void TypedSlotBundleFacts::setSlot3Class(SlotClass value)
	{
		Slot3Class = value;
	}

	SlotClass TypedSlotBundleFacts::getSlot4Class() const
	{
		return Slot4Class;
	}

	void TypedSlotBundleFacts::setSlot4Class(SlotClass value)
	{
		Slot4Class = value;
	}

	SlotClass TypedSlotBundleFacts::getSlot5Class() const
	{
		return Slot5Class;
	}

	void TypedSlotBundleFacts::setSlot5Class(SlotClass value)
	{
		Slot5Class = value;
	}

	SlotClass TypedSlotBundleFacts::getSlot6Class() const
	{
		return Slot6Class;
	}

	void TypedSlotBundleFacts::setSlot6Class(SlotClass value)
	{
		Slot6Class = value;
	}

	SlotClass TypedSlotBundleFacts::getSlot7Class() const
	{
		return Slot7Class;
	}

	void TypedSlotBundleFacts::setSlot7Class(SlotClass value)
	{
		Slot7Class = value;
	}

	unsigned char TypedSlotBundleFacts::getPinningKindMask() const
	{
		return PinningKindMask;
	}

	void TypedSlotBundleFacts::setPinningKindMask(unsigned char value)
	{
		PinningKindMask = value;
	}

	int TypedSlotBundleFacts::getFlexibleOpCount() const
	{
		return FlexibleOpCount;
	}

	void TypedSlotBundleFacts::setFlexibleOpCount(int value)
	{
		FlexibleOpCount = value;
	}

	int TypedSlotBundleFacts::getPinnedOpCount() const
	{
		return PinnedOpCount;
	}

	void TypedSlotBundleFacts::setPinnedOpCount(int value)
	{
		PinnedOpCount = value;
	}

	unsigned char TypedSlotBundleFacts::getAluCount() const
	{
		return AluCount;
	}

	void TypedSlotBundleFacts::setAluCount(unsigned char value)
	{
		AluCount = value;
	}

	unsigned char TypedSlotBundleFacts::getLsuCount() const
	{
		return LsuCount;
	}

	void TypedSlotBundleFacts::setLsuCount(unsigned char value)
	{
		LsuCount = value;
	}

	unsigned char TypedSlotBundleFacts::getDmaStreamCount() const
	{
		return DmaStreamCount;
	}

	void TypedSlotBundleFacts::setDmaStreamCount(unsigned char value)
	{
		DmaStreamCount = value;
	}

	unsigned char TypedSlotBundleFacts::getMatrixTileStreamCount() const
	{
		return MatrixTileStreamCount;
	}

	void TypedSlotBundleFacts::setMatrixTileStreamCount(unsigned char value)
	{
		MatrixTileStreamCount = value;
	}

	unsigned char TypedSlotBundleFacts::getBranchControlCount() const
	{
		return BranchControlCount;
	}

	void TypedSlotBundleFacts::setBranchControlCount(unsigned char value)
	{
		BranchControlCount = value;
	}

	unsigned char TypedSlotBundleFacts::getSystemSingletonCount() const
	{
		return SystemSingletonCount;
	}

	void TypedSlotBundleFacts::setSystemSingletonCount(unsigned char value)
	{
		SystemSingletonCount = value;
	}

	bool TypedSlotBundleFacts::isEmpty() const
	{
		return getFlexibleOpCount() == 0 && getPinnedOpCount() == 0 && getAluCount() == 0 && getLsuCount() == 0 && getDmaStreamCount() == 0 && getMatrixTileStreamCount() == 0 && getBranchControlCount() == 0 && getSystemSingletonCount() == 0;
	}

	SlotClass TypedSlotBundleFacts::GetSlotClass(int index) const
	{
		switch (index)
		{
		case 0: return getSlot0Class();
		case 1: return getSlot1Class();
		case 2: return getSlot2Class();
		case 3: return getSlot3Class();
		case 4: return getSlot4Class();
		case 5: return getSlot5Class();
		case 6: return getSlot6Class();
		case 7: return getSlot7Class();
		default: throw std::out_of_range("Slot index must be 0–7.");
		}
	}

	bool TypedSlotBundleFacts::IsSlotPinned(int index) const
	{
		return (getPinningKindMask() & (1 << index)) != 0;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static TypedSlotBundleFacts FromBundle(MicroOp?[] bundle, int bundleWidth = 8)
	TypedSlotBundleFacts TypedSlotBundleFacts::FromBundle(std::vector<std::shared_ptr<MicroOp>> &bundle, int bundleWidth)
	{
		ArgumentNullException::ThrowIfNull(bundle);

		int limit = bundle.size() < bundleWidth ? bundle.size() : bundleWidth;

// C# TO C++ CONVERTER TASK: There is no equivalent to 'stackalloc' in C++:
		std::array<SlotClass, 8> slotClasses{};
		unsigned char pinningMask = 0;
		int flexibleCount = 0;
		int pinnedCount = 0;
		unsigned char aluCount = 0, lsuCount = 0, dmaCount = 0, matrixTileStreamCount = 0, branchCount = 0, sysCount = 0;

		for (int i = 0; i < 8; i++)
		{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (i < limit && bundle[i] != nullptr)
			{
				const auto& op = bundle[i];
				SlotPlacementMetadata placement = op->getPlacement();
				slotClasses[i] = placement.RequiredSlotClass;

				if (placement.PinningKind == SlotPinningKind::HardPinned)
				{
					pinningMask |= static_cast<unsigned char>(1 << i);
					pinnedCount++;
				}
				else
				{
					flexibleCount++;
				}

				switch (placement.RequiredSlotClass)
				{
					case SlotClass::AluClass:
						aluCount++;
						break;
					case SlotClass::LsuClass:
						lsuCount++;
						break;
					case SlotClass::DmaStreamClass:
						dmaCount++;
						break;
					case SlotClass::MatrixTileStreamClass:
						matrixTileStreamCount++;
						break;
					case SlotClass::BranchControl:
						branchCount++;
						break;
					case SlotClass::SystemSingleton:
						sysCount++;
						break;
				}
			}
			else
			{
				slotClasses[i] = SlotClass::Unclassified;
			}
		}

		TypedSlotBundleFacts temp_var = TypedSlotBundleFacts();
		temp_var.setSlot0Class(slotClasses[0]);
		temp_var.setSlot1Class(slotClasses[1]);
		temp_var.setSlot2Class(slotClasses[2]);
		temp_var.setSlot3Class(slotClasses[3]);
		temp_var.setSlot4Class(slotClasses[4]);
		temp_var.setSlot5Class(slotClasses[5]);
		temp_var.setSlot6Class(slotClasses[6]);
		temp_var.setSlot7Class(slotClasses[7]);
		temp_var.setPinningKindMask(pinningMask);
		temp_var.setFlexibleOpCount(flexibleCount);
		temp_var.setPinnedOpCount(pinnedCount);
		temp_var.setAluCount(aluCount);
		temp_var.setLsuCount(lsuCount);
		temp_var.setDmaStreamCount(dmaCount);
		temp_var.setMatrixTileStreamCount(matrixTileStreamCount);
		temp_var.setBranchControlCount(branchCount);
		temp_var.setSystemSingletonCount(sysCount);
		return temp_var;
	}

	TypedSlotBundleFacts AgreementViolationReport::getCompilerFacts() const
	{
		return CompilerFacts;
	}

	void AgreementViolationReport::setCompilerFacts(TypedSlotBundleFacts value)
	{
		CompilerFacts = value;
	}

	TypedSlotRejectReason AgreementViolationReport::getRuntimeReject() const
	{
		return RuntimeReject;
	}

	void AgreementViolationReport::setRuntimeReject(TypedSlotRejectReason value)
	{
		RuntimeReject = value;
	}

	bool AgreementViolationReport::isStructuralMismatch() const
	{
		return IsStructuralMismatch;
	}

	void AgreementViolationReport::setStructuralMismatch(bool value)
	{
		IsStructuralMismatch = value;
	}

	bool AgreementViolationReport::IsStructuralReject(TypedSlotRejectReason reason)
	{
		return (reason == TypedSlotRejectReason::StaticClassOvercommit || reason == TypedSlotRejectReason::PinnedLaneConflict);
	}

	bool AgreementViolationReport::IsDynamicReject(TypedSlotRejectReason reason)
	{
		return (reason == TypedSlotRejectReason::ScoreboardReject || reason == TypedSlotRejectReason::BankPendingReject || reason == TypedSlotRejectReason::HardwareBudgetReject || reason == TypedSlotRejectReason::AssistQuotaReject || reason == TypedSlotRejectReason::AssistBackpressureReject || reason == TypedSlotRejectReason::SpeculationBudgetReject || reason == TypedSlotRejectReason::DomainReject || reason == TypedSlotRejectReason::DynamicClassExhaustion);
	}
}
