#pragma once

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Memory/IAtomicMemoryUnit.h"
#include "YAKSys_Hybrid_CPU.Core.Memory.MainMemoryAtomicMemoryUnit.h"
#include "../../../Pipeline/MicroOps/Control/MicroOp.Control.h"
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <mutex>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	class AtomicMemoryUnitExtensions final
	{
	private:
		inline static std::shared_ptr<MainMemoryAtomicMemoryUnit> RequireMainMemoryAtomicMemoryUnit(std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit, const std::wstring &operation);

		/// <summary>
		/// Resolve validated atomic retire intent without mutating memory.
		/// The returned effect is a retire-side carrier; reservation checks and memory writes
		/// happen only when <see cref="ApplyRetireEffect(IAtomicMemoryUnit, in AtomicRetireEffect)"/>
		/// is invoked on the authoritative retire path.
		/// </summary>
	public:
		static AtomicRetireEffect ResolveRetireEffect(std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit, unsigned short opcode, unsigned short destinationRegister, unsigned long long address, unsigned long long sourceValue, int coreId, int virtualThreadId, bool acquireOrdering = false, bool releaseOrdering = false);

		/// <summary>
		/// Apply a previously resolved atomic retire effect against the bound main-memory surface.
		/// The returned outcome reports retire-time memory mutation and any architectural
		/// destination-register writeback payload as separate facts.
		/// </summary>
		static AtomicRetireOutcome ApplyRetireEffect(std::shared_ptr<IAtomicMemoryUnit> atomicMemoryUnit, AtomicRetireEffect const effect);
	};

	class AtomicReservationRegistry final
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct Reservation
		class Reservation
		{
		private:
			const unsigned long long Address;
			const unsigned char AccessSize;

		public:
			Reservation(unsigned long long address, unsigned char accessSize);

			unsigned long long getAddress() const;

			unsigned char getAccessSize() const;

			Reservation() : Address(0), AccessSize(0) {}
		};

	private:
		static std::mutex Sync;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly Dictionary<(int CoreId, int VirtualThreadId), Reservation> Reservations = new();
		static inline std::map<std::pair<int, int>, Reservation> Reservations = {};

	public:
		static void RegisterReservation(int coreId, int virtualThreadId, unsigned long long address, unsigned char accessSize);

		static bool ConsumeReservation(int coreId, int virtualThreadId, unsigned long long address, unsigned char accessSize);

		static void ClearReservation(int coreId, int virtualThreadId);

		static void InvalidateOverlapping(unsigned long long address, int byteCount);

	};

}

