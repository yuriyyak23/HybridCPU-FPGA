#include "ClusterIssuePreparation.h"
#include "../Legality/ClusterIssueIntent.h"
#include "../Pipeline/Scheduling/SlotPlacementMetadata.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <bit>

namespace YAKSys_Hybrid_CPU::Core
{

	ScalarClusterIssueEntry::ScalarClusterIssueEntry(DecodedBundleSlotDescriptor slotDescriptor) : ScalarClusterIssueEntry(slotDescriptor.getSlotIndex(), slotDescriptor.getVirtualThreadId(), slotDescriptor.getOwnerThreadId(), slotDescriptor.getOpCode(), slotDescriptor.getMicroOp())
	{
	}

	ScalarClusterIssueEntry::ScalarClusterIssueEntry(unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp)
		: MicroOp(std::move(microOp)), SlotIndex(slotIndex), VirtualThreadId(virtualThreadId), OwnerThreadId(ownerThreadId), OpCode(opCode)
	{
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> ScalarClusterIssueEntry::getMicroOp() const
	{
		return MicroOp;
	}

	DecodedBundleSlotDescriptor ScalarClusterIssueEntry::getSlotDescriptor() const
	{
		DecodedBundleSlotDescriptor slotDescriptor = DecodedBundleSlotDescriptor::Create(getSlotIndex(), getMicroOp());
		return DecodedBundleSlotDescriptor(slotDescriptor.getMicroOp(), slotDescriptor.getSlotIndex(), getVirtualThreadId(), getOwnerThreadId(), getOpCode(), slotDescriptor.getReadRegisters(), slotDescriptor.getWriteRegisters(), slotDescriptor.getWritesRegister(), slotDescriptor.isMemoryOp(), slotDescriptor.isControlFlow(), slotDescriptor.getPlacement(), slotDescriptor.getMemoryBankIntent(), slotDescriptor.isFspInjected(), slotDescriptor.isEmptyOrNop(), slotDescriptor.isVectorOp());
	}

	unsigned char ScalarClusterIssueEntry::getSlotIndex() const
	{
		return SlotIndex;
	}

	int ScalarClusterIssueEntry::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int ScalarClusterIssueEntry::getOwnerThreadId() const
	{
		return OwnerThreadId;
	}

	unsigned int ScalarClusterIssueEntry::getOpCode() const
	{
		return OpCode;
	}

	ScalarClusterIssueGroup::ScalarClusterIssueGroup(unsigned char candidateMask, unsigned char preparedMask, std::vector<ScalarClusterIssueEntry> entries)
		: CandidateMask(candidateMask), PreparedMask(preparedMask), Entries(std::move(entries))
	{
	}

	unsigned char ScalarClusterIssueGroup::getCandidateMask() const
	{
		return CandidateMask;
	}

	unsigned char ScalarClusterIssueGroup::getPreparedMask() const
	{
		return PreparedMask;
	}

	std::vector<ScalarClusterIssueEntry> ScalarClusterIssueGroup::getEntries() const
	{
		return Entries;
	}

	int ScalarClusterIssueGroup::getCount() const
	{
		return getEntries().size();
	}

	VtScalarCandidateSummary ScalarClusterIssueGroup::BuildVtSummary()
	{
		int vt0 = 0, vt1 = 0, vt2 = 0, vt3 = 0;
		for (int i = 0; i < getEntries().size(); i++)
		{
			switch (getEntries()[i].getVirtualThreadId())
			{
				case 0:
					vt0++;
					break;
				case 1:
					vt1++;
					break;
				case 2:
					vt2++;
					break;
				case 3:
					vt3++;
					break;
			}
		}

		return VtScalarCandidateSummary(vt0, vt1, vt2, vt3);
	}

	VtScalarCandidateSummary::VtScalarCandidateSummary(int vt0Count, int vt1Count, int vt2Count, int vt3Count)
		: Vt0Count(vt0Count), Vt1Count(vt1Count), Vt2Count(vt2Count), Vt3Count(vt3Count)
	{
	}

	int VtScalarCandidateSummary::getVt0Count() const
	{
		return Vt0Count;
	}

	int VtScalarCandidateSummary::getVt1Count() const
	{
		return Vt1Count;
	}

	int VtScalarCandidateSummary::getVt2Count() const
	{
		return Vt2Count;
	}

	int VtScalarCandidateSummary::getVt3Count() const
	{
		return Vt3Count;
	}

	int VtScalarCandidateSummary::getTotalCount() const
	{
		return getVt0Count() + getVt1Count() + getVt2Count() + getVt3Count();
	}

	int VtScalarCandidateSummary::getActiveVtCount() const
	{
		return (getVt0Count() > 0 ? 1 : 0) + (getVt1Count() > 0 ? 1 : 0) + (getVt2Count() > 0 ? 1 : 0) + (getVt3Count() > 0 ? 1 : 0);
	}

	int VtScalarCandidateSummary::GetCountForVt(int vtIndex) const
	{
		int temp_var16;
		switch (vtIndex)
		{
			case 0:
				temp_var16 = getVt0Count();
				break;
			case 1:
				temp_var16 = getVt1Count();
				break;
			case 2:
				temp_var16 = getVt2Count();
				break;
			case 3:
				temp_var16 = getVt3Count();
				break;
			default:
				temp_var16 = 0;
				break;
		}
		return temp_var16;
	}

	AuxiliaryClusterReservation::AuxiliaryClusterReservation(AuxiliaryClusterKind kind, unsigned char slotMask)
		: Kind(kind), SlotMask(slotMask)
	{
	}

	AuxiliaryClusterKind AuxiliaryClusterReservation::getKind() const
	{
		return Kind;
	}

	unsigned char AuxiliaryClusterReservation::getSlotMask() const
	{
		return SlotMask;
	}

	int AuxiliaryClusterReservation::getCount() const
	{
		return std::popcount(static_cast<unsigned int>(getSlotMask()));
	}

	ClusterIssuePreparation::ClusterIssuePreparation(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, std::shared_ptr<ScalarClusterIssueGroup> scalarClusterGroup, std::vector<AuxiliaryClusterReservation> auxiliaryReservations, unsigned char fallbackDiagnosticsMask, DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> dependencySummary)
		: PC(pc), DecodeMode(decodeMode), ScalarClusterGroup(std::move(scalarClusterGroup)), AuxiliaryReservations(std::move(auxiliaryReservations)), FallbackDiagnosticsMask(fallbackDiagnosticsMask), AdmissionPrep(std::move(admissionPrep)), DependencySummary(std::move(dependencySummary))
	{
	}

	unsigned long long ClusterIssuePreparation::getPC() const
	{
		return PC;
	}

	YAKSys_Hybrid_CPU::Core::DecodeMode ClusterIssuePreparation::getDecodeMode() const
	{
		return DecodeMode;
	}

	std::shared_ptr<ScalarClusterIssueGroup> ClusterIssuePreparation::getScalarClusterGroup() const
	{
		return ScalarClusterGroup;
	}

	std::vector<AuxiliaryClusterReservation> ClusterIssuePreparation::getAuxiliaryReservations() const
	{
		return AuxiliaryReservations;
	}

	unsigned char ClusterIssuePreparation::getFallbackDiagnosticsMask() const
	{
		return FallbackDiagnosticsMask;
	}

	DecodedBundleAdmissionPrep ClusterIssuePreparation::getAdmissionPrep() const
	{
		return AdmissionPrep;
	}

	std::optional<DecodedBundleDependencySummary> ClusterIssuePreparation::getDependencySummary() const
	{
		return DependencySummary;
	}

	bool ClusterIssuePreparation::getHasScalarClusterCandidate() const
	{
		return getAdmissionPrep().getScalarCandidateMask() != 0;
	}

	bool ClusterIssuePreparation::getSuggestFallbackDiagnostics() const
	{
		return getAdmissionPrep().getSuggestNarrowFallback();
	}

	std::shared_ptr<ClusterIssuePreparation> ClusterIssuePreparation::CreateEmpty(unsigned long long pc)
	{
		return std::shared_ptr<ClusterIssuePreparation>(new ClusterIssuePreparation(pc, DecodeMode::ReferenceSequentialMode, std::make_shared<ScalarClusterIssueGroup>(0, 0, std::vector<ScalarClusterIssueEntry>{}), std::vector<AuxiliaryClusterReservation>{}, 0, YAKSys_Hybrid_CPU::Core::DecodedBundleAdmissionPrep(), std::nullopt));
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static ClusterIssuePreparation Create(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slots, DecodedBundleAdmissionPrep admissionPrep, System.Nullable<DecodedBundleDependencySummary> dependencySummary)
	std::shared_ptr<ClusterIssuePreparation> ClusterIssuePreparation::Create(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slots, DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> &dependencySummary)
	{
		unsigned char preparedScalarMask = ResolvePreparedScalarMask(admissionPrep, dependencySummary);
		std::shared_ptr<Legality::ClusterIssueIntent> clusterIssueIntent = Legality::ClusterIssueIntentBuilder::Build(slots, preparedScalarMask);
		std::shared_ptr<ScalarClusterIssueGroup> scalarClusterGroup = clusterIssueIntent->ToScalarClusterIssueGroup();
		std::vector<AuxiliaryClusterReservation> auxiliaryReservations = BuildAuxiliaryReservations(slots, admissionPrep.getAuxiliaryOpMask());

		return CreateCore(pc, scalarClusterGroup, auxiliaryReservations, BuildFallbackDiagnosticsMask(slots, preparedScalarMask), admissionPrep, dependencySummary);
	}

	std::shared_ptr<ClusterIssuePreparation> ClusterIssuePreparation::CreateCore(unsigned long long pc, std::shared_ptr<ScalarClusterIssueGroup> scalarClusterGroup, std::vector<AuxiliaryClusterReservation> &auxiliaryReservations, unsigned char fallbackDiagnosticsMask, DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> &dependencySummary)
	{
		YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode = admissionPrep.getScalarCandidateMask() != 0 ? DecodeMode::ClusterPreparedMode : DecodeMode::ReferenceSequentialMode;

		return std::shared_ptr<ClusterIssuePreparation>(new ClusterIssuePreparation(pc, decodeMode, std::move(scalarClusterGroup), std::move(auxiliaryReservations), fallbackDiagnosticsMask, std::move(admissionPrep), dependencySummary));
	}

	unsigned char ClusterIssuePreparation::ResolvePreparedScalarMask(DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> &dependencySummary)
	{
		unsigned char preparedScalarMask = admissionPrep.getWideReadyScalarMask();
		if (!dependencySummary)
		{
			return preparedScalarMask;
		}

		unsigned char refinedPreparedScalarMask = dependencySummary.value().ComputeRefinedWideReadyScalarMask(admissionPrep.getScalarCandidateMask());
		return std::popcount(static_cast<unsigned int>(refinedPreparedScalarMask)) > std::popcount(static_cast<unsigned int>(preparedScalarMask)) ? refinedPreparedScalarMask : preparedScalarMask;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte BuildFallbackDiagnosticsMask(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte preparedScalarMask)
	unsigned char ClusterIssuePreparation::BuildFallbackDiagnosticsMask(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char preparedScalarMask)
	{
		unsigned char nonEmptyMask = 0;
		for (int slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			if (slots[slotIndex].GetRuntimeExecutionIsEmptyOrNop())
			{
				continue;
			}

			nonEmptyMask |= static_cast<unsigned char>(1 << slotIndex);
		}

		return static_cast<unsigned char>(nonEmptyMask & ~preparedScalarMask);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AuxiliaryClusterReservation[] BuildAuxiliaryReservations(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte auxiliaryOpMask)
	std::vector<AuxiliaryClusterReservation> ClusterIssuePreparation::BuildAuxiliaryReservations(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char auxiliaryOpMask)
	{
		unsigned char vectorMask = 0;
		unsigned char memoryMask = 0;
		unsigned char dmaStreamMask = 0;
		unsigned char controlMask = 0;
		unsigned char systemMask = 0;
		unsigned char otherMask = 0;

		for (int slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if ((auxiliaryOpMask & slotBit) == 0)
			{
				continue;
			}

			DecodedBundleSlotDescriptor slot = slots[slotIndex];
			switch (ClassifyAuxiliaryReservation(slot))
			{
				case AuxiliaryClusterKind::Vector:
					vectorMask |= slotBit;
					break;
				case AuxiliaryClusterKind::Memory:
					memoryMask |= slotBit;
					break;
				case AuxiliaryClusterKind::DmaStream:
					dmaStreamMask |= slotBit;
					break;
				case AuxiliaryClusterKind::Control:
					controlMask |= slotBit;
					break;
				case AuxiliaryClusterKind::System:
					systemMask |= slotBit;
					break;
				default:
					otherMask |= slotBit;
					break;
			}
		}

		std::vector<AuxiliaryClusterReservation> reservations;
		reservations.reserve(6);
		int count = 0;
		AppendReservation(reservations, count, AuxiliaryClusterKind::Vector, vectorMask);
		AppendReservation(reservations, count, AuxiliaryClusterKind::Memory, memoryMask);
		AppendReservation(reservations, count, AuxiliaryClusterKind::DmaStream, dmaStreamMask);
		AppendReservation(reservations, count, AuxiliaryClusterKind::Control, controlMask);
		AppendReservation(reservations, count, AuxiliaryClusterKind::System, systemMask);
		AppendReservation(reservations, count, AuxiliaryClusterKind::Other, otherMask);

		return reservations;
	}

	AuxiliaryClusterKind ClusterIssuePreparation::ClassifyAuxiliaryReservation(DecodedBundleSlotDescriptor slot)
	{
		SlotPlacementMetadata placement = slot.GetRuntimeAdmissionPlacement();
		bool isMemoryOp = slot.GetRuntimeAdmissionIsMemoryOp();
		bool isControlFlow = slot.GetRuntimeAdmissionIsControlFlow();

		if (placement.RequiredSlotClass == SlotClass::DmaStreamClass || placement.RequiredSlotClass == SlotClass::MatrixTileStreamClass)
		{
			return AuxiliaryClusterKind::DmaStream;
		}
		if (slot.isVectorOp())
		{
			return AuxiliaryClusterKind::Vector;
		}
		if (isMemoryOp)
		{
			return AuxiliaryClusterKind::Memory;
		}
		if (isControlFlow || placement.RequiredSlotClass == SlotClass::BranchControl)
		{
			return AuxiliaryClusterKind::Control;
		}
		if (placement.RequiredSlotClass == SlotClass::SystemSingleton)
		{
			return AuxiliaryClusterKind::System;
		}

		return AuxiliaryClusterKind::Other;
	}

	void ClusterIssuePreparation::AppendReservation(std::vector<AuxiliaryClusterReservation> &reservations, int &count, AuxiliaryClusterKind kind, unsigned char slotMask)
	{
		if (slotMask == 0)
		{
			return;
		}

		reservations.push_back(AuxiliaryClusterReservation(kind, slotMask));
		count++;
	}
}
