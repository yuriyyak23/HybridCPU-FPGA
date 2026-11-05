#pragma once
#include "../Injection/EventInjectionDescriptor.h"

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualTimerState final : public std::enable_shared_from_this<VirtualTimerState>
	{
	private:
		bool Armed = false;
		unsigned long long DeadlineCycle = 0;
		unsigned long long TimerEpoch = 0;
		unsigned char TargetVtId = 0;
		unsigned short ExecutionDomainTag = 0;
		unsigned short AddressSpaceTag = 0;
		unsigned char Vector = 0;
		unsigned char Priority = 0;

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
		unsigned char getVector() const;
	private:
		void setVector(unsigned char value);

	public:
		unsigned char getPriority() const;
	private:
		void setPriority(unsigned char value);

	public:
		void Arm(unsigned long long nowCycle, unsigned long long deltaCycles, unsigned char targetVtId, unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned char vector, unsigned char priority = 0);

	private:
		void AdvanceEpoch();


	public:
		bool TryConsumeExpired(unsigned long long nowCycle, std::shared_ptr<EventInjectionDescriptor> &descriptor);
	};
}
