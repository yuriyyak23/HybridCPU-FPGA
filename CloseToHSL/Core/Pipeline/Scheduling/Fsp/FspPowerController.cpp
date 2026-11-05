#include "FspPowerController.h"
#include "../SlotLegality/SlotClassDefinitions.h"

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long FspPowerController::getTotalGatedCycles() const
	{
		return TotalGatedCycles;
	}

	void FspPowerController::setTotalGatedCycles(unsigned long long value)
	{
		TotalGatedCycles = value;
	}

	unsigned long long FspPowerController::getGateTransitions() const
	{
		return GateTransitions;
	}

	void FspPowerController::setGateTransitions(unsigned long long value)
	{
		GateTransitions = value;
	}

	unsigned long long FspPowerController::getClassCapacityEarlyGateCycles() const
	{
		return ClassCapacityEarlyGateCycles;
	}

	void FspPowerController::setClassCapacityEarlyGateCycles(unsigned long long value)
	{
		ClassCapacityEarlyGateCycles = value;
	}

	int FspPowerController::getCurrentGatedCount() const
	{
		int count = 0;
		for (int t = 0; t < MAX_THREADS; t++)
		{
			if (_gateStates[t])
			{
				count++;
			}
		}
		return count;
	}

	void FspPowerController::UpdateFromFsp(unsigned short readyMask)
	{
		for (int t = 0; t < MAX_THREADS; t++)
		{
			bool hasVisibleNomination = ((readyMask >> t) & 1) != 0;
			if (hasVisibleNomination)
			{
				// Thread has work → ungate immediately (1-cycle wakeup)
				if (_gateStates[t])
				{
					_gateStates[t] = false;
					setGateTransitions(getGateTransitions() + 1);
				}
				_idleCounters[t] = 0;
			}
			else
			{
				// Thread idle → increment hysteresis counter
				_idleCounters[t]++;

				if (_idleCounters[t] >= IDLE_HYSTERESIS_CYCLES && !_gateStates[t])
				{
					_gateStates[t] = true;
					setGateTransitions(getGateTransitions() + 1);
				}
			}

			// Accumulate gated-cycle statistics
			if (_gateStates[t])
			{
				setTotalGatedCycles(getTotalGatedCycles() + 1);
			}
		}
	}

	void FspPowerController::UpdateFromFspWithClassCapacity(unsigned short readyMask, SlotClassCapacityState classCapacity)
	{
		if (IsClassCapacityExhausted(classCapacity))
		{
			// No class-flexible capacity means the compatibility-era densification path is idle.
			// Run the standard update with all ports forced invalid.
			setClassCapacityEarlyGateCycles(getClassCapacityEarlyGateCycles() + 1);
			UpdateFromFsp(0);
			return;
		}

		UpdateFromFsp(readyMask);
	}

	inline bool FspPowerController::IsClassCapacityExhausted(SlotClassCapacityState classCapacity)
	{
		return !classCapacity.HasFreeCapacity(SlotClass::AluClass) && !classCapacity.HasFreeCapacity(SlotClass::LsuClass);
	}

	inline bool FspPowerController::IsThreadGated(int threadId) const
	{
		return static_cast<unsigned int>(threadId) < MAX_THREADS && _gateStates[threadId];
	}

	inline void FspPowerController::WakeThread(int threadId)
	{
		if (static_cast<unsigned int>(threadId) < MAX_THREADS && _gateStates[threadId])
		{
			_gateStates[threadId] = false;
			_idleCounters[threadId] = 0;
			setGateTransitions(getGateTransitions() + 1);
		}
	}

	void FspPowerController::WakeAll()
	{
		for (int t = 0; t < MAX_THREADS; t++)
		{
			if (_gateStates[t])
			{
				_gateStates[t] = false;
				_idleCounters[t] = 0;
				setGateTransitions(getGateTransitions() + 1);
			}
		}
	}

	double FspPowerController::GetPowerSavingsRatio(unsigned long long totalCycles) const
	{
		if (totalCycles == 0)
		{
			return 0.0;
		}
		// Each gated thread-cycle saves FETCH_DECODE_POWER_FRACTION of per-thread power
		return static_cast<double>(getTotalGatedCycles()) / (totalCycles * MAX_THREADS) * FETCH_DECODE_POWER_FRACTION;
	}

	void FspPowerController::Reset()
	{
		_idleCounters.fill(0);
		_gateStates.fill(false);
		setTotalGatedCycles(0);
		setGateTransitions(0);
		setClassCapacityEarlyGateCycles(0);
	}
}
