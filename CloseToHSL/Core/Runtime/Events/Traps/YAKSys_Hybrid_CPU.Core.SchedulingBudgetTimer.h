#pragma once

#include <memory>
#include "../../Lanes/Lane7/Lane7EvidenceTypes.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class NeutralTrapResult;

	struct SchedulingBudgetTimerSnapshot final
	{
		const bool Armed;
		const unsigned long long DeadlineCycle;
		const unsigned long long TimerEpoch;
		const unsigned char TargetVtId;
		const unsigned short ExecutionDomainTag;
		const unsigned short AddressSpaceTag;
		const bool Lane7PressurePreemptionArmed;
		const unsigned long long Lane7PressureEpoch;

		SchedulingBudgetTimerSnapshot(bool armed, unsigned long long deadlineCycle, unsigned long long timerEpoch,
			unsigned char targetVtId, unsigned short executionDomainTag, unsigned short addressSpaceTag,
			bool lane7PressurePreemptionArmed, unsigned long long lane7PressureEpoch)
			: Armed(armed), DeadlineCycle(deadlineCycle), TimerEpoch(timerEpoch), TargetVtId(targetVtId),
			  ExecutionDomainTag(executionDomainTag), AddressSpaceTag(addressSpaceTag),
			  Lane7PressurePreemptionArmed(lane7PressurePreemptionArmed), Lane7PressureEpoch(lane7PressureEpoch)
		{
		}
	};

	class SchedulingBudgetTimer final : public std::enable_shared_from_this<SchedulingBudgetTimer>
	{
	private:
		bool Armed = false;
		unsigned long long DeadlineCycle = 0;
		unsigned long long TimerEpoch = 0;
		unsigned char TargetVtId = 0;
		unsigned short ExecutionDomainTag = 0;
		unsigned short AddressSpaceTag = 0;
		bool Lane7PressurePreemptionArmed = false;
		unsigned long long Lane7PressureEpoch = 0;

	public:
		bool getArmed() const;
	private:
		void setArmed(bool value);

	public:
		unsigned long long getDeadlineCycle() const;
	private:
		void setDeadlineCycle(unsigned long long value);

	public:
		unsigned long long getTimerEpoch() const;
	private:
		void setTimerEpoch(unsigned long long value);

	public:
		unsigned char getTargetVtId() const;
	private:
		void setTargetVtId(unsigned char value);

	public:
		unsigned short getExecutionDomainTag() const;
	private:
		void setExecutionDomainTag(unsigned short value);

	public:
		unsigned short getAddressSpaceTag() const;
	private:
		void setAddressSpaceTag(unsigned short value);

	public:
		bool getLane7PressurePreemptionArmed() const;
	private:
		void setLane7PressurePreemptionArmed(bool value);

	public:
		unsigned long long getLane7PressureEpoch() const;
	private:
		void setLane7PressureEpoch(unsigned long long value);

	public:
		void Arm(unsigned long long nowCycle, unsigned long long deltaCycles, unsigned char targetVtId, unsigned short executionDomainTag, unsigned short addressSpaceTag);

		void ArmForLane7Pressure(unsigned long long nowCycle, unsigned long long deltaCycles, std::shared_ptr<Lane7PressureSnapshot> pressure);

		bool TryConsumeExpired(unsigned long long nowCycle, std::shared_ptr<NeutralTrapResult> &result);

	private:
		void AdvanceEpoch();


	public:
		std::shared_ptr<SchedulingBudgetTimerSnapshot> CreateSnapshot();

		void RestoreSnapshot(std::shared_ptr<SchedulingBudgetTimerSnapshot> snapshot);
	};
}
