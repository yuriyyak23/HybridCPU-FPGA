#pragma once

#include "../../Decoder/ClusterIssuePreparation.h"
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct AuxiliaryIssueReservationSet
	class AuxiliaryIssueReservationSet
	{
	private:
		const unsigned char VectorMask = 0;
		const unsigned char MemoryMask = 0;
		const unsigned char DmaStreamMask = 0;
		const unsigned char ControlMask = 0;
		const unsigned char SystemMask = 0;
		const unsigned char OtherMask = 0;

	public:
		AuxiliaryIssueReservationSet(unsigned char vectorMask, unsigned char memoryMask, unsigned char dmaStreamMask, unsigned char controlMask, unsigned char systemMask, unsigned char otherMask);

		unsigned char getVectorMask() const;
		unsigned char getMemoryMask() const;
		unsigned char getDmaStreamMask() const;
		unsigned char getControlMask() const;
		unsigned char getSystemMask() const;
		unsigned char getOtherMask() const;
		unsigned char getAggregateMask() const;

		static AuxiliaryIssueReservationSet Create(std::vector<AuxiliaryClusterReservation> &auxiliaryReservations);

		static AuxiliaryIssueReservationSet CreateEmpty();

		AuxiliaryIssueReservationSet() = default;
	};
}
