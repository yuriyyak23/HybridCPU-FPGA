#include "RuntimeClusterAdmissionPreparation.AuxiliaryIssueReservationSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

	AuxiliaryIssueReservationSet::AuxiliaryIssueReservationSet(unsigned char vectorMask, unsigned char memoryMask, unsigned char dmaStreamMask, unsigned char controlMask, unsigned char systemMask, unsigned char otherMask)
		: VectorMask(vectorMask), MemoryMask(memoryMask), DmaStreamMask(dmaStreamMask), ControlMask(controlMask), SystemMask(systemMask), OtherMask(otherMask)
	{
	}

	unsigned char AuxiliaryIssueReservationSet::getVectorMask() const
	{
		return VectorMask;
	}

	unsigned char AuxiliaryIssueReservationSet::getMemoryMask() const
	{
		return MemoryMask;
	}

	unsigned char AuxiliaryIssueReservationSet::getDmaStreamMask() const
	{
		return DmaStreamMask;
	}

	unsigned char AuxiliaryIssueReservationSet::getControlMask() const
	{
		return ControlMask;
	}

	unsigned char AuxiliaryIssueReservationSet::getSystemMask() const
	{
		return SystemMask;
	}

	unsigned char AuxiliaryIssueReservationSet::getOtherMask() const
	{
		return OtherMask;
	}

	unsigned char AuxiliaryIssueReservationSet::getAggregateMask() const
	{
		return static_cast<unsigned char>(getVectorMask() | getMemoryMask() | getDmaStreamMask() | getControlMask() | getSystemMask() | getOtherMask());
	}

	AuxiliaryIssueReservationSet AuxiliaryIssueReservationSet::Create(std::vector<AuxiliaryClusterReservation> &auxiliaryReservations)
	{
		unsigned char vectorMask = 0;
		unsigned char memoryMask = 0;
		unsigned char dmaStreamMask = 0;
		unsigned char controlMask = 0;
		unsigned char systemMask = 0;
		unsigned char otherMask = 0;

		for (int i = 0; i < auxiliaryReservations.size(); i++)
		{
			AuxiliaryClusterReservation reservation = auxiliaryReservations[i];
			switch (reservation.getKind())
			{
				case AuxiliaryClusterKind::Vector:
					vectorMask |= reservation.getSlotMask();
					break;
				case AuxiliaryClusterKind::Memory:
					memoryMask |= reservation.getSlotMask();
					break;
				case AuxiliaryClusterKind::DmaStream:
					dmaStreamMask |= reservation.getSlotMask();
					break;
				case AuxiliaryClusterKind::Control:
					controlMask |= reservation.getSlotMask();
					break;
				case AuxiliaryClusterKind::System:
					systemMask |= reservation.getSlotMask();
					break;
				default:
					otherMask |= reservation.getSlotMask();
					break;
			}
		}

		return AuxiliaryIssueReservationSet(vectorMask, memoryMask, dmaStreamMask, controlMask, systemMask, otherMask);
	}

	AuxiliaryIssueReservationSet AuxiliaryIssueReservationSet::CreateEmpty()
	{
		return AuxiliaryIssueReservationSet(0, 0, 0, 0, 0, 0);
	}
}
