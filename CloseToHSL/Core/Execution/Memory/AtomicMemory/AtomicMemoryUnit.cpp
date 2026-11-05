#include "AtomicMemoryUnit.h"
#include "../../../Architecture/MemoryModel/Addressing/MemoryRangeOverlap.h"
#include <typeinfo>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	std::mutex AtomicReservationRegistry::Sync;

	inline std::shared_ptr<MainMemoryAtomicMemoryUnit> AtomicMemoryUnitExtensions::RequireMainMemoryAtomicMemoryUnit(std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit, const std::wstring &operation)
	{
		ArgumentNullException::ThrowIfNull(atomicMemoryUnit);

		std::shared_ptr<MainMemoryAtomicMemoryUnit> mainMemoryAtomicMemoryUnit = std::dynamic_pointer_cast<MainMemoryAtomicMemoryUnit>(atomicMemoryUnit);
		if (mainMemoryAtomicMemoryUnit != nullptr)
		{
			return mainMemoryAtomicMemoryUnit;
		}

		const std::string actualTypeName = typeid(*atomicMemoryUnit).name();
		const std::wstring actualTypeNameWide(actualTypeName.begin(), actualTypeName.end());
		throw InvalidOperationException(std::format(L"{0} requires a core-owned MainMemoryAtomicMemoryUnit implementation, but received {1}.", operation, actualTypeNameWide));
	}

	YAKSys_Hybrid_CPU::Core::AtomicRetireEffect AtomicMemoryUnitExtensions::ResolveRetireEffect(std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit, unsigned short opcode, unsigned short destinationRegister, unsigned long long address, unsigned long long sourceValue, int coreId, int virtualThreadId, bool acquireOrdering, bool releaseOrdering)
	{
		std::shared_ptr<MainMemoryAtomicMemoryUnit> mainMemoryAtomicMemoryUnit = RequireMainMemoryAtomicMemoryUnit(atomicMemoryUnit, L"Atomic retire-effect resolution");
		unsigned char accessSize = MainMemoryAtomicMemoryUnit::ResolveAccessSize(opcode);
		mainMemoryAtomicMemoryUnit->ValidateAccess(opcode, address, accessSize);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return AtomicRetireEffect.Create(opcode, accessSize, address, sourceValue, destinationRegister, unchecked((ushort)coreId), virtualThreadId, acquireOrdering, releaseOrdering);
		return AtomicRetireEffect::Create(opcode, accessSize, address, sourceValue, destinationRegister, static_cast<unsigned short>(coreId), virtualThreadId, acquireOrdering, releaseOrdering);
	}

	YAKSys_Hybrid_CPU::Core::AtomicRetireOutcome AtomicMemoryUnitExtensions::ApplyRetireEffect(std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit, AtomicRetireEffect const effect)
	{
		if (!effect.isValid())
		{
			return YAKSys_Hybrid_CPU::Core::AtomicRetireOutcome();
		}

		return RequireMainMemoryAtomicMemoryUnit(atomicMemoryUnit, L"Atomic retire/apply")->ApplyResolvedRetireEffect(effect);
	}

	AtomicReservationRegistry::Reservation::Reservation(unsigned long long address, unsigned char accessSize) : Address(address), AccessSize(accessSize)
	{
	}

	unsigned long long AtomicReservationRegistry::Reservation::getAddress() const
	{
		return Address;
	}

	unsigned char AtomicReservationRegistry::Reservation::getAccessSize() const
	{
		return AccessSize;
	}

	void AtomicReservationRegistry::RegisterReservation(int coreId, int virtualThreadId, unsigned long long address, unsigned char accessSize)
	{
		{
			std::scoped_lock<std::mutex> lock(Sync);
			const std::pair<int, int> key = std::make_pair(coreId, virtualThreadId);
			Reservations.erase(key);
			Reservations.emplace(key, Reservation(address, accessSize));
		}
	}

	bool AtomicReservationRegistry::ConsumeReservation(int coreId, int virtualThreadId, unsigned long long address, unsigned char accessSize)
	{
		{
			std::scoped_lock<std::mutex> lock(Sync);
// C# TO C++ CONVERTER TASK: Tuple variables are not converted by C# to C++ Converter:
			std::pair<int, int> key = std::make_pair(coreId, virtualThreadId);
			std::map<std::pair<int, int>, Reservation>::const_iterator Reservations_iterator = Reservations.find(key);
			if (Reservations_iterator == Reservations.end()) return false;
			const Reservation reservation = Reservations_iterator->second;

			Reservations.erase(key);
			return reservation.getAddress() == address && reservation.getAccessSize() == accessSize;
		}
	}

	void AtomicReservationRegistry::ClearReservation(int coreId, int virtualThreadId)
	{
		{
			std::scoped_lock<std::mutex> lock(Sync);
			Reservations.erase(std::make_pair(coreId, virtualThreadId));
		}
	}

	void AtomicReservationRegistry::InvalidateOverlapping(unsigned long long address, int byteCount)
	{
		if (byteCount <= 0)
		{
			return;
		}

		unsigned long long length = static_cast<unsigned long long>(byteCount);
		{
			std::scoped_lock<std::mutex> lock(Sync);
			std::vector<std::pair<int, int>> staleReservations;
			for (auto entry : Reservations)
			{
				if (MemoryRangeOverlap::RangesOverlap(address, length, entry.second.getAddress(), entry.second.getAccessSize()))
				{
					staleReservations.push_back(entry.first);
				}
			}

			for (int i = 0; i < staleReservations.size(); i++)
			{
				Reservations.erase(staleReservations[i]);
			}
		}
	}
}


